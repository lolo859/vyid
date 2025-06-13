//* VYID is under the MIT license.
//* It depends on XXHash (BSD 2-Clause License) and XoshiroCpp (MIT license).
//* Please ensure you comply with their respective licenses.
//* Define VYID_NO_OS_DEPENDENCIES to disable OS functionnalities, such as string outputing and automatic seed generation
#ifndef VYID_H
#define VYID_H
#include <cstdlib>
#include <cstring>
#ifndef VYID_NO_OS_DEPENDENCIES
#include <string>
#endif
#include <random>
#include "XoshiroCpp.hpp"
extern "C" {
    #include "xxhash.h"
}
#include "base85.h"
#define VYID_MIN_BUFFER_SIZE 62
namespace vyid {
    //* A struct container for random values used by generate_vyid_raw
    struct random_values {
        uint64_t rand1;
        uint64_t rand2;
        uint64_t rand3;
    };
    //* Function to generate a VYID, made for single use
    //* gen : the random instance of Xoshiro256StarStar, seeded or no
    //* rand : the random_values struct already created
    //* Return the string containing the VYID
    #ifndef VYID_NO_OS_DEPENDENCIES
    std::string generate_vyid_string_raw(XoshiroCpp::Xoshiro256StarStar& gen,random_values& rand) {
        rand.rand1=gen();
        rand.rand2=gen();
        rand.rand3=gen();
        uint64_t seed=rand.rand1^rand.rand2^rand.rand3;
        struct pair64{uint64_t a,b;};
        pair64 tempstruct=pair64{rand.rand2,rand.rand3};
        uint64_t sign1h=XXH3_64bits_withSeed(&tempstruct,sizeof(tempstruct),seed);
        tempstruct=pair64{rand.rand1,rand.rand3};
        uint64_t sign2h=XXH3_64bits_withSeed(&tempstruct,sizeof(tempstruct),seed);
        tempstruct=pair64{rand.rand1,rand.rand2};
        uint64_t sign3h=XXH3_64bits_withSeed(&tempstruct,sizeof(tempstruct),seed);
        uint64_t lenc1=rand.rand1^sign1h;
        uint64_t lenc2=rand.rand2^sign2h;
        uint64_t lenc3=rand.rand3^sign3h;
        uint32_t enc1=lenc1>>32;
        uint32_t enc2=lenc2>>32;
        uint32_t enc3=lenc3>>32;
        struct triple64{uint64_t a,b,c;};
        triple64 tempst={enc1,enc2,enc3};
        auto sumtotal=XXH3_64bits_withSeed(&tempst,sizeof(tempst),seed);
        uint32_t trunctotal=sumtotal>>32;
        auto sumgrp=XXH3_64bits_withSeed(&trunctotal,sizeof(trunctotal),seed);
        uint32_t truncgrp=sumgrp>>32;
        alignas(128) char output[70];
        alignas(8) char grp[6];
        alignas(8) char hash[6];
        alignas(8) char enchar1[6];
        alignas(8) char enchar2[6];
        alignas(8) char enchar3[6];
        alignas(16) char rand1[11];
        alignas(16) char rand2[11];
        alignas(16) char rand3[11];
        base85::uint32_to_b85(truncgrp,grp);
        base85::uint32_to_b85(enc1,enchar1);
        base85::uint32_to_b85(enc2,enchar2);
        base85::uint32_to_b85(enc3,enchar3);
        base85::uint64_to_b85(rand.rand1,rand1);
        base85::uint64_to_b85(rand.rand2,rand2);
        base85::uint64_to_b85(rand.rand3,rand3);
        base85::uint32_to_b85(trunctotal,hash);
        int offset=0;
        output[offset++]='[';
        memcpy(output+offset,grp,5);
        offset+=5;
        output[offset++]='-';
        memcpy(output+offset,enchar1,5);
        offset+=5;
        memcpy(output+offset,rand1,10);
        offset+=10;
        output[offset++]='-';
        memcpy(output+offset,enchar2,5);
        offset+=5;
        memcpy(output+offset,rand2,10);
        offset+=10;
        output[offset++]='-';
        memcpy(output+offset,enchar3,5);
        offset+=5;
        memcpy(output+offset,rand3,10);
        offset+=10;
        output[offset++]='-';
        memcpy(output+offset,hash,5);
        offset+=5;
        output[offset++]=']';
        output[offset]='\0';
        return std::string(output,output+62);
    }
    #endif
    //* Function to generate a VYID, made for single use
    //* gen : the random instance of Xoshiro256StarStar, seeded or no
    //* rand : the random_values struct already created
    //* output : the const char* to put the VYID, must be at least 62 in size
    //* output_size : the size of the output buffer
    //* Return a int : 0 if sucess, 1 if failure
    int generate_vyid_char_raw(XoshiroCpp::Xoshiro256StarStar& gen,random_values& rand,char* output,size_t output_size) {
        if (__builtin_expect(output_size<VYID_MIN_BUFFER_SIZE,0)) {
            if (__builtin_expect(output_size>0,0)) {
                output[0]='\0';
            }
            return 1;
        }
        rand.rand1=gen();
        rand.rand2=gen();
        rand.rand3=gen();
        uint64_t seed=rand.rand1^rand.rand2^rand.rand3;
        struct pair64{uint64_t a,b;};
        pair64 tempstruct=pair64{rand.rand2,rand.rand3};
        uint64_t sign1h=XXH3_64bits_withSeed(&tempstruct,sizeof(tempstruct),seed);
        tempstruct=pair64{rand.rand1,rand.rand3};
        uint64_t sign2h=XXH3_64bits_withSeed(&tempstruct,sizeof(tempstruct),seed);
        tempstruct=pair64{rand.rand1,rand.rand2};
        uint64_t sign3h=XXH3_64bits_withSeed(&tempstruct,sizeof(tempstruct),seed);
        uint64_t lenc1=rand.rand1^sign1h;
        uint64_t lenc2=rand.rand2^sign2h;
        uint64_t lenc3=rand.rand3^sign3h;
        uint32_t enc1=lenc1>>32;
        uint32_t enc2=lenc2>>32;
        uint32_t enc3=lenc3>>32;
        struct triple64{uint64_t a,b,c;};
        triple64 tempst={enc1,enc2,enc3};
        auto sumtotal=XXH3_64bits_withSeed(&tempst,sizeof(tempst),seed);
        uint32_t trunctotal=sumtotal>>32;
        auto sumgrp=XXH3_64bits_withSeed(&trunctotal,sizeof(trunctotal),seed);
        uint32_t truncgrp=sumgrp>>32;
        alignas(8) char grp[6];
        alignas(8) char hash[6];
        alignas(8) char enchar1[6];
        alignas(8) char enchar2[6];
        alignas(8) char enchar3[6];
        alignas(16) char rand1[11];
        alignas(16) char rand2[11];
        alignas(16) char rand3[11];
        base85::uint32_to_b85(truncgrp,grp);
        base85::uint32_to_b85(enc1,enchar1);
        base85::uint32_to_b85(enc2,enchar2);
        base85::uint32_to_b85(enc3,enchar3);
        base85::uint64_to_b85(rand.rand1,rand1);
        base85::uint64_to_b85(rand.rand2,rand2);
        base85::uint64_to_b85(rand.rand3,rand3);
        base85::uint32_to_b85(trunctotal,hash);
        int offset=0;
        output[offset++]='[';
        memcpy(output+offset,grp,5);
        offset+=5;
        output[offset++]='-';
        memcpy(output+offset,enchar1,5);
        offset+=5;
        memcpy(output+offset,rand1,10);
        offset+=10;
        output[offset++]='-';
        memcpy(output+offset,enchar2,5);
        offset+=5;
        memcpy(output+offset,rand2,10);
        offset+=10;
        output[offset++]='-';
        memcpy(output+offset,enchar3,5);
        offset+=5;
        memcpy(output+offset,rand3,10);
        offset+=10;
        output[offset++]='-';
        memcpy(output+offset,hash,5);
        offset+=5;
        output[offset++]=']';
        output[offset]='\0';
        return 0;
    }
    //* Function to generate a VYID
    //* Use this fonction if you have access to random seed provider
    //* Automatically recycle random instance for bulk VYID generation
    //* Return the string containing the VYID
    #ifndef VYID_NO_OS_DEPENDENCIES
    std::string generate_vyid_string() {
        static XoshiroCpp::Xoshiro256StarStar ran = []() {std::random_device rd;
            std::mt19937_64 seeder(rd());
            std::array<uint64_t,4> seed_data={seeder(),seeder(),seeder(),seeder()};
            return XoshiroCpp::Xoshiro256StarStar(seed_data);
        }();
        static random_values rands;
        return generate_vyid_string_raw(ran,rands);
    }
    //* Function to generate a VYID with a specific seed
    //* Use this function if you don't have a access to a random seed provider
    //* That function is the one to use for bare metal usage if you want to get the result as a string
    //* Automatically recycle random instance for bulk VYID generation
    //* seed : a array of 4 uint64_t
    //* Return the string containing the VYID
    std::string generate_vyid_string_seed(std::array<uint64_t,4> seed) {
        static XoshiroCpp::Xoshiro256StarStar ran(seed);
        static random_values rands;
        return generate_vyid_string_raw(ran,rands);
    }
    //* Function to generate a VYID
    //* Use this fonction if you have access to random seed provider
    //* Automatically recycle random instance for bulk VYID generation
    //* output : the const char* to put the VYID, must be at least 62 in size
    //* output_size : the size of the output buffer
    //* Return a int : 0 if sucess, 1 if failure
    int generate_vyid_char(char* output,size_t output_size) {
        static XoshiroCpp::Xoshiro256StarStar ran = []() {std::random_device rd;
            std::mt19937_64 seeder(rd());
            std::array<uint64_t,4> seed_data={seeder(),seeder(),seeder(),seeder()};
            return XoshiroCpp::Xoshiro256StarStar(seed_data);
        }();
        static random_values rands;
        return generate_vyid_char_raw(ran,rands,output,output_size);
    }
    #endif
    //* Function to generate a VYID with a specific seed
    //* Use this function if you don't have a access to a random seed provider
    //* That function is the one to use for bare metal usage if you want to get the result as a char
    //* Automatically recycle random instance for bulk VYID generation
    //* seed : a array of 4 uint64_t
    //* output : the const char* to put the VYID, must be at least 62 in size
    //* output_size : the size of the output buffer
    //* Return a int : 0 if sucess, 1 if failure
    int generate_vyid_char_seed(std::array<uint64_t,4> seed,char* output,size_t output_size) {
        static XoshiroCpp::Xoshiro256StarStar ran(seed);
        static random_values rands;
        return generate_vyid_char_raw(ran,rands,output,output_size);
    }
    //* That function is the one to verify any VYID and detecting eventual corruption
    //* This function fully work in a bare metal environnement
    //* vyid : the char* which contain the VYID to verify
    //* vyid_size : the size of the char that contain the VYID
    //* Return a boolean : true if the VYID pass all tests, false if one of the test failed
    bool check_vyid(const char* vyid,size_t vyid_size) {
        if (__builtin_expect(vyid==nullptr,0)) {
            return false;
        }
        if (__builtin_expect(vyid_size<VYID_MIN_BUFFER_SIZE,0)) {
            return false;
        }
        if (__builtin_expect(vyid[0]!='[' || vyid[VYID_MIN_BUFFER_SIZE-2]!=']' || vyid[6]!='-' || vyid[22]!='-' || vyid[38]!='-' || vyid[54]!='-',0)) {
            return false;
        }
        alignas(8) char grp[6];
        alignas(8) char hash[6];
        alignas(8) char enchar1[6];
        alignas(8) char enchar2[6];
        alignas(8) char enchar3[6];
        alignas(16) char rand1[11];
        alignas(16) char rand2[11];
        alignas(16) char rand3[11];
        std::memcpy(grp,vyid+1,5);
        grp[5]='\0';
        std::memcpy(enchar1,vyid+7,5);
        enchar1[5]='\0';
        std::memcpy(rand1,vyid+12,10);
        rand1[10]='\0';
        std::memcpy(enchar2,vyid+23,5);
        enchar2[5]='\0';
        std::memcpy(rand2,vyid+28,10);
        rand2[10]='\0';
        std::memcpy(enchar3,vyid+39,5);
        enchar3[5]='\0';
        std::memcpy(rand3,vyid+44,10);
        rand3[10]='\0';
        std::memcpy(hash,vyid+55,5);
        hash[5]='\0';
        uint32_t igrp;
        uint32_t ihash;
        uint32_t encint1;
        uint32_t encint2;
        uint32_t encint3;
        uint64_t irand1;
        uint64_t irand2;
        uint64_t irand3;
        if (__builtin_expect(base85::b85_to_uint32(grp,igrp)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint32(hash,ihash)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint32(enchar1,encint1)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint32(enchar2,encint2)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint32(enchar3,encint3)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint64(rand1,irand1)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint64(rand2,irand2)!=0,0)) return false;
        if (__builtin_expect(base85::b85_to_uint64(rand3,irand3)!=0,0)) return false;
        uint64_t seed=irand1^irand2^irand3;
        struct pair64 {uint64_t a,b;};
        pair64 p1{irand2,irand3};
        uint64_t sign1h=XXH3_64bits_withSeed(&p1,sizeof(p1),seed);
        pair64 p2{irand1,irand3};
        uint64_t sign2h=XXH3_64bits_withSeed(&p2,sizeof(p2),seed);
        pair64 p3{irand1,irand2};
        uint64_t sign3h=XXH3_64bits_withSeed(&p3,sizeof(p3),seed);
        uint64_t enc1=(irand1^sign1h)>>32;
        uint64_t enc2=(irand2^sign2h)>>32;
        uint64_t enc3=(irand3^sign3h)>>32;
        if (__builtin_expect(encint1!=enc1,0)) {
            return false;
        }
        if (__builtin_expect(encint2!=enc2,0)) {
            return false;
        }
        if (__builtin_expect(encint3!=enc3,0)) {
            return false;
        }
        struct triple64 {uint64_t a,b,c;};
        triple64 tempst={enc1,enc2,enc3};
        auto sumtotal=XXH3_64bits_withSeed(&tempst,sizeof(tempst),seed);
        uint32_t trunctotal=sumtotal>>32;
        if (__builtin_expect(trunctotal!=ihash,0)) {
            return false;
        }
        alignas(4) char trunctotal_bytes[4];
        std::memcpy(trunctotal_bytes,&trunctotal,4);
        auto sumgrp=XXH3_64bits_withSeed(trunctotal_bytes,sizeof(trunctotal_bytes),seed);
        uint32_t truncgrp=sumgrp>>32;
        if (__builtin_expect(truncgrp!=igrp,0)) {
            return false;
        }
        return true;
    }
}
#endif