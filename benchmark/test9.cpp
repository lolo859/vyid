#include <iostream>
#include <chrono>
#include <random>
#include <stdlib.h>
#include <array>
#include <iomanip>
#include "../vyid.h"
using namespace std;
using namespace std::chrono;
char out[VYID2_MIN_BUFFER_SIZE]={};
char payload_32[32]="abcdefghabcdefghabcdefghabcdefg";
uint8_t seed[16]={UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX,UINT8_MAX};
int main() {
    vyid::v2::initialize_csprng(seed);
    const size_t iterations=10000000;
    cout<<"---------------- VYID Benchmark with "<<iterations<<" iterations ----------------"<<endl;
    vyid::v2::generate_vyid_char(out,sizeof(out),payload_32,0);
    cout<<"v2 : Generation with no payload, constant provided seed"<<endl;
    auto start=high_resolution_clock::now();
    for (size_t i=0;i<iterations;++i) {
        vyid::v2::generate_vyid_char(out,sizeof(out),payload_32,0);
        asm volatile("" : : "r"(out[0]) : "memory");
    }
    auto end=high_resolution_clock::now();
    auto duration=std::chrono::duration_cast<std::chrono::duration<double>>(end-start);
    std::cout<<"Total time: "<<duration.count()<<" seconds\n";
    std::cout<<"Average time per call: "<<(duration.count()/iterations)*1000000<<" µs\n";
    std::cout << "VYIDs per second: "<<std::fixed<<std::setprecision(0)<<iterations/duration.count() << "\n";
    return 0;
}
