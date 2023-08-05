#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <cctype>
#include <locale>
#include <array>
#include <iostream>

#include <boost/process.hpp>
#if defined(_MSC_VER)
#include <boost/process/windows.hpp>
#endif

#include "strops.h"

std::string ExecuteCommand(const char* cmd);
boost::process::child ExecuteAsync(std::string cmd, bool printOutput);

#endif