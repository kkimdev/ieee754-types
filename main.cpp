#include <iostream>
#include "ieee754_types.hpp"

int main() {
  IEEE_754::Binary<32> x = 1.0;
  std::cout << x << std::endl;
}
