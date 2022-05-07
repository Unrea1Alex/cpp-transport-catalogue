#include <algorithm>
#include <sstream>
#include "input_reader.h"
#include "stat_reader.h"
#include "geo.h"

using namespace catalogue::stat::detail;
using namespace catalogue::input::detail;

namespace catalogue
{
    namespace stat
    {
        void StatReader::ReadQueries(std::istream& stream)
        {
            int count = 0;

            stream >> count;

            std::string str;

            std::getline(stream, str);

            for(int i = 0; i < count; i++)
            {
                std::getline(stream, str);

                queries.push_back(str);
            }
        }

        std::string StatReader::GetRouteInfo(const std::string& route_name) const
        {
            auto route_stops = catalogue_.GetRouteStops(route_name);

            std::stringstream ss("");

            ss << "Bus " << route_name << ": ";

            if(route_stops.empty())
            {
                ss << "not found";
                return  ss.str();
            }

            long double geo_distance = 0;
            int real_distance = 0;

            double curvature = 0.;

            for(auto i = 0; i < route_stops.size() - 1; i++)
            {
                real_distance += catalogue_.GetDistance(route_stops[i], route_stops[i + 1]);

                geo_distance += ComputeDistance(route_stops[i]->coordinates_, route_stops[i + 1]->coordinates_);

                curvature = (double)real_distance / geo_distance;
            }

            ss << route_stops.size() << " stops on route, ";

            std::sort(route_stops.begin(), route_stops.end(), [](const auto l, const auto r)
            {
                return l->name_ < r->name_;
            });

            auto end = std::unique(route_stops.begin(), route_stops.end());

            ss << std::distance(route_stops.begin(), end) << " unique stops, " << real_distance << " route length, " << curvature << " curvature";

            return ss.str();
        }

        void StatReader::PrintRouteInfo(std::string route_name, std::ostream& stream) const
        {
            stream << GetRouteInfo(route_name) << std::endl;
        }

        std::string StatReader::GetStopInfo(std::string stop_name) const
        {
            std::stringstream ss("");

            ss << "Stop " << stop_name << ": ";

            auto routes_tmp = catalogue_.GetStopRoutes(stop_name);

            if(!routes_tmp.has_value())
            {
                ss << "not found";
                return ss.str();
            }

            if((*routes_tmp).empty())
            {
                ss << "no buses";
                return ss.str();
            }

            std::vector<std::string> routes((*routes_tmp).size());

            std::transform((*routes_tmp).begin(), (*routes_tmp).end(), routes.begin(), [](auto route_ptr)
            {
                return *route_ptr;
            });

            std::sort(routes.begin(), routes.end());

            ss << "buses " << routes;

            return ss.str();
        }

        void StatReader::PrintStopInfo(std::string stop_name, std::ostream& stream) const
        {
            stream << GetStopInfo(stop_name) << std::endl;
        }

        void StatReader::PrintInfo(std::ostream& stream) const
        {
            for(const auto str : queries)
            {
                if(ToLower(str.substr(0, str.find(' '))) == "bus")
                {
                    PrintRouteInfo(trim(str.substr(4)), stream);
                }
                else
                {
                    PrintStopInfo(trim(str.substr(5)), stream);
                }
            }
        }
    }
}