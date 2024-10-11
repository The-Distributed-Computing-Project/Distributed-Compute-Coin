#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "benchmark.h"
using namespace std;
using namespace std::chrono;


inline static const string roundFloat(const double input, const int decimal_places)
{
    ostringstream str;
    str << fixed << setprecision(decimal_places);
    str << input;
    return str.str();
}

std::string truncateNum(double x){
    if(x > 1000000000000)
        return roundFloat((float)((long long)x/10000000000)/100.0,2) + " T";
    else if(x > 1000000000)
        return roundFloat((float)((long)x/10000000)/100.0,2) + " B";
    else if(x > 1000000)
        return roundFloat((float)((long)x/10000)/100.0,2) + " M";
    else if(x > 1000)
        return roundFloat((float)((long)x/10)/100.0,2) + " K";
    else
        return std::to_string(x);
}

#define FLOPS_SAMPLES 100000
unsigned long long benchmark()
{
    double averageFlops = 0.0;
    for(int g = 0; g < FLOPS_SAMPLES; g++){
        if(g%10000 == 0)
            std::cout << "Benchmarking...  " << (g) << " of " << FLOPS_SAMPLES << " samples complete          \r";

        double secondValue = rand()+67858;
        double startValue = 123891.123871;
        
    	auto start = high_resolution_clock::now();
    
        // 4000 total operations (about)
        for(int i = 0; i < 1000; i++){
            startValue += secondValue;
            startValue *= secondValue;
            startValue -= secondValue;
            startValue /= secondValue;
        }
        
    	auto stop = high_resolution_clock::now();
    
    	auto duration = duration_cast<microseconds>(stop - start);
    	
    	averageFlops += (1000000.0/(double)(duration.count())*4000.0)/(double)FLOPS_SAMPLES;
    }

    /*std::cout << "                                                          \r";
	cout << "FLOPS: " << truncateNum(averageFlops)  <<"flops"<< endl;*/

    return (unsigned long long)averageFlops * (unsigned long long)processor_count;
}
