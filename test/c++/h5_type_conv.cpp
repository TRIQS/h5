// Copyright (c) 2024 Simons Foundation
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
// Authors: Lukas Weber

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <hdf5.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

// Restore the built-in default conversion callback when leaving a scope.
struct cb_guard {
  ~cb_guard() { h5::set_type_conv_cb({}); }
};

// Read into the destination type and return the list of conversion situations the callback observed.
template <typename Dst>
std::vector<h5::conv_except> read_collecting(h5::group g, std::string const &name, std::vector<Dst> &out) {
  std::vector<h5::conv_except> calls;
  h5::set_type_conv_cb([&calls](h5::conv_except e, h5::datatype, h5::datatype, void *, void *) {
    calls.push_back(e);
    return h5::conv_ret::unhandled;
  });
  h5::h5_read(g, name, out);
  return calls;
}

TEST(H5TypeConv, WideningReportsNotEqual) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  std::vector<int> data(5);
  std::iota(data.begin(), data.end(), 0);
  h5::h5_write(f, "ints", data);

  // int -> long is a lossless widening: reported as not_equal, never narrower
  std::vector<long> out;
  auto calls = read_collecting(f, "ints", out);

  ASSERT_EQ(calls.size(), 1u);
  EXPECT_EQ(calls[0], h5::conv_except::not_equal);
  ASSERT_EQ(out.size(), data.size());
  for (size_t i = 0; i < data.size(); ++i) { EXPECT_EQ(out[i], static_cast<long>(data[i])); }
}

TEST(H5TypeConv, NarrowingReportsNarrower) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  std::vector<int> data(5);
  std::iota(data.begin(), data.end(), 0);
  h5::h5_write(f, "ints", data);

  // int -> short loses precision: reported as narrower (values still fit, so HDF5 itself raises no exception)
  std::vector<short> out;
  auto calls = read_collecting(f, "ints", out);

  ASSERT_EQ(calls.size(), 1u);
  EXPECT_EQ(calls[0], h5::conv_except::narrower);
  ASSERT_EQ(out.size(), data.size());
  for (size_t i = 0; i < data.size(); ++i) { EXPECT_EQ(out[i], static_cast<short>(data[i])); }
}

TEST(H5TypeConv, AbortThrows) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  std::vector<int> data{1, 2, 3};
  h5::h5_write(f, "ints", data);

  h5::set_type_conv_cb([](h5::conv_except, h5::datatype, h5::datatype, void *, void *) { return h5::conv_ret::abort; });

  std::vector<long> out;
  EXPECT_THROW(h5::h5_read(f, "ints", out), std::runtime_error);
}

TEST(H5TypeConv, DefaultCallbackRestoredAndSilentNativeRead) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  std::vector<int> data{1, 2, 3};
  h5::h5_write(f, "ints", data);

  // install then restore the default; an equal-type read must not invoke the callback nor throw
  h5::set_type_conv_cb([](h5::conv_except, h5::datatype, h5::datatype, void *, void *) { return h5::conv_ret::abort; });
  h5::set_type_conv_cb({});

  std::vector<int> out;
  EXPECT_NO_THROW(h5::h5_read(f, "ints", out));
  ASSERT_EQ(out.size(), data.size());
  for (size_t i = 0; i < data.size(); ++i) { EXPECT_EQ(out[i], data[i]); }
}

TEST(H5TypeConv, CrossClassConversionIsAllowed) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  // whole-valued doubles convert cleanly to int (HDF5 raises no per-element exception)
  std::vector<double> data{0.0, 1.0, 2.0, 3.0};
  h5::h5_write(f, "doubles", data);

  std::vector<int> out;
  auto calls = read_collecting(f, "doubles", out);

  // previously this threw on the class-mismatch pre-check; now HDF5 performs the conversion
  ASSERT_EQ(out.size(), data.size());
  for (size_t i = 0; i < data.size(); ++i) { EXPECT_EQ(out[i], static_cast<int>(data[i])); }
  // the (manual) mismatch situation is reported as not_equal (differing classes are not classified as narrower)
  EXPECT_NE(std::find(calls.begin(), calls.end(), h5::conv_except::not_equal), calls.end());
}

