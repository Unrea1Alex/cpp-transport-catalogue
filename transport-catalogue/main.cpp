#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
//#include "tests.h"

int main()
{
    //TestRouteStops();
    //TestStopsRoutes();

    catalogue::TransportCatalogue catalogue;

    catalogue::input::InputReader reader;

    reader.ReadQueries(std::cin);

    reader.FillCatalogueData(catalogue);

    catalogue::stat::StatReader stat_reader(catalogue);

    stat_reader.ReadQueries(std::cin);

    stat_reader.PrintInfo(std::cout);
}