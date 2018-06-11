#include <iostream>
#include "ieee754_types.hpp"

int main() {
  IEEE_754::_2008::Binary<64> x = 1.0;
  std::cout << x << std::endl;
}
