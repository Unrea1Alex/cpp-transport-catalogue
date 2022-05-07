#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <deque>
#include "transport_catalogue.h"

namespace catalogue
{
    namespace input
    {
        const std::string WHITESPACE = " \n\r\t\f\v";

        class InputReader
        {
        public:

            void ReadQueries(std::istream& stream);

            void FillCatalogueData(TransportCatalogue& catalogue);

        private:

            struct StopInfo
            {
                std::string name;
                Coordinates coord;
                std::vector<std::pair<std::string, int>> distances;
            };

            void ParseStops(const std::string& stop_str);

            std::vector<std::string> ParseRouteStops(const std::string&& route_stops_str); 

            std::vector<StopInfo> stops_info_;

            std::vector<std::string> stops_strings;
            std::vector<std::string> bus_strings;
        };

        namespace detail
        {
            std::string ToLower(std::string&& str);

            std::string ltrim(const std::string &s);
            
            std::string rtrim(const std::string &s);
            
            std::string trim(const std::string &s);
        }
    }

}