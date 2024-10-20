#ifndef miner_h
#define miner_h

#include <string>
#include <vector>
#include <boost/process.hpp>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>
#if WINDOWS
#include <conio.h>
#define GETKEY() getch()
#elif UNIX
#include <curses.h>
#include <stdio.h>
#define GETKEY() getch()
#endif


#include "json.hpp"
#include "Console.h"
#include "System.h"
#include "strops.h"
#include "SettingsConsts.h"
#include "crypto.h"
#include "Network.h"
#include "FileManip.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

int Mine(json, int, json&);
int PoolMine(std::string, json&);
int MineAnyBlock(int, std::string&);

#endif
