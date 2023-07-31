#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <cctype>
#include <locale>
#include <array>
#include <iostream>

#include <boost/process.hpp>
#include <boost/process/windows.hpp>

#include "strops.h"

std::string ExecuteCommand(const char* cmd);
boost::process::child ExecuteAsync(std::string cmd, bool printOutput);

#endif