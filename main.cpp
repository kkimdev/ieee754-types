#ifndef IEEE754_TYPES_HPP_
#define IEEE754_TYPES_HPP_

#include <climits>
#include <stdexcept>

namespace IEEE_754_2008 {
namespace detail {

template <typename T>
constexpr ::std::size_t get_storage_bits() {
  return sizeof(T) * CHAR_BIT;
}

constexpr ::std::size_t num_bits(::std::size_t x) {
  return x == 0 ? 0 : 1 + num_bits(x >> 1);
}

template <typename T>
constexpr ::std::size_t get_exponent_bits() {
  return num_bits(::std::numeric_limits<T>::max_exponent -
                  ::std::numeric_limits<T>::min_exponent);
}

template <typename T>
constexpr ::std::size_t get_mantissa_bits() {
  return ::std::numeric_limits<T>::digits - 1;
}

constexpr ::std::size_t standard_binary_interchange_format_exponent_bits(
    ::std::size_t storage_bits) {
  if (storage_bits == 16) return 5;
  if (storage_bits == 32) return 8;
  if (storage_bits == 64) return 11;
  if (storage_bits == 128) return 15;
  if (storage_bits > 128 && storage_bits % 32 == 0)
    throw ::std::logic_error("Not Implemented.");

  throw ::std::invalid_argument("");
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

template <typename T, int storage_bits, int exponent_bits, int mantissa_bits>
constexpr bool is_ieee_754_2008_binary_interchange_format() {
  return ::std::is_floating_point<T>() &&  //
         ::std::numeric_limits<T>::is_iec559 &&
         ::std::numeric_limits<T>::radix == 2 &&
         storage_bits == get_storage_bits<T>() &&
         exponent_bits == get_exponent_bits<T>() &&
         mantissa_bits == get_mantissa_bits<T>();
}

template <int storage_bits, int exponent_bits, int mantissa_bits,
          typename... Ts>
struct Foo;

template <int storage_bits, int exponent_bits, int mantissa_bits, typename T,
          typename... Ts>
struct Foo<storage_bits, exponent_bits, mantissa_bits, T, Ts...> {
  using type = ::std::conditional_t<
      is_ieee_754_2008_binary_interchange_format<T, storage_bits, exponent_bits,
                                                 mantissa_bits>(),
      T, typename Foo<storage_bits, exponent_bits, mantissa_bits, Ts...>::type>;
};

template <int storage_bits, int exponent_bits, int mantissa_bits>
struct Foo<storage_bits, exponent_bits, mantissa_bits> {
  template <bool t>
  class BinaryFormatNotFound {
    // `false && storage_bits` is a hack to postpone static_assert to template
    // instantiation.
    static_assert(
        false && t,
        "No corresponding IEEE 754-2008 binary interchange format found.");
  };
  using type = BinaryFormatNotFound<true>;
};

template <int storage_bits,
          int exponent_bits =
              standard_binary_interchange_format_exponent_bits(storage_bits),
          int mantissa_bits =
              standard_binary_interchange_format_mantissa_bits(storage_bits)>
using Binary = typename Foo<storage_bits, exponent_bits, mantissa_bits, float,
                            double, long double>::type;

/////////////////////
}  // namespace detail

template <int storage_bits>
using Binary = detail::Binary<storage_bits>;

}  // namespace IEEE_754_2008

#include <iostream>
int main() {
  IEEE_754_2008::Binary<32> f;
  std::cout << f;
  return 0;
}

#endif  // IEEE754_TYPES_HPP_