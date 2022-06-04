#include <algorithm>
#include "transport_catalogue.h"

namespace catalogue
{
    Stop* TransportCatalogue::AddStop(const std::string& name, const geo::Coordinates& coordinates)
    {
        Stop* stop = &stops.emplace_back(std::move(name), std::move(coordinates));

        stops_index_[std::string_view(stop->name_)] = stop;

        return stop;
    }

    void TransportCatalogue::AddStopsDistances(const std::string& name, const std::vector<std::pair<std::string, int>>& distances)
    {
        auto stop_ptr = FindStopPtr(name);

        if(stop_ptr)
        {
            for(const auto& [name, distance] : distances)
            {
                auto next_stop_ptr = FindStopPtr(name);

                if(next_stop_ptr)
                {
                    stop_distances_[{stop_ptr, next_stop_ptr}] = distance;
                }
            }
        }
    }

    std::string* TransportCatalogue::AddBus(const std::string&& name)
    {
        return &buses_.emplace_back(std::move(name));
    }

    void TransportCatalogue::AddRoute(std::string&& route_name, std::vector<std::string>&& stop_names, bool is_circular)
    {
        auto bus_ptr = AddBus(std::move(route_name));

        auto route = MakeRoute(std::move(stop_names), is_circular);

        routes_index[std::string_view(*bus_ptr)] = route;

        for(auto stop_ptr : route->stops_)
        {
            UpdateStopIndex(stop_ptr, bus_ptr);
        }
    }

    Route* TransportCatalogue::MakeRoute(const std::vector<std::string>&& stop_names, bool is_circular)
    {
        std::vector<const Stop*> stop_ptrs;

        for(const auto& stop : stop_names)
        {
            auto stop_ptr = FindStopPtr(stop);

            if(stop_ptr)
            {
                stop_ptrs.push_back(stop_ptr);
            }
        }

        if(!is_circular)
        {
            for(int i = stop_ptrs.size() - 2; i > -1; i--)
            {
                stop_ptrs.push_back(stop_ptrs[i]);
            }
        }

        auto route = &routes_.emplace_back(stop_ptrs, is_circular);

        return route;
    }

    void TransportCatalogue::UpdateStopIndex(const Stop* stop_ptr, std::string* route_name_ptr)
    {
        if(stop_ptr == nullptr || route_name_ptr == nullptr)
        {
            return;
        }
        
        stop_to_routes_index_[stop_ptr].insert(route_name_ptr);
    }

    const std::vector<const Stop*> TransportCatalogue::GetRouteStops(const std::string& route_name) const
    {
        auto it = routes_index.find(route_name);

        if(it != routes_index.end())
        {
            return it->second->stops_;
        }
        return std::vector<const Stop*>();
    }

    const Stop* TransportCatalogue::FindStopPtr(const std::string& stop_name) const
    {
        auto it = stops_index_.find(stop_name);

        if(it != stops_index_.end())
        {
            return it->second;
        }
        return nullptr;
    }

    const std::optional<std::unordered_set<std::string*>> TransportCatalogue::GetStopRoutes(const std::string& stop_name) const
    {
        auto stop_ptr = FindStopPtr(stop_name);

        if(!stop_ptr)
        {
            return std::nullopt;
        }

        auto found_it = stop_to_routes_index_.find(stop_ptr);

        if(found_it != stop_to_routes_index_.end())
        {
            return std::optional<std::unordered_set<std::string*>>{found_it->second};
        }
        return std::optional<std::unordered_set<std::string*>>{std::unordered_set<std::string*>()};
    }

    int TransportCatalogue::GetDistance(const Stop* stop1, const Stop* stop2) const
    {
        auto it = stop_distances_.find({stop1, stop2});

        if(it != stop_distances_.end())
        {
            return it->second;
        }

        auto it1 = stop_distances_.find({stop2, stop1});

        if(it1 != stop_distances_.end())
        {
            return it1->second;
        }
        return 0;
    }

    std::vector<std::string_view> TransportCatalogue::GetBuses() const
    {
        std::vector<std::string_view> result(buses_.size());

        std::transform(buses_.begin(), buses_.end(), result.begin(), [](const auto& str)
        {
            return std::string_view(str);
        });
        return result;
    }

    std::vector<const Stop*>&& TransportCatalogue::GetStopsIndex() const
    {
        static std::vector<const Stop*> stops;

        for(const auto [name, stop] : stops_index_)
        {
            if(stop_to_routes_index_.count(stop))
            {
                stops.push_back(stop);
            }
        }
        return std::move(stops);
    }

    const std::unordered_map<std::string_view, Route*> TransportCatalogue::GetRouteIndex() const
    {
        return routes_index;
    }
}