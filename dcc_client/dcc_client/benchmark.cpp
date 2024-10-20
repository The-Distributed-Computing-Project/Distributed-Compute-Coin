#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include "benchmark.h"
#include "strops.h"

using namespace std;
using namespace std::chrono;

std::atomic_bool stopBenchmarkThread = false;

#pragma GCC push_options
#pragma GCC optimize("O0")
void calculateFLOPS(unsigned long long *p1){
	double a= 124.23525, b = 21.2412, c = 2342.23432, d = 23.324, e= 2.3412, f = 123.21, g = 1231.12, h =567.4, j = 34.4, k =24, l =342.24,
	 m= 324.23525, n = 51.2412, o = 242.23432, p = 254.324, q= 112.3412, r = 853.21, s = 31.12, t =67.4, u = 34.4, v =4, w =3.24, x =89.131, y =123.23, z =123.123,
	 v1 =12.3, v2 = 3, v3 = 56.5, v4 = 88.56, v5 = 787.43, v6 = 0, v7 = 0, v8 =0, v9 = 0, v10 = 1, v11 = 54, v12 = 12, v13= 45, v14 = 5.66, v15 = 123.12, v16 =1 ,
	 v17 =1, v18 = 12, v19 =12, v20 = 12.1, v21 = 1.1, v22 = 12;

	/* total operations = 33
		inside loop : 16 arithmatic and 17 assignment operations
		while loop : 1 loop condition 
	*/
	unsigned long long noOfOperations = 0, loopOperations = 33;


	// run the loop
	while(stopBenchmarkThread == false/* && noOfOperations < 999999999999999999 */)
	{
		a = a + b;
		c = d + e;
		f = g * h;
		m = m + n;
		j = k * l;


		q = o + p;
		s = r + s;
		v = t * u;
		m = k + n;
		j = w * l;

		x = x * y;
		v6 = v1 + v2;
		v7 = v3 * v4;
		v8 = v5 * v5;
		v9 = g * k;	

		// calculate total number of floating point operations
		noOfOperations = noOfOperations + loopOperations;		
		*((unsigned long long *)p1) = noOfOperations;
		//maxValue++;
	}
	return;
}

#define BENCHMARK_SAMPLES 2
unsigned long long benchmark(){
	unsigned long long flops1[BENCHMARK_SAMPLES];
	unsigned long long flops = 0, prevFlops1 = 0; 
	unsigned long long tFlops1 = 0; 


	indicators::show_console_cursor(false);
	using namespace indicators;
	indicators::ProgressSpinner spinner{
		option::PostfixText{"Benchmarking "},
		option::ForegroundColor{Color::yellow},
		option::SpinnerStates{std::vector<std::string>{"-", "\\", "|", "/"}},
		option::FontStyles{std::vector<FontStyle>{FontStyle::bold}},
		option::ShowRemainingTime{true},
		option::ShowPercentage{false}
	};


	std::thread benchThread(&calculateFLOPS, &tFlops1);
	//Calculates the 5 flops samples	
	for(int i = 0 ; i < BENCHMARK_SAMPLES; i++)
	{
		// sleep the main thread for 1 second
		//for (int j = 0; j < 10; j++){
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			//spinner.set_progress(100*i/BENCHMARK_SAMPLES);
		//}

		spinner.set_progress(100*i/BENCHMARK_SAMPLES);
		unsigned long long dif = tFlops1 - prevFlops1;
		double difd = (double)dif;
		spinner.set_option(option::PostfixText{"Benchmarking -- ("+truncateMetricNum(difd*processor_count) + "Flops)"});
		prevFlops1 = tFlops1;
		flops += dif;
	}
	stopBenchmarkThread = true;
	benchThread.join();

	indicators::show_console_cursor(true);

	//cancel all the created threads
	//pthread_cancel(memThreads[0]);
	return flops / BENCHMARK_SAMPLES * processor_count;
}
#pragma GCC pop_options

//#define FLOPS_SAMPLES 100000
//unsigned long long benchmark()
//{
//
//    double averageFlops = 0.0;
//    for(int g = 0; g < FLOPS_SAMPLES; g++){
//        if(g%10000 == 0)
//            std::cout << "Benchmarking...  " << (g) << " of " << FLOPS_SAMPLES << " samples complete          \r";
//
//        double secondValue = rand()+67858;
//        double startValue = 123891.123871;
//        
//    	auto start = high_resolution_clock::now();
//    
//        // 4000 total operations (about)
//        for(int i = 0; i < 1000; i++){
//            startValue += secondValue;
//            startValue *= secondValue;
//            startValue -= secondValue;
//            startValue /= secondValue;
//        }
//        
//    	auto stop = high_resolution_clock::now();
//    
//    	auto duration = duration_cast<microseconds>(stop - start);
//    	
//    	averageFlops += (1000000.0/(double)(duration.count())*4000.0)/(double)FLOPS_SAMPLES;
//    }
//
//    /*std::cout << "                                                          \r";
//	cout << "FLOPS: " << truncateMetricNum(averageFlops)  <<"flops"<< endl;*/
//
//    return (unsigned long long)averageFlops * (unsigned long long)processor_count;
//}
