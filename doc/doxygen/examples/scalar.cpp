#include <h5/h5.hpp>
#include <complex>
#include <iostream>

int main() {
  // create file in read/write mode
  h5::file file("complex.h5", 'w');

  // write complex number
  std::complex<double> z { 1.0, 2.0 };
  h5::write(file, "z", z);

  // read into complex number
  std::complex<double> z_rd;
  h5::read(file, "z", z_rd);

  // output complex number
  std::cout << z_rd << std::endl;
}
