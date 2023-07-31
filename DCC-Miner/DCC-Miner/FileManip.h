#ifndef filemanip_h
#define filemanip_h
//
//#include <algorithm> 
//#include <cctype>
//#include <locale>
//#include "extlibs/elzip/include/elzip/elzip.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

int ExtractZip(std::string path, std::string saveAs);

int FileCount(std::string dir);

#endif
