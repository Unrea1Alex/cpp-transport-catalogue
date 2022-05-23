#include <iostream>
#include <sstream>
#include <fstream>
#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <filesystem>


using namespace catalogue;
using namespace catalogue::input;
using namespace catalogue::output;
using namespace catalogue::requests;

using std::filesystem::current_path;

int main() 
{
    TransportCatalogue catalogue;    

    //std::fstream fs(current_path().string() + "/map_test.json");
    //JsonReader jr(fs);

    //fs.close();

    JsonReader jr(std::cin);

    RequestHandler rh(catalogue);

    rh.FillCatalogueFromJson(jr);

    //std::fstream fs_out(current_path().string() + "/map_test_out.json");

    //rh.RenderMap(jr, fs_out);

    //fs_out.close();

    rh.PrintResponse(jr, std::cout);

    //std::cout << "1";
}