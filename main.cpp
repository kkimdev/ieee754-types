// Any copyright is dedicated to the Public Domain.
// http://creativecommons.org/publicdomain/zero/1.0/

#include <iostream>
#include "ieee754_types.hpp"

int main() {
  IEEE_754::_2008::Binary<32> x = 1.0;
  IEEE_754::_2008::Binary<64> y = 2.0;

  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
