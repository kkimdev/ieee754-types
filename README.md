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

  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
```
