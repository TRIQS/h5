#include <h5/h5.hpp>

int main() {
  // create file in write mode
  h5::file file("file.h5", 'w');

  // write Euler number
  h5::write(file, "e", 2.7182);

  // create root group
  h5::group root(file);

  // create a subgroup of root
  auto subgroup = root.create_group("mydata");

  // write pi to subgroup
  h5::write(subgroup, "pi", 3.1415);
}
