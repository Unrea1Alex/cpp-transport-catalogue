#pragma once

#include <string>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <string_view>
#include <functional>
#include "domain.h"

using namespace domain;

namespace catalogue
{
    class TransportCatalogue
    {
    public:
        
        Stop* AddStop(const std::string& name, const geo::Coordinates& coordinates);

        void AddStopsDistances(const std::string& name, const std::vector<std::pair<std::string, int>>& distances);

        void AddRoute(std::string&& route_name, std::vector<std::string>&& stop_names, bool is_circular);

        const Stop* FindStopPtr(const std::string& stop_name) const;

        const std::vector<const Stop*> GetRouteStops(const std::string& route_name) const;

        const std::optional<std::unordered_set<std::string*>> GetStopRoutes(const std::string& stop_name) const;

        int GetDistance(const Stop* stop1, const Stop* stop2) const;

        std::vector<const Stop*>&& GetStopsIndex() const;
        
        std::vector<std::string_view> GetBuses() const;

        template<typename T>
        std::vector<std::string_view> GetBuses(T predicate) const;

        const std::unordered_map<std::string_view, Route*> GetRouteIndex() const;

    private:

        std::string* AddBus(const std::string&& name);
        void UpdateStopIndex(const Stop* stop_ptr, std::string* route_name_ptr);
        Route* MakeRoute(const std::vector<std::string>&& stop_names, bool is_circular);

        std::deque<Stop> stops;
        std::deque<std::string> buses_;
        std::deque<Route> routes_;

        std::unordered_map<std::string_view, Stop*> stops_index_;

        std::unordered_map<std::string_view, Route*> routes_index;
        std::unordered_map<const Stop*, std::unordered_set<std::string*>> stop_to_routes_index_; 

        std::unordered_map<std::pair<const Stop*, const Stop*>, double, StopPairHash<Stop>> stop_distances_;
    };

    template<typename T>
    std::vector<std::string_view> TransportCatalogue::GetBuses(T predicate) const
    {
        std::vector<std::string_view> result = std::move(GetBuses());

        std::sort(result.begin(), result.end(), predicate);

        return result;
    }
}