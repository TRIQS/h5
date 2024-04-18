// Copyright (c) 2020 Simons Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: Miles Stoudenmire, Nils Wentzell

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <string>
#include <vector>

TEST(H5, StringEncodings) {
  // write/read ASCII and UTF8 strings
  std::string ascii_str = "Hello World!";
  std::string utf8_str  = "Price: 10 €";

  {
    h5::file file("test_string.h5", 'w');

    // write ASCII (internally uses UTF8 encoding)
    h5::write(file, "ASCII", ascii_str);
    h5::write_attribute(file, "ASCII_Attr", ascii_str);

    // write UTF8
    h5::write(file, "UTF8", utf8_str);
    h5::write_attribute(file, "UTF8_Attr", utf8_str);
  }

  {
    h5::file file("ascii.ref.h5", 'r');

    // read ASCII stored with H5T_CSET_ASCII
    std::string ascii_str_in = "";
    h5::read(file, "ASCII", ascii_str_in);

    EXPECT_EQ(ascii_str, ascii_str_in);
    h5::read_attribute(file, "ASCII_Attr", ascii_str_in);
    EXPECT_EQ(ascii_str, ascii_str_in);
  }

  {
    h5::file file("test_string.h5", 'r');

    // read ASCII stored with UTF8 encoding
    std::string ascii_str_in = "";
    h5::read(file, "ASCII", ascii_str_in);

    EXPECT_EQ(ascii_str, ascii_str_in);
    h5::read_attribute(file, "ASCII_Attr", ascii_str_in);
    EXPECT_EQ(ascii_str, ascii_str_in);

    // read UTF8
    std::string utf8_str_in = "";
    h5::read(file, "UTF8", utf8_str_in);

    EXPECT_EQ(utf8_str, utf8_str_in);
    h5::read_attribute(file, "UTF8_Attr", utf8_str_in);
    EXPECT_EQ(utf8_str, utf8_str_in);
  }
}

TEST(H5, CharBuff) {
  // write/read a char buffer
  std::vector<std::string> vec{"Hello", "World!"};
  std::vector<std::vector<std::string>> vec2{{"Hello", "World!", ""}, {"Hello", "again", "World!"}};

  // create char buffers
  auto cb = h5::to_char_buf(vec);
  EXPECT_EQ(cb.lengths[0], 2); // 2 strings in 1-dimensional vector
  EXPECT_EQ(cb.lengths[1], 7); // max. length of the strings + 1
  auto cb2 = h5::to_char_buf(vec2);
  EXPECT_EQ(cb2.lengths[0], 2); // 2 vector of strings in first dimension
  EXPECT_EQ(cb2.lengths[1], 3); // 3 strings in second dimension
  EXPECT_EQ(cb2.lengths[2], 7); // max. length of the strings + 1

  {
    h5::file file("char_buf.h5", 'w');
    h5::write(file, "cb", cb);
    h5::write(file, "cb2", cb2);
  }

  {
    h5::file file("char_buf.h5", 'r');

    h5::char_buf cb_in, cb2_in;
    h5::read(file, "cb", cb_in);
    h5::read(file, "cb2", cb2_in);

    // convert char buffers to vector (of vectors) of strings
    std::vector<std::string> vec_in;
    h5::from_char_buf(cb_in, vec_in);
    EXPECT_EQ(vec, vec_in);

    std::vector<std::vector<std::string>> vec2_in;
    h5::from_char_buf(cb2_in, vec2_in);
    EXPECT_EQ(vec2, vec2_in);
  }
}
