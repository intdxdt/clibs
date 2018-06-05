#include <cstdint>

#ifndef TWIDDLE_CPP_H
#define TWIDDLE_CPP_H
/**
 * Based on Stanford bit twiddling hack library
 *    http//graphics.stanford.edu/~seander/bithacks.html
 */
using uint32 =  uint32_t;
using int32  =  int32_t;
//const uint32 INT_BITS = 32; //number of bits in an integer
//const int32 INT_MAX = std::numeric_limits<int>::max();
//const int32 INT_MIN = std::numeric_limits<int>::min();
//@formatter:off
const uint32 REVERSE_TABLE[256] = {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48,
	176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248, 4,
	132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 12, 140, 76, 204, 44,
	172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252, 2, 130, 66, 194, 34, 162, 98, 226, 18,
	146, 82, 210, 50, 178, 114, 242, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58,
	186, 122, 250, 6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14,
	142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 1, 129, 65, 193, 33,
	161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169, 105, 233, 25,
	153, 89, 217, 57, 185, 121, 249, 5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53,
	181, 117, 245, 13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 3,
	131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43,
	171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167, 103, 231, 23,
	151, 87, 215, 55, 183, 119, 247, 15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63,
	191, 127, 255};
//@formatter:on



//Returns -1, 0, +1 depending on sign of x
inline int32 sign(int32 v) {
    return (v > 0) - (v < 0);
}


//Checks if a number is a power of two
bool inline is_pow2(int32 v) {
    return (v != 0) && ((v & (v - 1)) == 0);
}

//Computes log base 2 of v
inline uint32 log2(uint32 v) {
    uint32 r{0}, shift{0};
    //@formatter:off
    r = static_cast<uint32>((v > 0xFFFF) << 4u);
    v >>= r;
    shift = static_cast<uint32>((v > 0xFF) << 3u);
    v >>= shift;
    r |= shift;
    shift = static_cast<uint32>((v > 0xF) << 2u);
    v >>= shift;
    r |= shift;
    shift = static_cast<uint32>((v > 0x3) << 1u);
    v >>= shift;
    r |= shift;
    return r | (v >> 1u);
    //@formatter:on

}

//Computes log base 10 of v
int32 log10(int32 v) {
    //@formatter:off
    return (v >= 1000000000) ? 9 :
           (v >= 100000000) ? 8 :
           (v >= 10000000) ? 7 :
           (v >= 1000000) ? 6 :
           (v >= 100000) ? 5 :
           (v >= 10000) ? 4 :
           (v >= 1000) ? 3 :
           (v >= 100) ? 2 :
           (v >= 10) ? 1 : 0;
    //@formatter:on
}

//Counts number of bits
uint32 pop_count(uint32 v) {
    //    other solution
    //    var  v = v - ((v >> 1) & 0x55555555)
    //    v = (v & 0x33333333) + ((v >> 2) & 0x33333333)
    //    ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24 //int32 & uint32 cause overflow on this line
    uint32 c{0};
    uint32 s0{1}, s1{2}, s2{4}, s3{8}, s4{16}; // magic binary numbers
    uint32 b0{0x55555555}, b1{0x33333333}, b2{0x0F0F0F0F}, b3{0x00FF00FF}, b4{0x0000FFFF};

    c = v - ((v >> s0) & b0);
    c = ((c >> s1) & b1) + (c & b1);
    c = ((c >> s2) + c) & b2;
    c = ((c >> s3) + c) & b3;
    return ((c >> s4) + c) & b4;
}

//
////finding the log base 2 in parallel
////first isolate the lowest 1 bit, and then
//// proceed with c starting at the maximum and decreasing
////func count_trailing_zeros(v uint32)  uint32 {
////    var  c = 32uint32
////    var sv = v as int32
////    v = (sv & -sv) as uint32 //NOTE may overflow uint32  -int32
////    if v != 0 { c -= 1 }
////    if v & 0x0000FFFF != 0 { c -= 16 }
////    if v & 0x00FF00FF != 0 { c -= 8 }
////    if v & 0x0F0F0F0F != 0 { c -= 4 }
////    if v & 0x33333333 != 0 { c -= 2 }
////    if v & 0x55555555 != 0 { c -= 1 }
////    c
////}
////Computes the number of trailing zeros by accumulating c
//// in a manner akin to binary search
uint32 count_trailing_zeros(uint32 v) {
    uint32 c = 32;
    // NOTE if 0 == v, then c = 31.
    //short circuit v == 0 as 32
    if (v == 0) {
        return c;
    }
    if ((v & 0x1u) != 0) {
        c = 0; // special case for odd v (assumed to happen half of the time)
    } else {
        c = 1;
        if ((v & 0xffffu) == 0) {
            v >>= 16;
            c += 16;
        }
        if ((v & 0xffu) == 0) {
            v >>= 8;
            c += 8;
        }
        if ((v & 0xfu) == 0) {
            v >>= 4;
            c += 4;
        }
        if ((v & 0x3u) == 0) {
            v >>= 2;
            c += 2;
        }
        c -= v & 0x1u;
    }
    return c;
}

