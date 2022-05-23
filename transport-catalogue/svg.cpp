#include "svg.h"
#include <algorithm>
#include <math.h>

namespace svg 
{

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const 
    {
        context.RenderIndent();

        RenderObject(context);

        context.out << std::endl;
    }

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const 
    {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;

        RenderAttrs(context);

        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point)
    {
        points_.push_back(point);

        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<polyline points="sv;

        out << points_;

        out << " "sv;

        RenderAttrs(context);
        
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        data_ = data;
        return *this;
    }

    std::string Text::ClearData() const
    {
        std::string result;

        for(const auto c : data_)
        {
            if(symbols.find(c) != symbols.end())
            {
                result += symbols.at(c);
                continue;
            }

            result += c;
        }

        return result;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text "sv;
        
        if(!font_family_.empty())
        {
            out << "font-family=\""sv << font_family_ << "\" "sv;
        }
        
        if(!font_weight_.empty())
        {
            out << "font-weight=\""sv << font_weight_ << "\" "sv;
        }

        out << "font-size=\""sv << size_ << "\" "sv;
        out << "x=\""sv << pos_.x << "\" "sv;
        out << "y=\""sv << pos_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" "sv;
        out << "dy=\""sv << offset_.y << "\" "sv;

        RenderAttrs(context);

        out << ">"sv;
        
        out << ClearData();
        
        out << "</text>"sv;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        RenderContext ctx(out, 2, 2);

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        out.precision(6);

        for(const auto& obj : objects_)
        {
            obj.get()->Render(ctx);
        }

        out << "</svg>"sv;
    }

    void GetColor(std::ostream& stream, std::monostate)
    {
        stream << "none"sv;
    }

    void GetColor(std::ostream& stream, std::string str)
    {
        stream << str;
    }

    void GetColor(std::ostream& stream, Rgb rgb)
    {
        stream << "rgb("sv << (int)rgb.red << ","sv << (int)rgb.green << ","sv << (int)rgb.blue << ")"sv;
    }

    void GetColor(std::ostream& stream, Rgba rgba)
    {
        stream << "rgba("sv << (int)rgba.red << ","sv << (int)rgba.green << ","sv << (int)rgba.blue << ","sv << rgba.opacity << ")"sv;
    }

    std::ostream& operator<<(std::ostream& stream, std::vector<Point> points)
    {
        if(points.empty())
        {
            stream << "\"\""sv;
            return stream; 
        }
        
        stream << "\""sv;

        bool is_first = true;

        for(auto point : points)
        {
            stream.precision(6);
            if(is_first)
            {
                stream << point.x << ","sv << point.y;
                is_first = false;
                continue;
            }

            stream << " "sv << point.x << ","sv << point.y;
        }

        stream << "\"";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, StrokeLineCap line_cap)
    {
        switch (line_cap)
            {
            case StrokeLineCap::BUTT :
                stream << "butt"sv;
                break;
            
            case StrokeLineCap::ROUND :
                stream << "round"sv;
                break;
            
            case StrokeLineCap::SQUARE :
                stream << "square"sv;
                break;
            
            default:
                stream << "butt"sv;
                break;
            }

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, StrokeLineJoin line_join)
    {
        switch (line_join)
            {
            case StrokeLineJoin::ARCS :
                stream << "arcs"sv;
                break;
            
            case StrokeLineJoin::BEVEL :
                stream << "bevel"sv;
                break;
            
            case StrokeLineJoin::MITER :
                stream << "miter"sv;
                break;
            
            case StrokeLineJoin::MITER_CLIP :
                stream << "miter-clip"sv;
                break;

            case StrokeLineJoin::ROUND :
                stream << "round"sv;
                break;
            
            default:
                stream << "miter"sv;
                break;
            }

            return stream;
    }

    std::ostream& operator<<(std::ostream& stream, Color color)
    {
        std::visit([&stream](auto value) 
        {
            svg::GetColor(stream, value);
        }, color);

        return stream;
    }

}