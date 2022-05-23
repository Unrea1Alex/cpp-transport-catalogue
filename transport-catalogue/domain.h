#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "geo.h"
#include "svg.h"

namespace domain
{
    struct Stop
    {
        std::string name_;
        geo::Coordinates coordinates_;

        Stop(std::string name, geo::Coordinates coordinates) : name_(name), coordinates_(coordinates){}

        bool operator==(const Stop& other) const
        {
            return other.name_ == name_;
        }
    };

    struct Route
    {
        Route(std::vector<const Stop*> stops, bool is_circular) : stops_(std::move(stops)), is_circular_(is_circular) {}
        std::vector<const Stop*> stops_;
        bool is_circular_;
    };

    template<typename T>
    struct StopPairHash
    {
        size_t operator()(std::pair<const T*, const T*> pair) const
        {
            return (std::hash<const void*>{}(pair.first) * 37) + (std::hash<const void*>{}(pair.second) * 37 * 37);
        }
    };

    inline const double EPSILON = 1e-6;
    inline bool IsZero(double value) 
    {
        return std::abs(value) < EPSILON;
    }

    class SphereProjector 
    {
    public:

        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding);

        svg::Point operator()(geo::Coordinates coords) const;

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    template <typename PointInputIt>
    SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding) : padding_(padding)
    {
        if (points_begin == points_end) 
        {
            return;
        }

        const auto [left_it, right_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) 
        { 
            return lhs.lng < rhs.lng; 
        });

        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) 
        { 
            return lhs.lat < rhs.lat; 
        });

        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) 
        {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) 
        {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) 
        {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) 
        {
            zoom_coeff_ = *width_zoom;
        } 
        else if (height_zoom) 
        {
            zoom_coeff_ = *height_zoom;
        }
    };
}