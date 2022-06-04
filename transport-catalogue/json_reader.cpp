#include "json_reader.h"
#include <sstream>
#include <iterator>
#include <iostream>
#include <variant>
#include <algorithm>

using namespace catalogue::input;
using namespace catalogue::detail;
using namespace json;
using namespace std::literals;

namespace catalogue
{
    namespace input
    {
        bool JsonReader::Check(std::istream& stream, std::string&& str)
        {
            for(const char c : str)
            {
                char tmp;
                stream >> tmp;

                if(tmp != c || stream.eof())
                {
                    throw ParsingError("false token error");
                }
            }
            return true;
        }

        Node JsonReader::LoadNode(std::istream& input) 
        {
            char c;
            input >> c;

            switch (c)
            {
                case '{' :
                    return LoadDict(input);
                    break;

                case '[' :
                    return LoadArray(input);
                    break;

                case '"' :
                    return LoadString(input);
                    break;

                case '0' :
                case '1' :
                case '2' :
                case '3' :
                case '4' :
                case '5' :
                case '6' :
                case '7' :
                case '8' :
                case '9' :
                case '-' :
                    input.putback(c);
                    return LoadNum(input);
                    break;

                case 'f' :
                    Check(input, "alse");
                    return Node(false);
                    break;

                case 't' :
                    Check(input, "rue");
                    return Node(true);
                    break;

                case 'n' :
                    Check(input, "ull");
                    return Node();
                    break;

                default:
                    throw ParsingError("error symbol");
                    break;
            }
            return Node();
        }

        Node JsonReader::LoadArray(std::istream& in) 
        {
            Array result;

            for (char c; in >> c && c != ']';) 
            {
                if(c == '{' || c == '[')
                {
                    in.putback(c);
                    result.push_back(std::move(LoadNode(in)));
                    continue;
                }

                if (c != ',') 
                {
                    in.putback(c);
                }
                result.push_back(std::move(LoadNode(in)));
            }

            if(in.eof())
            {
                throw ParsingError("eof");
            }
            return Node(move(result));
        }

        Node JsonReader::LoadNum(std::istream& in) 
        {
            std::string res;
            bool is_double = false;

            char next;
            in >> next;

            while(next != ',' && next != ']' && next != '}' && !in.eof())
            {
                if(next == '.' || next == 'e' || next == 'E')
                {
                    is_double = true;
                }
                res += next;
                in >> next;
            }

            if(next == ']' || next == '}')
            {
                in.putback(next);
            }

            if(is_double)
            {
                return Node(std::stod(res));
            }
            return Node(std::stoi(res));
        }

        Node JsonReader::LoadString(std::istream& in) 
        {
            std::string line;

            char c;
            c = in.get();
            
            while(c != '"')
            {
                if(c == '\\')
                {
                    c = in.get();

                    switch (c)
                    {
                    case 'n':
                        line += '\n';
                        break;

                    case 't':
                        line += '\t';
                        break;

                    case 'r':
                        line += '\r';
                        break;

                    case '"':
                        line += '\"';
                        break;

                    case '\\':
                        line += '\\';
                        break;
                    
                    default:
                        throw ParsingError("error symbol");
                        break;
                    }
                }
                else if(c == '\n' || c == '\r')
                {
                    throw ParsingError("error n or r");
                }
                else 
                {
                    line += c;
                }

                c = in.get();

                if(in.eof())
                {
                    throw ParsingError("eof");
                }
            }
            return {std::move(line)};
        }

        Node JsonReader::LoadDict(std::istream& in) 
        {
            Dict result;

            for (char c; in >> c && c != '}';) 
            {
                if (c == ',') 
                {
                    in >> c;
                }

                std::string key = LoadString(in).AsString();

                in >> c;

                Node node = LoadNode(in);

                result.emplace(std::move(key), std::move(node));
            }

            if(in.eof())
            {
                throw ParsingError("eof");
            }

            return Node(move(result));
        }

        void JsonReader::Load(std::istream& input) 
        {
            document_ = MakeDocument(input);
        }

        Document& JsonReader::Get()
        {
            return document_;
        }

