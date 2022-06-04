#include "domain.h"

namespace domain
{
    svg::Point SphereProjector::operator()(geo::Coordinates coords) const
    {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

    bool Stop::operator==(const Stop& other) const
    {
        return other.name_ == name_;
    }

    Route::Route(std::vector<const Stop*> stops, bool is_circular) : stops_(std::move(stops)), is_circular_(is_circular) {}
    
}
