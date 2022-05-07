#pragma once 

#include <iostream>
#include <string>
#include "transport_catalogue.h"

namespace catalogue
{
    namespace stat
    {
        class StatReader
        {
        public:

            StatReader() = delete;

            StatReader(const TransportCatalogue& catalogue) : catalogue_(catalogue){};

            void ReadQueries(std::istream& stream);

            void PrintInfo(std::ostream& stream) const;

        private:

            std::string GetRouteInfo(const std::string& route_name) const;

            std::string GetStopInfo(std::string stop_name) const;

            void PrintRouteInfo(std::string route_name, std::ostream& stream) const;

            void PrintStopInfo(std::string route_name, std::ostream& stream) const;

            std::vector<std::string> queries;

            const TransportCatalogue& catalogue_;
        };

        namespace detail
        {
            template<typename T>
            std::ostream& operator<<(std::ostream& stream, std::vector<T> container)
            {
                bool is_first = true;

                for(auto r : container)
                {
                    if(is_first)
                    {
                        stream << r;
                        is_first = false;

                        continue;
                    }

                    stream << " " << r;
                }

                return stream;
            }
        }
    }
}



