#include <h5/h5.hpp>
#include <h5/serialization.hpp>
#include <iostream>

int main() {
  // serialize a complex number
  std::complex<double> original { 1.0, 2.0 };
  auto buffer = h5::serialize(original);

  // deserialize the complex number
  auto restored = h5::deserialize<std::complex<double>>(buffer);
  std::cout << original << " == " << restored << "\n";
}
