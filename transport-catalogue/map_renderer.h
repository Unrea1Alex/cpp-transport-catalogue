#pragma once
#include <cmath>
#include <optional>
#include <map>
#include "svg.h"
#include "geo.h"
#include "domain.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
using namespace domain;

namespace catalogue
{
    namespace render
    {
        struct RenderSetup
        {
            double width;
            double height;

            double padding;

            double line_width;
            double stop_radius;

            int bus_label_font_size;
            svg::Point bus_label_offset;

            int stop_label_font_size;
            svg::Point stop_label_offset;

            svg::Color underlayer_color;
            double underlayer_width;

            std::vector<svg::Color> color_palette;

            svg::Color GetNextColor(int& id) const;
        };

        class Renderer
        {
        public:

            Renderer(RenderSetup&& setup, SphereProjector&& sphere_projector) : setup_(setup), sphere_projector_(sphere_projector) {}

            void AddRoute(const std::string_view name, const std::vector<const Stop*> stops, bool is_roundtrip);

            void Render(std::ostream& stream) const;

        private:

            svg::Text&& GetRouteUnderlayerText(std::string route_name, const Stop* stop) const;
            svg::Text&& GetRouteText(std::string route_name, svg::Color color, const Stop* stop) const;

            svg::Text&& GetStopUnderlayerText(const Stop* stop) const;
            svg::Text&& GetStopText(svg::Color color, const Stop* stop) const;

            RenderSetup setup_;

            SphereProjector sphere_projector_;

            std::map<std::string, std::pair<std::vector<const Stop*>, bool>> routes_;
        };

    }
}