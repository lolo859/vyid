#include <stdint.h>
#include <cstring>
#include <array>
namespace base85 {
    constexpr char Z85_CHARS[]=
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        ".~:+=^!/*?&<>()_|{}@%$#";
    constexpr int8_t get_decode_val(char c) {
        for (int i=0;i<85;++i) {
            if (Z85_CHARS[i]==c) {
                return i;
            }
        }
        return -1;
    }
    template<size_t... Is>
    constexpr std::array<int8_t,256> make_decode_map_impl(std::index_sequence<Is...>) {
        return {{(int8_t)get_decode_val(static_cast<char>(Is))...}};
    }
    constexpr std::array<int8_t,256> Z85_DECODE_MAP_ARRAY=make_decode_map_impl(std::make_index_sequence<256>{});
    inline void uint32_to_b85(uint32_t value,char* out) {
        constexpr uint32_t d1=52200625;
        constexpr uint32_t d2=614125;
        constexpr uint32_t d3=7225;
        constexpr uint32_t d4=85;
        uint32_t q1=value/d1;
        value-=q1*d1;
        uint32_t q2=value/d2;
        value-=q2*d2;
        uint32_t q3=value/d3;
        value-=q3*d3;
        uint32_t q4=value/d4;
        value-=q4*d4;
        out[0]=Z85_CHARS[q1];
        out[1]=Z85_CHARS[q2];
        out[2]=Z85_CHARS[q3];
        out[3]=Z85_CHARS[q4];
        out[4]=Z85_CHARS[value];
        out[5]='\0';
    }
    inline void uint64_to_b85(uint64_t value,char* out) {
        uint32_to_b85(static_cast<uint32_t>(value>>32),out);
        uint32_to_b85(static_cast<uint32_t>(value & 0xFFFFFFFF),out+5);
        out[10]='\0';
    }
    inline int b85_to_uint32_internal(const char* in,uint32_t& value) {
        constexpr uint32_t p1=85;
        constexpr uint32_t p2=85*85;
        constexpr uint32_t p3=85*85*85;
        constexpr uint32_t p4=85*85*85*85;
        uint32_t v0=Z85_DECODE_MAP_ARRAY[static_cast<unsigned char>(in[0])];
        uint32_t v1=Z85_DECODE_MAP_ARRAY[static_cast<unsigned char>(in[1])];
        uint32_t v2=Z85_DECODE_MAP_ARRAY[static_cast<unsigned char>(in[2])];
        uint32_t v3=Z85_DECODE_MAP_ARRAY[static_cast<unsigned char>(in[3])];
        uint32_t v4=Z85_DECODE_MAP_ARRAY[static_cast<unsigned char>(in[4])];
        if (v0==-1 || v1==-1 || v2==-1 || v3==-1 || v4==-1) {
            value=0;
            return 1;
        }
        value=v0*p4+v1*p3+v2*p2+v3*p1+v4;
        return 0;
    }
    inline int b85_to_uint32(const char* in,uint32_t& out) {
        return b85_to_uint32_internal(in,out);
    }
    inline int b85_to_uint64(const char* in,uint64_t& out) {
        uint32_t high_part,low_part;
        int error=0;
        error|=b85_to_uint32_internal(in,high_part);
        error|=b85_to_uint32_internal(in+5,low_part);
        out=(static_cast<uint64_t>(high_part)<<32) | low_part;
        return error;
    }
}