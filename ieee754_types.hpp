#pragma once

// Based on IEEE Standard 754-2008.

#include <climits>
#include <limits>

namespace detail {

constexpr unsigned bits(unsigned x) { return x == 0 ? 0 : 1 + bits(x >> 1); }

constexpr float constexpr_log2(float x, int recursion_limit = 256) {
  if (recursion_limit == 0) return 0;
  if (0 < x && x < 1) return constexpr_log2(x * 2, --recursion_limit) - 1;
  if (x == 1) return 0;
  if (1 < x && x < 2) return constexpr_log2(x * x, --recursion_limit) / 2;
  if (2 <= x) return constexpr_log2(x / 2, --recursion_limit) + 1;
  return -1;
}

constexpr int ieee_binary_format_standard_exponent_bits(int total_bits) {
  // TODO: What do we do for wrong total_bits? i.e., not(16 or multiple of 32).
  return total_bits == 16 ? 5 :      //
             total_bits == 32 ? 8 :  //
                 int(4 * constexpr_log2(total_bits) + 0.5f) - 13;
}

static_assert(ieee_binary_format_standard_exponent_bits(16) == 5, "");
static_assert(ieee_binary_format_standard_exponent_bits(32) == 8, "");
static_assert(ieee_binary_format_standard_exponent_bits(64) == 11, "");
static_assert(ieee_binary_format_standard_exponent_bits(128) == 15, "");
static_assert(ieee_binary_format_standard_exponent_bits(256) == 19, "");

constexpr int ieee_binary_format_standard_mantissa_bits(int total_bits) {
  return total_bits - ieee_binary_format_standard_exponent_bits(total_bits) - 1;
}

static_assert(ieee_binary_format_standard_mantissa_bits(16) == 10, "");
static_assert(ieee_binary_format_standard_mantissa_bits(32) == 23, "");
static_assert(ieee_binary_format_standard_mantissa_bits(64) == 52, "");
static_assert(ieee_binary_format_standard_mantissa_bits(128) == 112, "");
static_assert(ieee_binary_format_standard_mantissa_bits(256) == 236, "");

template <typename T>
constexpr int get_total_bits() {
  return sizeof(T) * CHAR_BIT;
}

template <typename T>
constexpr int get_exponent_bits() {
  return bits(::std::numeric_limits<T>::max_exponent -
              ::std::numeric_limits<T>::min_exponent);
}

template <typename T>
constexpr int get_mantissa_bits() {
  return ::std::numeric_limits<T>::digits - 1;
}

template <typename T, int total_bits, int exponent_bits, int mantissa_bits>
constexpr bool is_ieee754_binary_format() {
  return ::std::is_floating_point<T>() &&  //
         ::std::numeric_limits<T>::is_iec559 &&
         ::std::numeric_limits<T>::radix == 2 &&
         total_bits == get_total_bits<T>() &&
         exponent_bits == get_exponent_bits<T>() &&
         mantissa_bits == get_mantissa_bits<T>();
}

template <int total_bits, int exponent_bits, int mantissa_bits>
class IEEE754BinaryNotFound {
  // `false && total_bits` is a hack to postpone static_assert to template
  // instantiation.
  static_assert(false && total_bits,
                "No corresponding IEEE754 binary exchange format found.");
};

template <
    int total_bits,
    int exponent_bits = ieee_binary_format_standard_exponent_bits(total_bits),
    int mantissa_bits = ieee_binary_format_standard_mantissa_bits(total_bits)>
using IEEE754Binary =  //
    ::std::conditional_t<
        is_ieee754_binary_format<float, total_bits, exponent_bits,
                                 mantissa_bits>(),
        float,  //
        ::std::conditional_t<
            is_ieee754_binary_format<double, total_bits, exponent_bits,
                                     mantissa_bits>(),
            double,  //
            ::std::conditional_t<
                is_ieee754_binary_format<long double, total_bits, exponent_bits,
                                         mantissa_bits>(),
                long double,  //
                IEEE754BinaryNotFound<total_bits, exponent_bits,
                                      mantissa_bits>>>>;
}

template <int total_bits>
using IEEE754Binary = detail::IEEE754Binary<total_bits>;

using IEEE754Binary16 = detail::IEEE754Binary<16>;
using IEEE754Binary32 = detail::IEEE754Binary<32>;
using IEEE754Binary64 = detail::IEEE754Binary<64>;
using IEEE754Binary128 = detail::IEEE754Binary<128>;
using IEEE754Binary80X87 = detail::IEEE754Binary<128, 15, 63>;

// TODO: How to perform the following assertions only on instantiation?
// static_assert(sizeof(IEEE754Binary16) == 16 / CHAR_BIT, "");
// static_assert(sizeof(IEEE754Binary32) == 32 / CHAR_BIT, "");
// static_assert(sizeof(IEEE754Binary64) == 64 / CHAR_BIT, "");
// static_assert(sizeof(IEEE754Binary128) == 128 / CHAR_BIT, "");
