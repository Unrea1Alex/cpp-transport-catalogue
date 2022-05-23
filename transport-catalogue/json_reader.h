#pragma once

#include <string>
#include <vector>
#include <istream>
#include "json.h"
#include "geo.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace json;
using namespace catalogue::render;

namespace catalogue
{    
    namespace input
    {
        class JsonReader
        {
        public: 
            JsonReader() = default;

            JsonReader(std::istream& input) : document_(std::move(MakeDocument(input))){}

            void Load(std::istream& input);

            Document& Get();

            RenderSetup&& GetRenderSetup() const;

        private:

            Document MakeDocument(std::istream& input);

            bool Check(std::istream& stream, std::string&& str);

            Node LoadNode(std::istream& input);

            Node LoadArray(std::istream& in);

            Node LoadNum(std::istream& in);

            Node LoadString(std::istream& in);

            Node LoadDict(std::istream& in);

            Document document_;
        };
    }

    namespace output
    {
        class JsonWriter
        {
        public:
            void Print(const Document& doc, std::ostream& output);

        private:
            template<typename T>
            void Print(const T& value, std::ostream& output)
            {
                output << value;
            }

            void Print(std::nullptr_t, std::ostream& output);

            void Print(bool value, std::ostream& output);

            void Print(std::string value, std::ostream& output);

            void Print(Array value, std::ostream& output);

            void Print(Dict value, std::ostream& output);

            void PrintNode(const Node& node, std::ostream& output);
        };

        
    }
    
    namespace detail
    {
        const std::string WHITESPACE = " \n\r\t\f\v";

        std::string ToLower(std::string&& str);

        std::string ltrim(const std::string &s);
        
        std::string rtrim(const std::string &s);
        
        std::string trim(const std::string &s);

        svg::Color GetColorFromNode(Node node);
    }

}