TEST(H5TypeConv, NativeExceptionReachesCallback) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  // fractional doubles -> int truncate during conversion: HDF5 delivers a native per-element exception
  // through the trampoline installed on the global transfer property list
  std::vector<double> data{0.5, 1.5, 2.5};
  h5::h5_write(f, "fracs", data);

  std::vector<int> out;
  auto calls = read_collecting(f, "fracs", out);

  bool const saw_native = std::any_of(calls.begin(), calls.end(), [](h5::conv_except e) {
    return e == h5::conv_except::truncate or e == h5::conv_except::precision;
  });
  EXPECT_TRUE(saw_native);
  EXPECT_EQ(out.size(), data.size());
}

TEST(H5TypeConv, UnconvertiblePairThrowsWithTypeNames) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  // write a plain 1D int dataset via the array interface
  std::vector<int> data{1, 2, 3};
  h5::array_interface::array_view wv(h5::hdf5_type<int>(), (void *)data.data(), 1, false);
  wv.slab.count[0]   = static_cast<h5::hsize_t>(data.size());
  wv.parent_shape[0] = static_cast<h5::hsize_t>(data.size());
  h5::array_interface::write(f, "ints", wv, false);

  // there is no conversion path from integer to string, so H5Dread fails and we throw
  h5::set_type_conv_cb([](h5::conv_except, h5::datatype, h5::datatype, void *, void *) { return h5::conv_ret::unhandled; });
  h5::datatype str = H5Tcopy(H5T_C_S1);
  H5Tset_size(str, 8);
  std::vector<char> buf(data.size() * 8, '\0');
  h5::array_interface::array_view rv(str, (void *)buf.data(), 1, false);
  rv.slab.count[0]   = static_cast<h5::hsize_t>(data.size());
  rv.parent_shape[0] = static_cast<h5::hsize_t>(data.size());

  try {
    h5::array_interface::read(f, "ints", rv);
    FAIL() << "expected an exception for the unconvertible int -> string read";
  } catch (std::runtime_error const &e) {
    std::string const msg = e.what();
    EXPECT_NE(msg.find("int"), std::string::npos);
    EXPECT_NE(msg.find("std::string"), std::string::npos);
  }
}

TEST(H5TypeConv, NativeExceptionForwardsBuffers) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  // fractional doubles -> int truncate: HDF5 delivers a native per-element exception, so the callback must
  // receive non-null pointers to the source/destination element being converted
  std::vector<double> data{0.5, 1.5, 2.5};
  h5::h5_write(f, "fracs", data);

  bool saw_native_buffers = false;
  h5::set_type_conv_cb([&saw_native_buffers](h5::conv_except e, h5::datatype, h5::datatype, void *src_buf, void *dst_buf) {
    if (e == h5::conv_except::truncate or e == h5::conv_except::precision) {
      EXPECT_NE(src_buf, nullptr);
      EXPECT_NE(dst_buf, nullptr);
      saw_native_buffers = true;
    }
    return h5::conv_ret::unhandled;
  });

  std::vector<int> out;
  h5::h5_read(f, "fracs", out);
  EXPECT_TRUE(saw_native_buffers);
}

TEST(H5TypeConv, CustomExceptionForwardsNullBuffers) {
  cb_guard guard;
  h5::file f("type_conv.h5", 'w');

  // int -> long widening is an h5-specific (custom) not_equal case, raised manually with no per-element buffer,
  // so the callback must receive null src/dst pointers
  std::vector<int> data(5);
  std::iota(data.begin(), data.end(), 0);
  h5::h5_write(f, "ints", data);

  bool saw_custom = false;
  h5::set_type_conv_cb([&saw_custom](h5::conv_except e, h5::datatype, h5::datatype, void *src_buf, void *dst_buf) {
    if (e == h5::conv_except::not_equal or e == h5::conv_except::narrower) {
      EXPECT_EQ(src_buf, nullptr);
      EXPECT_EQ(dst_buf, nullptr);
      saw_custom = true;
    }
    return h5::conv_ret::unhandled;
  });

  std::vector<long> out;
  h5::h5_read(f, "ints", out);
  EXPECT_TRUE(saw_custom);
}
