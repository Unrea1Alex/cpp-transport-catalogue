#pragma once

#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

using namespace catalogue::input;
using namespace catalogue::render;

namespace catalogue
{
    namespace requests
    {
        class RequestHandler
        {
        public:

            RequestHandler(TransportCatalogue& catalogue) : catalogue_(catalogue) {}
            void FillCatalogueFromJson(JsonReader reader);
            void PrintResponse(JsonReader reader, std::ostream& stream) const;
            void RenderMap(JsonReader reader, std::ostream& stream) const;

        private:

            Node GetBusStatJson(std::string route_name, int request_id) const;
            Node GetBusesByStopJson(std::string stop_name, int request_id) const;
            Node GetMapJson(JsonReader reader, int request_id) const;

            TransportCatalogue& catalogue_;
        };
    }
}