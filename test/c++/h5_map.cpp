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

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <map>

TEST(H5, Map) {

  // write
  std::map<std::string, int> m                  = {{"a", 1}, {"b", 2}};
  std::map<std::string, std::vector<double>> mv = {{"a", {1.0, 2.0}}, {"b", {2.0, 3.0, 4.0}}};

  {
    h5::file file{"test_map.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "map_int", m);
    h5_write(grp, "map_vec", mv);
  }

  // read
  std::map<std::string, int> mm;
  std::map<std::string, std::vector<double>> mmv;

  {
    h5::file file{"test_map.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "map_int", mm);
    h5_read(grp, "map_vec", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}




class custom_key_class {
  
  int var;
  
  public: 
  custom_key_class(int v = 0): var(v){};

  static std::string hdf5_format() { return "TestkeyClass"; }

  friend void h5_write(h5::group f, std::string const &name, custom_key_class const &c){
    h5_write(f, name,  c.var );
  };

  friend void h5_read(h5::group f, std::string const &name, custom_key_class &c){
    h5_read(f, name,  c.var );
  };

  bool operator < (const custom_key_class& other) const{
    return var < other.var;
  }
  
  bool operator == (const custom_key_class& other) const{
    return var == other.var;
  }
};

TEST(H5, Map_customKey) {
  // Read/Write with custom key.

  // write
  std::map<custom_key_class, std::string> m;
  m.emplace(custom_key_class(1), "hey");
  m.emplace(custom_key_class(10),"you");

  {
    h5::file file{"test_map_2.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "map_customClass", m);
  }

  // read
  std::map<custom_key_class, std::string> mm;
  {
    h5::file file{"test_map_2.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "map_customClass", mm);
  }

  // compare
  EXPECT_EQ(m, mm);
}





template <typename T>
void h5_write_old(h5::group f, std::string const &name, std::map<std::string, T> const &M) {
  auto gr = f.create_group(name);
  write_hdf5_format(gr, M);
  
  for (auto &pvp : M) h5_write(gr, pvp.first, pvp.second);
}


TEST(H5, Map_OldVsNew) {
  // Tests backwards compatibility by writing with the old function and reading
  // using the new function.

  // write
  std::map<std::string, int> m;
  m.emplace("hey",1);
  m.emplace("you",15);

  {
    h5::file file{"test_map_3.h5", 'w'};
    h5::group grp{file};
    h5_write_old(grp, "map_int", m);
  }

  // read
  std::map<std::string, int> mm;
  {
    h5::file file{"test_map_3.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "map_int", mm);
  }

  // compare
  EXPECT_EQ(m, mm);
}

