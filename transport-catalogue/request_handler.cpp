#include <algorithm>
#include "request_handler.h"
#include "geo.h"

using namespace catalogue::input;
using namespace catalogue::output;
using namespace std::literals;
using namespace domain;

namespace catalogue
{
    namespace requests
    {
        void RequestHandler::FillCatalogueFromJson(JsonReader reader)
        {
            Document doc = reader.Get();

            Array base_requests = doc.GetRoot().AsMap().at("base_requests").AsArray();

            for(const auto& request : base_requests)
            {
                if(request.AsMap().at("type").AsString() == "Stop")
                {
                    geo::Coordinates coord{request.AsMap().at("latitude").AsDouble(), request.AsMap().at("longitude").AsDouble()};
                    catalogue_.AddStop(request.AsMap().at("name").AsString(), coord);
                }
            }

            for(const auto& request : base_requests)
            {
                if(request.AsMap().at("type").AsString() == "Bus")
                {
                    std::string name = request.AsMap().at("name").AsString();
                    Array stops = request.AsMap().at("stops").AsArray();

                    std::vector<std::string> stop_names(stops.size());

                    std::transform(stops.begin(), stops.end(), stop_names.begin(), [](const auto& node)
                    {
                        return node.AsString();
                    });

                    bool is_circular = request.AsMap().at("is_roundtrip").AsBool();

                    catalogue_.AddRoute(std::move(name), std::move(stop_names), is_circular);
                }
                else
                {
                    std::string name = request.AsMap().at("name").AsString();
                    Dict json_distances = request.AsMap().at("road_distances").AsMap();

                    std::vector<std::pair<std::string, int>> distances(json_distances.size());

                    std::transform(json_distances.begin(), json_distances.end(), distances.begin(), [](const auto& dist_pair)
                    {
                        return std::pair<std::string, int>(dist_pair.first, dist_pair.second.AsInt());
                    });

                    catalogue_.AddStopsDistances(name, distances);
                }
            }
        }

        Node RequestHandler::GetBusStatJson(std::string route_name, int request_id) const
        {
            std::vector<const Stop*> stops = catalogue_.GetRouteStops(route_name);

            if(stops.empty())
            {
                Dict res = Dict
                {
                    {"request_id"s, request_id},
                    {"error_message"s, "not found"s},
                };

                return Node(res);
            }

            long double geo_distance = 0;
            int real_distance = 0;

            double curvature = 0.;

            for(auto i = 0; i < stops.size() - 1; i++)
            {
                real_distance += catalogue_.GetDistance(stops[i], stops[i + 1]);

                geo_distance += geo::ComputeDistance(stops[i]->coordinates_, stops[i + 1]->coordinates_);

                curvature = (double)real_distance / geo_distance;
            }

            std::sort(stops.begin(), stops.end(), [](const auto l, const auto r)
            {
                return l->name_ < r->name_;
            });

            auto end = std::unique(stops.begin(), stops.end());

            int unique_stops = std::distance(stops.begin(), end);

            Dict res = Dict
            {
                {"request_id"s, request_id},
                {"curvature"s, curvature},
                {"route_length"s, real_distance},
                {"stop_count"s, (int)(stops.size())},
                {"unique_stop_count"s, unique_stops},
            };

            return Node(res);
        }

        Node RequestHandler::GetBusesByStopJson(std::string stop_name, int request_id) const
        {
            const std::optional<std::unordered_set<std::string*>> routes = catalogue_.GetStopRoutes(stop_name);

            if(!routes.has_value())
            {
                Dict res = Dict
                {
                    {"request_id"s, request_id},
                    {"error_message"s, "not found"s},
                };

                return Node(res);
            }

            Array buses;

            for(auto str : routes.value())
            {
                buses.push_back(*str);
            }

            std::sort(buses.begin(), buses.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.AsString() < rhs.AsString();
            });

            Dict res = Dict
            {
                {"request_id"s, request_id},
                {"buses"s, buses},
            };

            return Node(res);
        }

        Node RequestHandler::GetMapJson(JsonReader reader, int request_id) const
        {
            std::stringstream ss;
            
            RenderMap(reader, ss);
            
            Dict res = Dict
            {
                {"request_id"s, request_id},
                {"Map"s, ss.str()},
            };

            return Node(res);
        }

        void RequestHandler::PrintResponse(JsonReader reader, std::ostream& stream) const
        {
            Document doc = reader.Get();

            Array stat_requests = doc.GetRoot().AsMap().at("stat_requests").AsArray();

            Array response_array;

            for(const auto& request : stat_requests)
            {
                if(request.AsMap().at("type").AsString() == "Stop")
                {
                    response_array.push_back(GetBusesByStopJson(request.AsMap().at("name").AsString(), request.AsMap().at("id").AsInt()));
                    continue;
                }
                
                if(request.AsMap().at("type").AsString() == "Bus")
                {
                    response_array.push_back(GetBusStatJson(request.AsMap().at("name").AsString(), request.AsMap().at("id").AsInt()));
                }

                if(request.AsMap().at("type").AsString() == "Map")
                {
                    response_array.push_back(GetMapJson(reader, request.AsMap().at("id").AsInt()));
                }
            }

            json::Document result{response_array};

            JsonWriter writer;

            writer.Print(result, stream);
        }

        void RequestHandler::RenderMap(JsonReader reader, std::ostream& stream) const
        {
            std::vector<const Stop*> all_stops = catalogue_.GetStopsIndex();

            std::vector<geo::Coordinates> coords(all_stops.size());

            std::transform(all_stops.begin(), all_stops.end(), coords.begin(), [](const auto& stop)
            {
                return stop->coordinates_;
            });

            RenderSetup render_setup = reader.GetRenderSetup();

            SphereProjector sphere_projector(coords.begin(), coords.end(), render_setup.width, render_setup.height, render_setup.padding);

            Renderer renderer(std::move(render_setup), std::move(sphere_projector));

            std::vector<std::string_view> buses = catalogue_.GetBuses();

            for(const auto bus : buses)
            {
                const std::vector<const Stop*> stops = catalogue_.GetRouteStops(std::string(bus));

                bool is_roundtrip = catalogue_.GetRouteIndex().at(std::string(bus))->is_circular_;

                renderer.AddRoute(bus, stops, is_roundtrip);
            }

            renderer.Render(stream);
        }
    }
}