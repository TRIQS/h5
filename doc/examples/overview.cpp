#include <h5/h5.hpp>
#include <string>
#include <vector>

int main(){
  {
    // Open file in write mode
    h5::file file("vec.h5", 'w');

    std::vector<std::string> vecs = {"a", "b"};
    std::vector<double> vecd      = {1.0, 2.0};

    h5::write(file, "vecs", vecs);
    h5::write(file, "vecd", vecd);

  } // Close file

  {
    // Open file in read mode
    h5::file file("vec.h5", 'r');

    std::vector<std::string> vecs;
    std::vector<double> vecd;

    h5::read(file, "vecs", vecs);
    h5::read(file, "vecd", vecd);

  } // Close file
}
