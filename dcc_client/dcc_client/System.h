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

std::string ExecuteCommand(const char* cmd, bool printout=true);
boost::process::child ExecuteAsync(std::string cmd, bool printOutput);

template <
	class result_t = std::chrono::milliseconds,
	class clock_t = std::chrono::steady_clock,
	class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
	return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

#endif