#include <set>
#include "map_renderer.h"

namespace catalogue
{
    namespace render
    {
        svg::Color RenderSetup::GetNextColor(int& id) const
        {
            if(id >= 0 && id < (int)color_palette.size())
            {
                int old_id = id;

                if(id + 1 >= (int)color_palette.size())
                {
                    id = 0;
                }
                else
                {
                    id++;
                }
                return color_palette[old_id];
            }
            throw std::invalid_argument("invalid color palette id");
        }

        void Renderer::AddRoute(const std::string_view name, const std::vector<const Stop*> stops, bool is_roundtrip)
        {
            routes_[std::string(name)] = std::pair<std::vector<const Stop*>, bool>(stops, is_roundtrip);
        }

        svg::Text&& Renderer::GetRouteUnderlayerText(std::string route_name, const Stop* stop) const
        {
            static svg::Text bus_text_underlayer;
            bus_text_underlayer.SetData(route_name);
            bus_text_underlayer.SetFontSize(setup_.bus_label_font_size);
            bus_text_underlayer.SetFontFamily("Verdana");
            bus_text_underlayer.SetPosition(sphere_projector_(stop->coordinates_));
            bus_text_underlayer.SetOffset(setup_.bus_label_offset);
            bus_text_underlayer.SetFontWeight("bold");
            bus_text_underlayer.SetFillColor(setup_.underlayer_color);
            bus_text_underlayer.SetStrokeColor(setup_.underlayer_color);
            bus_text_underlayer.SetStrokeWidth(setup_.underlayer_width);
            bus_text_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            bus_text_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            return std::move(bus_text_underlayer);
        }

        svg::Text&& Renderer::GetRouteText(std::string route_name, svg::Color color, const Stop* stop) const
        {        
            static svg::Text bus_text;
            bus_text.SetData(route_name);
            bus_text.SetFontSize(setup_.bus_label_font_size);
            bus_text.SetFontFamily("Verdana");
            bus_text.SetPosition(sphere_projector_(stop->coordinates_));
            bus_text.SetOffset(setup_.bus_label_offset);
            bus_text.SetFontWeight("bold");
            bus_text.SetFillColor(color);

            return std::move(bus_text);
        }

        svg::Text&& Renderer::GetStopUnderlayerText(const Stop* stop) const
        {
            static svg::Text stop_text_underlayer;
            stop_text_underlayer.SetData(stop->name_);
            stop_text_underlayer.SetFontSize(setup_.stop_label_font_size);
            stop_text_underlayer.SetFontFamily("Verdana");
            stop_text_underlayer.SetPosition(sphere_projector_(stop->coordinates_));
            stop_text_underlayer.SetOffset(setup_.stop_label_offset);
            stop_text_underlayer.SetFillColor(setup_.underlayer_color);
            stop_text_underlayer.SetStrokeColor(setup_.underlayer_color);
            stop_text_underlayer.SetStrokeWidth(setup_.underlayer_width);
            stop_text_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            stop_text_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            return std::move(stop_text_underlayer);
        }

        svg::Text&& Renderer::GetStopText(svg::Color color, const Stop* stop) const
        {
            static svg::Text stop_text;
            stop_text.SetData(stop->name_);
            stop_text.SetFontSize(setup_.stop_label_font_size);
            stop_text.SetFontFamily("Verdana");
            stop_text.SetPosition(sphere_projector_(stop->coordinates_));
            stop_text.SetOffset(setup_.stop_label_offset);
            stop_text.SetFillColor(color);

            return std::move(stop_text);
        }

        void Renderer::Render(std::ostream& stream) const
        {
            svg::Document doc;
            std::vector<svg::Circle> circles;
            std::vector<svg::Text> bus_texts;
            std::vector<svg::Text> stop_texts;

            int current_color_id = 0;

            std::map<std::string_view, const Stop*> stops_list;

            for(const auto& [name, stops] : routes_)
            {
                svg::Polyline line;

                for(const auto stop : stops.first)
                {
                    svg::Point coords = sphere_projector_(stop->coordinates_);

                    line.AddPoint(coords);

                    stops_list[std::string_view(stop->name_)] = stop;
                }

                if(!stops.first.empty())
                {
                    svg::Color color = setup_.GetNextColor(current_color_id);

                    bus_texts.push_back(GetRouteUnderlayerText(name, stops.first[0]));
                    bus_texts.push_back(GetRouteText(name, color, stops.first[0]));
                
                    if(!stops.second)
                    {
                        int stop_count = stops.first.size() - 1;

                        int id = stop_count / 2 + (stop_count - (stop_count / 2) * 2);

                        if(stops.first[0] != stops.first[id])
                        {
                            bus_texts.push_back(GetRouteUnderlayerText(name, stops.first[id]));
                            bus_texts.push_back(GetRouteText(name, color, stops.first[id]));
                        }
                    }

                    line.SetStrokeColor(color);
                    line.SetFillColor(svg::Color("none"));
                    line.SetStrokeWidth(setup_.line_width);
                    line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                    line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                    doc.Add(line);
                }                 
            }

            for(const auto [name, stop] : stops_list)
            {
                svg::Circle circle;
                circle.SetCenter(sphere_projector_(stop->coordinates_));
                circle.SetRadius(setup_.stop_radius);
                circle.SetFillColor("white");

                circles.push_back(circle);

                stop_texts.push_back(GetStopUnderlayerText(stop));
                stop_texts.push_back(GetStopText(svg::Color("black"), stop));
            }

            for(auto& t : bus_texts)
            {
                doc.Add(t);
            }

            for(auto& c : circles)
            {
                doc.Add(c);
            }

            for(auto& t : stop_texts)
            {
                doc.Add(t);
            }

            doc.Render(stream);
        }
    }
}