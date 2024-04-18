@page ex3 Example 3: Serialization

[TOC]

This example shows how to use **h5** to serialize/deserialize a `std::complex<double>` object to/from a byte buffer.

```cpp
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
```

Output:

```
(1,2) == (1,2)
```
