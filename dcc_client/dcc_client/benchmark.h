#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <algorithm>
#include <chrono>
#include <iostream>
#include<vector>
#include <string>
#include <thread>

#include "strops.h"

using namespace std;
using namespace std::chrono;

const auto processor_count = std::thread::hardware_concurrency();

unsigned long long benchmark();

#endif
