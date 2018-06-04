#include <vector>
#include <array>
#include <sstream>
#include <cstring>
#include <iostream>

#ifndef DBITS_DBITS_H
#define DBITS_DBITS_H

using int32 = int32_t;
using uint32 = uint32_t;
using uchar  = u_char;
const size_t size_dbl = sizeof(double);
const size_t size_u32 = sizeof(uint32_t);
using Bytes  = std::array<uchar, size_dbl>;

template<typename T>
auto bytes(T value) {
    std::array<uchar, sizeof(T)> v{};
    const auto *ptr = reinterpret_cast<const uchar *>(&value);
    for (size_t i = 0; i < sizeof(T); ++i)
        v[i] = ptr[i];
    return v;
}

double bytes_to_val(Bytes &buffer) {
    double d;
    memcpy(&d, buffer.data(), size_dbl);
    return d;
}

std::string bytes_to_str(Bytes &buf) {
    std::ostringstream ss;
    auto n = buf.size() - 1;
    ss << "[";
    for (size_t i = 0; i <= n; ++i) {
        auto v = static_cast<int>(buf[i]);
        (i < n) ? ss << v << ", " : ss << v;
    };
    ss << "]";
    return ss.str();
}

uint32_t read_as_u32(std::array<u_char, sizeof(uint32_t)> bytes) {
    uint32_t d;
    memcpy(&d, bytes.data(), size_u32);
    return d;
}

std::array<uint32, 2> double_bits(double n) {
    uint32 a, b;
    auto buf = bytes(n);

    std::array<u_char, size_u32> first{};
    std::array<u_char, size_u32> second{};

    std::copy(buf.begin(), buf.begin() + size_u32, first.begin());
    std::copy(buf.begin() + size_u32, buf.end(), second.begin());

    a = read_as_u32(first);
    b = read_as_u32(second);
    return {a, b};
}

///pack low and higher uints of double as double
double pack(uint32 lo, uint32 hi) {
    auto buffer_b = bytes(hi);
    Bytes buffer;

    auto buf = bytes(lo);
    std::copy(buf.begin(), buf.end(), buffer.begin());

    buf = bytes(hi);
    std::copy(buf.begin(), buf.end(), buffer.begin() + size_u32);

    return bytes_to_val(buffer);
}


uint32 low(double n) {
    auto buffer = bytes(n);
    std::array<u_char, size_u32> buf{};
    std::copy(buffer.begin(), buffer.begin() + size_u32, buf.begin());
    return read_as_u32(buf);
}

uint32 high(double n) {
    auto buffer = bytes(n);
    std::array<u_char, size_u32> buf{};
    std::copy(buffer.begin() + size_u32, buffer.end(), buf.begin());
    return read_as_u32(buf);
}

uint32 sign(double n) {
    return high(n) >> 31u;
}

int32 exponent(double n) {
    auto b = high(n);
    return ((b << 1u) >> 21u) - 1023;
}

std::array<uint32, 2> fraction(double n) {
    auto l = low(n);
    auto h = high(n);
    auto b = static_cast<uint32>(h & ((1u << 20u) - 1u));
    if ((h & 0x7ff00000u) != 0) {
        b += 1u << 20u;
    }
    return {l, b};
}

bool denormalized(double n) {
    auto h = high(n);
    return (h & 0x7ff00000ul) == 0;
}

#endif //DBITS_DBITS_H
