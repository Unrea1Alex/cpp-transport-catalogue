Transport catalogue 
The project uses CMake build. When starting, you must specify an input file of the form input11.json and input12.json with data from the database of the transport directory and queries, respectively. The response to requests is output to the standard output stream. Bus, Stop, Route, and Map queries are recognized, which display bus stops, buses at the stop, the route from stop A to B with transfers and total travel time, and an overall route map, respectively. Output as a JSON file.

Building and Run

mkdir BuildSearchEngine && cd BuildSearchEngine
cmake ..
cmake --build .
Start ./search_engine or search_engine.exe

System requirements and Stack C++17 GCC version 8.1.0 Cmake 3.21.2 (minimal 3.10) JSON SVG
