#include <iostream>
#include <chrono>
#include <random>
#include <stdlib.h>
#include <array>
#include <iomanip>
#include "../vyid.h"
using namespace std;
using namespace std::chrono;
char out[VYID1_MIN_BUFFER_SIZE];
array<uint64_t,4> seed={UINT64_MAX,UINT64_MAX,UINT64_MAX,UINT64_MAX};
int main() {
    const size_t iterations=10000000;
    cout<<"---------------- VYID Benchmark with "<<iterations<<" iterations ----------------"<<endl;
    char out[VYID1_MIN_BUFFER_SIZE];
    vyid::v1::generate_vyid_char(out,sizeof(out));
    cout<<"v1 : Generation, output as char, constant seed provided"<<endl;
    auto start=high_resolution_clock::now();
    for (size_t i=0;i<iterations;++i) {
        vyid::v1::generate_vyid_char_seed(seed,out,sizeof(out));
        asm volatile("" : : "r"(out[0]) : "memory");
    }
    auto end=high_resolution_clock::now();
    auto duration=std::chrono::duration_cast<std::chrono::duration<double>>(end-start);
    std::cout<<"Total time: "<<duration.count()<<" seconds\n";
    std::cout<<"Average time per call: "<<(duration.count()/iterations)*1000000<<" µs\n";
    std::cout << "VYIDs per second: "<<std::fixed<<std::setprecision(0)<<iterations/duration.count() << "\n";
    return 0;
}