////Rounds to next power of 2
int32 next_pow2(uint32 v) {
    v += (v == 0);
    v -= 1;
    v |= v >> 1u;
    v |= v >> 2u;
    v |= v >> 4u;
    v |= v >> 8u;
    v |= v >> 16u;
    return v + 1u;
}

////Rounds down to previous power of 2
int32 prev_pow2(uint32 v) {
    v |= v >> 1u;
    v |= v >> 2u;
    v |= v >> 4u;
    v |= v >> 8u;
    v |= v >> 16u;
    return v - (v >> 1u);
}

////Computes parity of word
int32 parity(uint32 v) {
    v ^= v >> 16u;
    v ^= v >> 8u;
    v ^= v >> 4u;
    v &= 0xf;
    return (0x6996u >> v) & 1u;
}

////func _gen_reverse_table(table&mut [int32256]) {
////  for i in  0..256 {
////    var   v = i
////    var  r = i
////    var  s = 7
////    v >>= 1
////    while v != 0 {
////      r <<= 1
////      r |= v & 1
////      s-=1
////      v >>= 1
////    }
////    table[i] = (r << s) & 0xff
////  }
////}
//
////Reverse bits in a 32 bit word
uint32 reverse(uint32 v) {
    //@formatter:off
    return (REVERSE_TABLE[v  & 0xffu      ] << 24u) |
           (REVERSE_TABLE[v >> 8u  & 0xffu] << 16u) |
           (REVERSE_TABLE[v >> 16u & 0xffu] << 8u)  |
           REVERSE_TABLE [v >> 24u & 0xffu];
    //@formatter:on
}

////Interleave bits of 2 coordinates with 16
//// Useful for fast quadtree codes
uint32 interleave2(uint32 x, uint32 y) {
    x &= 0xFFFF;
    x = (x | (x << 8u)) & 0x00FF00FFu;
    x = (x | (x << 4u)) & 0x0F0F0F0Fu;
    x = (x | (x << 2u)) & 0x33333333u;
    x = (x | (x << 1u)) & 0x55555555u;

    y &= 0xFFFF;
    y = (y | (y << 8u)) & 0x00FF00FFu;
    y = (y | (y << 4u)) & 0x0F0F0F0Fu;
    y = (y | (y << 2u)) & 0x33333333u;
    y = (y | (y << 1u)) & 0x55555555u;

    return x | (y << 1u);
}

////Extracts the nth interleaved component
//func Deinterleave2(v uint32, n uint32) uint32 {
//	//@formatter:off
//	v = (v >> n) & 0x55555555
//	v = (v | (v >> 1)) & 0x33333333
//	v = (v | (v >> 2)) & 0x0F0F0F0F
//	v = (v | (v >> 4)) & 0x00FF00FF
//	v = (v | (v >> 16)) & 0x000FFFF
//	return (v << 16) >> 16
//	//@formatter:on
//}
//
////Interleave bits of 3 coordinates, each with 10
//// Useful for fast octree codes
//func Interleave3(x uint32, y uint32, z uint32) uint32 {
//	x &= 0x3FF
//	x = (x | (x << 16)) & 4278190335
//	x = (x | (x << 8)) & 251719695
//	x = (x | (x << 4)) & 3272356035
//	x = (x | (x << 2)) & 1227133513
//
//	y &= 0x3FF
//	y = (y | (y << 16)) & 4278190335
//	y = (y | (y << 8)) & 251719695
//	y = (y | (y << 4)) & 3272356035
//	y = (y | (y << 2)) & 1227133513
//	x |= y << 1
//
//	z &= 0x3FF
//	z = (z | (z << 16)) & 4278190335
//	z = (z | (z << 8)) & 251719695
//	z = (z | (z << 4)) & 3272356035
//	z = (z | (z << 2)) & 1227133513
//
//	return x | (z << 2)
//}
//
////Extracts nth interleaved component of a 3-tuple
//func Deinterleave3(v uint32, n uint32) uint32 {
//	v = (v >> n) & 1227133513
//	v = (v | (v >> 2)) & 3272356035
//	v = (v | (v >> 4)) & 251719695
//	v = (v | (v >> 8)) & 4278190335
//	v = (v | (v >> 16)) & 0x3FF
//	return (v << 22) >> 22
//}
//
////Computes next combination in colexicographic order (this is mistakenly called
//// nextPermutation on the bit twiddling hacks page)
//func NextCombination(v uint32) uint32 {
//	var t = v | (v - 1)
//	var c = uint32(not(int32(t)) & -not(int32(t))) - 1
//	return (t + 1) | (c >> (CountTrailingZeros(v) + 1))
//}

#endif //ROBUSTCPP_TWIDDLE_H
