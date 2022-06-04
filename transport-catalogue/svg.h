#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <tuple>
#include <sstream>

namespace svg 
{
    enum class StrokeLineCap 
    {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin 
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    struct Rgb
    {
        Rgb() : red(0), green(0), blue(0) {}
        Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {};

        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    struct Rgba
    {
        Rgba() : red(0), green(0), blue(0), opacity(1.) {}
        Rgba(uint8_t r, uint8_t g, uint8_t b, double o) : red(r), green(g), blue(b), opacity(o) {};

        uint8_t red;
        uint8_t green;
        uint8_t blue;
        double opacity = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

    inline const Color NoneColor{"none"};

    struct Point 
    {
        Point() = default;
        Point(double x, double y) : x(x), y(y) { }

        double x = 0;
        double y = 0;
    };

    struct RenderContext 
    {
        RenderContext(std::ostream& out) : out(out) { }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out), indent_step(indent_step), indent(indent) { }

        inline RenderContext Indented() const { return {out, indent_step, indent + indent_step}; }

        void RenderIndent() const;

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    void GetColor(std::ostream&, std::monostate);
    void GetColor(std::string);
    void GetColor(Rgb);
    void GetColor(Rgba);

    std::ostream& operator<<(std::ostream& stream, StrokeLineCap line_cap);
    std::ostream& operator<<(std::ostream& stream, StrokeLineJoin line_join);
    std::ostream& operator<<(std::ostream& stream, Color color);


    template <typename T>
    class PathProps
    {
    public:
        T& SetFillColor(Color color)
        {
            fill_color_ = color;

            return AsOwner();
        }

        T& SetStrokeColor(Color color)
        {
            stroke_color_ = color;

            return AsOwner();
        }

        T& SetStrokeWidth(double width)
        {
            stroke_width_ = width;

            return AsOwner();
        }

        T& SetStrokeLineCap(StrokeLineCap line_cap)
        {
            stroke_line_cap_ = line_cap;

            return AsOwner();
        }

        T& SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            stroke_line_join_ = line_join;

            return AsOwner();
        }
        
    protected:
        void RenderAttrs(const RenderContext& context) const
        {
            std::ostream& stream = context.out;
            std::stringstream ss;

            if(!std::holds_alternative<std::monostate>(fill_color_))
            {
                stream << "fill=\"";
                stream << fill_color_;
                stream << "\" ";
            }

            if(!std::holds_alternative<std::monostate>(stroke_color_))
            {
                stream << "stroke=\"";
                stream << stroke_color_;
                stream << "\" ";
            }

            if(stroke_width_ > 0)
            {
                stream << "stroke-width=\"" << stroke_width_ << "\" ";
            }

            if(stroke_line_cap_.has_value())
            {
                stream << "stroke-linejoin=\"";
                stream << stroke_line_cap_.value();
                stream << "\" ";
            }

            if(stroke_line_join_.has_value())
            {
                stream << "stroke-linecap=\"";
                stream << stroke_line_join_.value();
                stream << "\" ";
            }
        }

        ~PathProps() = default;

    private:
        Color fill_color_;
        Color stroke_color_;
        double stroke_width_ = 0;

        std::optional<StrokeLineCap> stroke_line_cap_ = std::optional<StrokeLineCap>();
        std::optional<StrokeLineJoin> stroke_line_join_ = std::optional<StrokeLineJoin>();

        T& AsOwner()
        {
            return static_cast<T&>(*this);
        }
    };

    class Object 
    {
    public:
        void Render(const RenderContext& context) const;
        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class Circle final : public Object, public PathProps<Circle>
    {
    public:
        Circle() : center_({0, 0}), radius_(1.){}
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_;
    };

    class Polyline : public Object, public PathProps<Polyline>
    {
    public:
        Polyline() = default;
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_;
    };

    class Text : public Object, public PathProps<Text>
    {
    public:
        Text() : pos_({0, 0}), offset_({0, 0}), size_(1), font_family_(""), font_weight_(""), data_("") {}
        Text& SetPosition(Point pos);
        Text& SetOffset(Point offset);
        Text& SetFontSize(uint32_t size);
        Text& SetFontFamily(std::string font_family);
        Text& SetFontWeight(std::string font_weight);
        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const override;
        std::string ClearData() const;

        Point pos_;
        Point offset_;
        uint32_t size_;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;

        inline static const std::map<const char, const std::string> symbols
        {
            {'\"', "&quot;"},
            {'\'', "&apos;"},
            {'<', "&lt;"},
            {'>', "&gt;"},
            {'&', "&amp;"}
        };
        
    };

    class ObjectContainer
    {
    public:
        template<class Obj>
        void Add(Obj object);
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
        virtual ~ObjectContainer() = default;
    };

    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    class Document : public ObjectContainer
    {
    public:
        void AddPtr(std::unique_ptr<Object>&& obj) override;
        void Render(std::ostream& out) const;

    private:

        std::vector<std::unique_ptr<Object>> objects_;
    };

    template<class Obj>
    void ObjectContainer::Add(Obj object)
    {
        AddPtr(std::make_unique<Obj>(object));
    }

    std::ostream& operator<<(std::ostream& stream, std::vector<Point> points);
}