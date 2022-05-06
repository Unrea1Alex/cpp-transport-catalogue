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

    reader.ReadQueries();

    reader.FillCatalogueData(catalogue);

    catalogue::stat::StatReader stat_reader(catalogue);

    stat_reader.ReadQueries();

    stat_reader.PrintInfo();
}