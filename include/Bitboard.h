// #pragma once

// #include <cstdint>

// #include "Globals.h"

// // this struct is NOT completed!
// // and I probably won't bother using it

// struct Bitboard {
//     Bitboard(uint64_t value = 0ULL) : m_value(value) {}
//     Bitboard(const Bitboard& bb) : m_value(bb.m_value) {}

//     inline bool isBitSet(int bit) { return m_value & (1ULL << bit) != 0ULL; }
    
//     inline void setBit(int bit) { m_value |= 1ULL << bit; }
//     inline void clearBit(int bit) { m_value &= ~(1ULL << bit); }

//     inline int count() { return BitOps::CountBits(m_value); }
//     inline int firstBitIndex() { return BitOps::FirstBitIndex(m_value); }

//     inline int popFirstBit() {
//         int lsbi = firstBitIndex();
//         m_value &= m_value - 1;
//         return lsbi;
//     }

//     operator uint64_t() const { return m_value; }
//     operator bool() const { return m_value != 0ULL; }

//     friend inline Bitboard operator&(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator|(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator^(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator<<(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator>>(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator*(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator/(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator+(const Bitboard& a, const Bitboard& b);
//     friend inline Bitboard operator-(const Bitboard& a, const Bitboard& b);

//     friend inline Bitboard operator&(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator|(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator^(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator<<(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator>>(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator*(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator/(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator+(const Bitboard& a, const U64 b);
//     friend inline Bitboard operator-(const Bitboard& a, const U64 b);

//     Bitboard& operator &=(const Bitboard& other) {
//         m_value &= other.m_value;
//         return *this;
//     }

//     Bitboard& operator |=(const Bitboard& other) {
//         m_value |= other.m_value;
//         return *this;
//     }

//     Bitboard& operator ^=(const Bitboard& other) {
//         m_value ^= other.m_value;
//         return *this;
//     }

//     Bitboard& operator <<=(const Bitboard& other) {
//         m_value <<= other.m_value;
//         return *this;
//     }

//     Bitboard& operator >>=(const Bitboard& other) {
//         m_value >>= other.m_value;
//         return *this;
//     }

//     Bitboard& operator &=(const U64 x) {
//         m_value &= x;
//         return *this;
//     }

//     Bitboard& operator |=(const U64 x) {
//         m_value |= x;
//         return *this;
//     }
    
//     Bitboard& operator ^=(const U64& x) {
//         m_value ^= x;
//         return *this;
//     }
    
//     Bitboard& operator <<=(const U64 x) {
//         m_value <<= x;
//         return *this;
//     }
    
//     Bitboard& operator >>=(const U64 x) {
//         m_value >>= x;
//         return *this;
//     }

//     private:

//     uint64_t m_value;
// };

// inline Bitboard operator&(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value & b.m_value);
// }

// inline Bitboard operator|(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value | b.m_value);
// }

// inline Bitboard operator^(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value ^ b.m_value);
// }

// inline Bitboard operator<<(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value << b.m_value);
// }

// inline Bitboard operator>>(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value >> b.m_value);
// }

// inline Bitboard operator*(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value * b.m_value);
// }

// inline Bitboard operator/(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value / b.m_value);
// }

// inline Bitboard operator+(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value + b.m_value);
// }

// inline Bitboard operator-(const Bitboard& a, const Bitboard& b) {
//     return Bitboard(a.m_value - b.m_value);
// }

// inline Bitboard operator&(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value & b);
// }

// inline Bitboard operator|(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value | b);
// }

// inline Bitboard operator^(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value ^ b);
// }

// inline Bitboard operator<<(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value << b);
// }

// inline Bitboard operator>>(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value >> b);
// }

// inline Bitboard operator*(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value * b);
// }

// inline Bitboard operator/(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value / b);
// }

// inline Bitboard operator+(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value + b);
// }

// inline Bitboard operator-(const Bitboard& a, const U64 b) {
//     return Bitboard(a.m_value - b);
// }