        RenderSetup&& JsonReader::GetRenderSetup() const
        {
            static RenderSetup result;

            if(!document_.GetRoot().AsDict().count("render_settings"))
            {
                return std::move(result);
            }
            
            Dict render_settings = document_.GetRoot().AsDict().at("render_settings").AsDict();

            result.width = render_settings.at("width").AsDouble();
            result.height = render_settings.at("height").AsDouble();
            result.padding = render_settings.at("padding").AsDouble();
            result.line_width = render_settings.at("line_width").AsDouble();
            result.stop_radius = render_settings.at("stop_radius").AsDouble();
            result.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
            result.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
            result.underlayer_width = render_settings.at("underlayer_width").AsDouble();

            Array b_label_offset = render_settings.at("bus_label_offset").AsArray();
            result.bus_label_offset = {b_label_offset[0].AsDouble(), b_label_offset[1].AsDouble()};

            Array s_label_offset = render_settings.at("stop_label_offset").AsArray();
            result.stop_label_offset = {s_label_offset[0].AsDouble(), s_label_offset[1].AsDouble()};

            Array c_palette = render_settings.at("color_palette").AsArray();

            for(const auto& item : c_palette)
            {
                result.color_palette.push_back(GetColorFromNode(item));
            }

            Node uc_node = render_settings.at("underlayer_color");

            result.underlayer_color = GetColorFromNode(uc_node);

            return std::move(result);
        }

        Document JsonReader::MakeDocument(std::istream& input) 
        {
            return {LoadNode(input)};
        }
    }

    namespace output
    {
        void JsonWriter::Print(const Document& doc, std::ostream& output) 
        {
            PrintNode(doc.GetRoot(), output);
        }

        void JsonWriter::Print(Array value, std::ostream& output)
        {
            output << "[";

            bool is_first = true;

            for(const auto& item : value)
            {
                if(is_first)
                {
                    PrintNode(item, output);

                    is_first = false;

                    continue;
                }

                output << ", ";

                PrintNode(item, output);
            }

            output << "]";
        }
    
        void JsonWriter::Print(Dict value, std::ostream& output)
        {
            output << "{";

            bool is_first = true;

            for (auto [key, val] : value)
            {
                if(is_first)
                {
                    output << "\"" << key << "\"" << ":";

                    PrintNode(val, output);

                    is_first = false;

                    continue;
                }

                output << ", ";

                output << "\"" << key << "\"" << ":";

                PrintNode(val, output);
            }

            output << "}";
        }

        void JsonWriter::Print(std::nullptr_t, std::ostream& output)
        {
            output << "null";
        }

        void JsonWriter::Print(bool value, std::ostream& output)
        {
            output << (value ? "true" : "false");
        }

        void JsonWriter::Print(std::string value, std::ostream& output)
        {
            std::string res;

            for(const char c : value)
            {
                switch (c)
                {
                case '\n':
                    res += "\\n";
                    break;

                case '\r':
                    res += "\\r";
                    break;

                case '\t':
                    res += "\t";
                    break;

                case '\"':
                    res += "\\\"";
                    break;

                case '\\':
                    res += "\\\\";
                    break;
                
                default:
                    res += c;
                    break;
                }
            }

            output << "\"" << res << "\"";
        }

        void JsonWriter::PrintNode(const Node& node, std::ostream& output)
        {
            std::visit([&](const auto& value)
            {
                Print(value, output);
            }, node.GetValue());
        }
    }

    namespace detail
    {
        std::string ToLower(std::string&& str)
        {
            std::transform(str.begin(), str.end(), str.begin(),[](unsigned char c) { return std::tolower(c); });
            return str;
        }

        std::string ltrim(const std::string &s)
        {
            size_t start = s.find_first_not_of(WHITESPACE);
            return (start == std::string::npos) ? "" : s.substr(start);
        }

        std::string rtrim(const std::string &s)
        {
            size_t end = s.find_last_not_of(WHITESPACE);
            return (end == std::string::npos) ? "" : s.substr(0, end + 1);
        }

        std::string trim(const std::string &s) 
        {
            return rtrim(ltrim(s));
        }

        svg::Color GetColorFromNode(Node node)
        {
            svg::Color tmp;

            if(node.IsString())
            {
                tmp = node.AsString();
            }
            else
            {
                Array color = node.AsArray();

                if(color.size() == 3)
                {
                    tmp = svg::Rgb(color[0].AsInt(), color[1].AsInt(), color[2].AsInt());
                }
                else
                {
                    tmp  = svg::Rgba(color[0].AsInt(), color[1].AsInt(), color[2].AsInt(), color[3].AsDouble());
                }
            }
            return tmp;
        }
    }
}
