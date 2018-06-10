#ifndef IEEE754_TYPES_HPP_
#define IEEE754_TYPES_HPP_

// Based on IEEE 754-2008

#include <climits>
#include <stdexcept>

namespace IEEE_754 {
namespace detail {

template <typename T>
constexpr int get_storage_bits() {
  return sizeof(T) * CHAR_BIT;
}

constexpr int num_bits(int x) {  //
  return x == 0 ? 0 : 1 + num_bits(x >> 1);
}

template <typename T>
constexpr int get_exponent_bits() {
  return num_bits(::std::numeric_limits<T>::max_exponent -
                  ::std::numeric_limits<T>::min_exponent);
}

template <typename T>
constexpr int get_mantissa_bits() {
  return ::std::numeric_limits<T>::digits - 1;
}

constexpr int standard_binary_interchange_format_exponent_bits(
    int storage_bits) {
  if (storage_bits == 16) return 5;
  if (storage_bits == 32) return 8;
  if (storage_bits == 64) return 11;
  if (storage_bits == 128) return 15;
  if (storage_bits > 128 && storage_bits % 32 == 0)
    throw ::std::logic_error(
        "Not Implemented for storage bits larger than 128.");

  throw ::std::invalid_argument(
      "IEEE 754-2008 standard binary interchange formats are only defined for "
      "the following storage width in bits: 16, 32, 64, 128, and any multiple "
      "of 32 of at least 128.");
}

constexpr int standard_binary_interchange_format_mantissa_bits(
    int storage_bits) {
  return storage_bits -
         standard_binary_interchange_format_exponent_bits(storage_bits) - 1;
}

static_assert(standard_binary_interchange_format_exponent_bits(16) == 5, "");
static_assert(standard_binary_interchange_format_mantissa_bits(16) == 10, "");
static_assert(standard_binary_interchange_format_exponent_bits(32) == 8, "");
static_assert(standard_binary_interchange_format_mantissa_bits(32) == 23, "");
static_assert(standard_binary_interchange_format_exponent_bits(64) == 11, "");
static_assert(standard_binary_interchange_format_mantissa_bits(64) == 52, "");
static_assert(standard_binary_interchange_format_exponent_bits(128) == 15, "");
static_assert(standard_binary_interchange_format_mantissa_bits(128) == 112, "");

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

template <typename F, typename... Ts>
struct FindType;

// Recursion
template <typename F, typename T, typename... Ts>
struct FindType<F, T, Ts...> {
  // Set `type = T` if T satisfies the condition, F.  Otherwise, keep searching
  // in the remaining types, Ts... .
  using type = ::std::conditional_t<  //
      F::template value<T>(), T, typename FindType<F, Ts...>::type>;
};

// Recursion termination.  Type not found.
template <typename F>
struct FindType<F> {
  template <typename T>
  struct TypeNotFound {
    static_assert(
        !::std::is_same_v<T, T>,
        "No corresponding IEEE 754-2008 binary interchange format found.");
  };
  using type = TypeNotFound<void>;
};

template <int storage_bits,
          int exponent_bits =
              standard_binary_interchange_format_exponent_bits(storage_bits),
          int mantissa_bits =
              standard_binary_interchange_format_mantissa_bits(storage_bits)>
using Binary =
    typename FindType<Is_Ieee754_2008_Binary_Interchange_Format<
                          storage_bits, exponent_bits, mantissa_bits>,
                      float, double, long double>::type;

}  // namespace detail

namespace _2008 {
template <int storage_bits>
using Binary = detail::Binary<storage_bits>;
}  // namespace _2008

}  // namespace IEEE_754

#endif  // IEEE754_TYPES_HPP_
