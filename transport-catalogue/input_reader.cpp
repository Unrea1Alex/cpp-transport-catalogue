#include <sstream>
#include <iterator>
#include <iostream>
#include "input_reader.h"

using namespace catalogue::input;
using namespace catalogue::input::detail;

namespace catalogue
{
    namespace input
    {
        void InputReader::ReadQueries(std::istream& stream)
        {
            int count = 0;

            stream >> count;

            std::string str;
            std::getline(stream, str);

            for(int i = 0; i < count; i++)
            {
                stream.precision(6);
                std::getline(stream, str);

                if(ToLower(str.substr(0, str.find(' '))) == "bus")
                {
                    bus_strings.push_back(str);
                }
                else
                {
                    ParseStops(str);
                }
            }
        }

        void InputReader::ParseStops(const std::string& stop_str)
        {
            StopInfo stop_info;

            auto name_start_pos = stop_str.find(' ') + 1;
            auto info_start_pos = stop_str.find(':');

            stop_info.name = stop_str.substr(name_start_pos, info_start_pos - name_start_pos);

            std::stringstream ss;

            ss << stop_str.substr(info_start_pos + 2);

            std::string info_str;

            int counter = 0;

            while(getline(ss, info_str, ','))
            {
                if(counter == 0)
                {
                    stop_info.coord.lat = std::atof(trim(info_str).c_str());
                    counter++;
                    continue;
                }

                if(counter == 1)
                {
                    stop_info.coord.lng = std::atof(trim(info_str).c_str());
                    counter++;
                    continue;
                }

                auto distance_end_pos = info_str.find('m');
                auto stop_name_start_pos = info_str.find_first_not_of(' ', distance_end_pos + 1) + 2;

                stop_info.distances.emplace_back(trim(info_str.substr(stop_name_start_pos)).c_str(), std::atof(trim(info_str.substr(0, distance_end_pos)).c_str()));
            }

            stops_info_.emplace_back(std::move(stop_info));
        }

        void InputReader::FillCatalogueData(TransportCatalogue& catalogue)
        {
            //fill stops
            for(auto stop : stops_info_)
            {
                catalogue.AddStop(stop.name, stop.coord);
            }

            //fill stops distances
            for(auto stop : stops_info_)
            {
                catalogue.AddStopsDistances(stop.name, stop.distances);
            }

            //fill routes
            for(const auto& str : bus_strings)
            {
                auto delimiter_pos = str.find(':');
                auto name = str.substr(4, delimiter_pos - 4);
                
                const auto& stops_str = str.substr(delimiter_pos + 2);

                bool is_circular = stops_str.find("-") == std::string::npos;

                auto route_stops = ParseRouteStops(std::move(stops_str));

                catalogue.AddRoute(std::move(name), std::move(route_stops), is_circular);
            }
        }

        std::vector<std::string> InputReader::ParseRouteStops(const std::string&& route_stops_str)
        {
            std::stringstream ss(route_stops_str);

            const char delimiter = route_stops_str.find("-") != std::string::npos ? '-' : '>';

            std::vector<std::string> result;

            std::string tmp;

            while(std::getline(ss, tmp, delimiter))
            {
                result.push_back(trim(tmp));
            }

            return result;
        }

        namespace detail
        {
            std::string ToLower(std::string&& str)
            {
                std::transform(str.begin(), str.end(), str.begin(),[](unsigned char c) { return std::tolower(c); });

                return str;
            }

            std::string ltrim(const std::string &s)
            {
                size_t start = s.find_first_not_of(WHITESPACE);
                return (start == std::string::npos) ? "" : s.substr(start);
            }

            std::string rtrim(const std::string &s)
            {
                size_t end = s.find_last_not_of(WHITESPACE);
                return (end == std::string::npos) ? "" : s.substr(0, end + 1);
            }

            std::string trim(const std::string &s) {
                return rtrim(ltrim(s));
            }
        }
    }
}
