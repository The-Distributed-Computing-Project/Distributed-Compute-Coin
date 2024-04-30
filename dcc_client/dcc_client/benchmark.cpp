#include <algorithm>
#include <chrono>
#include <iostream>
#include<vector>
#include <string>
#include "benchmark.h"
using namespace std;
using namespace std::chrono;

std::string truncateNum(double x){
    if(x > 1000000000000)
        return std::to_string((long long)x/1000000000000) + " T";
    else if(x > 1000000000)
        return std::to_string((long)x/1000000000) + " B";
    else if(x > 1000000)
        return std::to_string((long)x/1000000) + " M";
    else if(x > 1000)
        return std::to_string((long)x/1000) + " K";
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

    std::cout << "                                                          \r";
	cout << "FLOPS: " << truncateNum(averageFlops)  <<"flops"<< endl;

    return (unsigned long long)averageFlops;
}
