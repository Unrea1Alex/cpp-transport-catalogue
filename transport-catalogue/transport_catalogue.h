#pragma once

#include <string>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <string_view>
#include <functional>
#include "geo.h"

namespace catalogue
{
    struct Stop
    {
        std::string name_;
        Coordinates coordinates_;

        Stop(std::string name, Coordinates coordinates) : name_(name), coordinates_(coordinates){}

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

    class TransportCatalogue
    {
    public:
        
        Stop* AddStop(const std::string& name, const Coordinates& coordinates);

        void AddStopsDistances(const std::string& name, const std::vector<std::pair<std::string, int>>& distances);

        void AddRoute(std::string&& route_name, std::vector<std::string>&& stop_names, bool is_circular);

        const Stop* FindStopPtr(const std::string& stop_name) const;

        const std::vector<const Stop*> GetRouteStops(const std::string& route_name) const;

        const std::optional<std::unordered_set<std::string*>> GetStopRoutes(const std::string& stop_name) const;

        int GetDistance(const Stop* stop1, const Stop* stop2) const;

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
}