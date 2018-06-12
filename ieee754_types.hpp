// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef IEEE754_TYPES_HPP_
#define IEEE754_TYPES_HPP_

// Based on IEEE 754-2008

#if __cplusplus < 201703L
#error This file requires C++17
#endif

#include <climits>
#include <limits>
#include <type_traits>

namespace IEEE_754 {
namespace detail {

template <typename T>
constexpr int get_storage_bits() {
  return sizeof(T) * CHAR_BIT;
}

template <typename T>
constexpr int get_exponent_bits() {
  int exponent_range = ::std::numeric_limits<T>::max_exponent -
                       ::std::numeric_limits<T>::min_exponent;
  int bits = 0;
  while ((exponent_range >> bits) > 0) ++bits;
  return bits;
}

template <typename T>
constexpr int get_mantissa_bits() {
  return ::std::numeric_limits<T>::digits - 1;
}

template <int storage_bits>
constexpr int standard_binary_interchange_format_exponent_bits() {
  constexpr bool is_valid_storage_bits =
      storage_bits == 16 ||   //
      storage_bits == 32 ||   //
      storage_bits == 64 ||   //
      storage_bits == 128 ||  //
      (storage_bits > 128 && storage_bits % 32 == 0);
  static_assert(
      is_valid_storage_bits,
      "IEEE 754-2008 standard binary interchange formats are only defined for "
      "the following storage width in bits: 16, 32, 64, 128, and any multiple "
      "of 32 of at least 128.");
  static_assert(!(is_valid_storage_bits && storage_bits > 128),
                "Not Implemented for storage bits larger than 128.");

  if (storage_bits == 16) return 5;
  if (storage_bits == 32) return 8;
  if (storage_bits == 64) return 11;
  if (storage_bits == 128) return 15;

  throw;
}

template <int storage_bits>
constexpr int standard_binary_interchange_format_mantissa_bits() {
  return storage_bits -
         standard_binary_interchange_format_exponent_bits<storage_bits>() - 1;
}

static_assert(standard_binary_interchange_format_exponent_bits<16>() == 5, "");
static_assert(standard_binary_interchange_format_mantissa_bits<16>() == 10, "");
static_assert(standard_binary_interchange_format_exponent_bits<32>() == 8, "");
static_assert(standard_binary_interchange_format_mantissa_bits<32>() == 23, "");
static_assert(standard_binary_interchange_format_exponent_bits<64>() == 11, "");
static_assert(standard_binary_interchange_format_mantissa_bits<64>() == 52, "");
static_assert(standard_binary_interchange_format_exponent_bits<128>() == 15,
              "");
static_assert(standard_binary_interchange_format_mantissa_bits<128>() == 112,
              "");

template <int storage_bits, int exponent_bits, int mantissa_bits>
struct Is_Ieee754_2008_Binary_Interchange_Format {
  template <typename T>
  static constexpr bool value() {
    return ::std::is_floating_point<T>() &&            //
           ::std::numeric_limits<T>::is_iec559 &&      //
           ::std::numeric_limits<T>::radix == 2 &&     //
           get_storage_bits<T>() == storage_bits &&    //
           get_exponent_bits<T>() == exponent_bits &&  //
           get_mantissa_bits<T>() == mantissa_bits;
  }
};

///////////////////////////////
// TODO: WIP

template <typename F, typename T, typename... Ts>
constexpr auto find_type() {
  if constexpr (F::template value<T>()) {
    return T();
  } else if constexpr (sizeof...(Ts) == 0) {
    return void();
  } else {
    return find_type<F, Ts...>();
  }
}

///////////////////////////////

// Recursion termination.  Type not found.
template <typename F, typename... Ts>
struct FindType {
  using type = void;
};

// Recursion
template <typename F, typename T, typename... Ts>
struct FindType<F, T, Ts...> {
  // Set `type = T` if T satisfies the condition, F.  Otherwise, keep
  // searching in the remaining types, Ts... .
  using type = ::std::conditional_t<  //
      F::template value<T>(), T, typename FindType<F, Ts...>::type>;
};

template <int storage_bits,
          int exponent_bits =
              standard_binary_interchange_format_exponent_bits<storage_bits>(),
          int mantissa_bits =
              standard_binary_interchange_format_mantissa_bits<storage_bits>()>
// using BinaryFloatOrVoid = typename FindType<                  //
//     Is_Ieee754_2008_Binary_Interchange_Format<storage_bits,   //
//                                               exponent_bits,  //
//                                               mantissa_bits>,
//     float, double, long double>::type;
using BinaryFloatOrVoid =
    decltype(find_type<                                                //
             Is_Ieee754_2008_Binary_Interchange_Format<storage_bits,   //
                                                       exponent_bits,  //
                                                       mantissa_bits>,
             float, double, long double>());

template <typename T>
struct AssertTypeFound {
  static_assert(
      !::std::is_same_v<T, void>,
      "No corresponding IEEE 754-2008 binary interchange format found.");
  using type = T;
};

}  // namespace detail

namespace _2008 {
template <int storage_bits>
using Binary = typename detail::AssertTypeFound<
    detail::BinaryFloatOrVoid<storage_bits>>::type;
}  // namespace _2008

namespace detail {

template <int storage_bits, int exponent_bits, int mantissa_bits,
          typename = void>
struct AssertBitsIfTypeExists {
  using T = ::IEEE_754::_2008::Binary<storage_bits>;
  static_assert(get_storage_bits<T>() == storage_bits, "");
  static_assert(get_exponent_bits<T>() == exponent_bits, "");
  static_assert(get_mantissa_bits<T>() == mantissa_bits, "");
};

template <int storage_bits, int exponent_bits, int mantissa_bits>
struct AssertBitsIfTypeExists<storage_bits, exponent_bits, mantissa_bits,
                              ::std::enable_if_t<::std::is_same_v<
                                  BinaryFloatOrVoid<storage_bits>, void>>> {};

struct Asserts {
  AssertBitsIfTypeExists<16, 5, 10> test16;
  AssertBitsIfTypeExists<32, 8, 23> test32;
  AssertBitsIfTypeExists<64, 11, 52> test64;
  AssertBitsIfTypeExists<128, 15, 112> test128;
};

}  // namespace detail

}  // namespace IEEE_754

#endif  // IEEE754_TYPES_HPP_
