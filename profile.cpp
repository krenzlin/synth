#ifndef ARDUINO

#include <chrono>
#include <iostream>
#include "generators.h"


int main() {
    Saw osc;
    osc.set_frequency(440.0);

    float buffer[SAMPLE_RATE];
    auto start = std::chrono::high_resolution_clock::now(); 

    for (auto k=0; k<1000; k++) {
        for (auto i=0; i<SAMPLE_RATE; i++) {
            buffer[i] = osc.next_sample();
        }
    }
    auto stop = std::chrono::high_resolution_clock::now(); 

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl; 

    return 0;
}

#endif
