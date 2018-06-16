[![Build Status](https://travis-ci.org/kkimdev/ieee754-types.svg?branch=master)](https://travis-ci.org/kkimdev/ieee754-types)

# IEEE 754 Types
This is a single header file C++ library that provides IEEE 754 floating point types.
Example usage:

```c++
#include <iostream>
#include "ieee754_types.hpp"

int main() {
  IEEE_754::_2008::Binary<32> x = 1.0;
  IEEE_754::_2008::Binary<64> y = 2.0;

  std::cout << x + y << std::endl;

  // Compile time error if the requested type doesn't exist in the system.
  // IEEE_754::_2008::Binary<16> z;
}
```

The type provided by this library `IEEE_754_2008_Binary<n>` is guaranteed to be IEEE 754 floating point, and it won't compile if the requested type is not available in the system.

For more details, please refer to this blog article [IEEE 754 Floating Point Type in C++ - Overengineering Series #1](https://kkimdev.github.io/posts/2018/06/15/IEEE-754-Floating-Point-Type-in-C++-Overengineering-Series-1.html) .
