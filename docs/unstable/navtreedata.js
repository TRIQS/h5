/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "TRIQS/h5", "index.html", [
    [ "Overview", "index.html", "index" ],
    [ "Installation", "installation.html", [
      [ "Dependencies", "installation.html#dependencies", null ],
      [ "Installation steps", "installation.html#install_steps", null ],
      [ "Versions", "installation.html#versions", null ],
      [ "Custom CMake options", "installation.html#cmake_options", null ]
    ] ],
    [ "Integration in C++ projects", "integration.html", [
      [ "CMake", "integration.html#cmake", [
        [ "FetchContent", "integration.html#fetch", null ],
        [ "find_package", "integration.html#find_package", null ],
        [ "add_subdirectory", "integration.html#add_sub", null ]
      ] ]
    ] ],
    [ "Examples", "examples.html", [
      [ "Compiling the examples", "examples.html#compiling", null ],
      [ "Example 1: Write/Read a 2-dimensional array", "ex1.html", null ],
      [ "Example 2: Write/Read a custom C++ type", "ex2.html", null ],
      [ "Example 3: Serialization", "ex3.html", null ]
    ] ],
    [ "API Documentation", "documentation.html", [
      [ "h5 data model", "group__data__model.html", [
        [ "object", "classh5_1_1object.html", null ],
        [ "file", "classh5_1_1file.html", null ],
        [ "group", "classh5_1_1group.html", null ],
        [ "dataset", "group__data__model.html#ga4ebc846edbef1306c242c2d3fad9d786", null ],
        [ "datatype", "group__data__model.html#ga177e858254a3d1ab84a707a225fa4b1f", null ],
        [ "dataspace", "group__data__model.html#ga7702bf25897ae4f687e0f6e2ffd49a4d", null ],
        [ "proplist", "group__data__model.html#gaaeba12898bf632f53beb56f017165142", null ],
        [ "attribute", "group__data__model.html#ga2160229edfcb9c21a9aa5332a1fd155b", null ]
      ] ],
      [ "h5 types", "group__h5__types.html", [
        [ "dcplx_t", "structh5_1_1dcplx__t.html", null ],
        [ "get_hdf5_type", "group__h5__types.html#ga9663a6cefb9589bafc7185f7c45563fc", null ],
        [ "get_name_of_h5_type", "group__h5__types.html#ga9b4181108d25ba467cf17dd150d9fb9a", null ],
        [ "hdf5_type", "group__h5__types.html#ga92724d6ad97c1cbab14a73477693620d", null ],
        [ "hdf5_type_equal", "group__h5__types.html#ga1f1926665da46101206e6ffdddb19600", null ],
        [ "is_complex_v", "group__h5__types.html#ga55e04870d75c17e740a6e757ac25fa7b", null ]
      ] ],
      [ "Read and write from/to HDF5", "group__readwrite.html", [
        [ "Generic read/write", "group__rw__generic.html", null ],
        [ "Array interface", "group__rw__arrayinterface.html", [
          [ "array_view", "structh5_1_1array__interface_1_1array__view.html", null ],
          [ "dataset_info", "structh5_1_1array__interface_1_1dataset__info.html", null ],
          [ "hyperslab", "structh5_1_1array__interface_1_1hyperslab.html", null ]
        ] ],
        [ "Arithmetic scalar types", "group__rw__scalar.html", null ],
        [ "std::string, const char* and char_buf", "group__rw__string.html", [
          [ "char_buf", "structh5_1_1char__buf.html", null ],
          [ "hdf5_format_impl<std::string>", "structh5_1_1hdf5__format__impl_3_01std_1_1string_01_4.html", null ]
        ] ],
        [ "std::array", "group__rw__array.html", null ],
        [ "std::map", "group__rw__map.html", [
          [ "hdf5_format_impl<std::map>", "structh5_1_1hdf5__format__impl_3_01std_1_1map_3_01key_t_00_01value_t_01_4_01_4.html", null ]
        ] ],
        [ "std::optional", "group__rw__optional.html", [
          [ "hdf5_format_impl<std::optional>", "structh5_1_1hdf5__format__impl_3_01std_1_1optional_3_01_t_01_4_01_4.html", null ]
        ] ],
        [ "std::pair", "group__rw__pair.html", [
          [ "hdf5_format_impl<std::pair>", "structh5_1_1hdf5__format__impl_3_01std_1_1pair_3_01_t1_00_01_t2_01_4_01_4.html", null ]
        ] ],
        [ "std::tuple", "group__rw__tuple.html", [
          [ "hdf5_format_impl<std::tuple>", "structh5_1_1hdf5__format__impl_3_01std_1_1tuple_3_01_t_8_8_8_01_4_01_4.html", null ]
        ] ],
        [ "std::variant", "group__rw__variant.html", [
          [ "hdf5_format_impl<std::variant>", "structh5_1_1hdf5__format__impl_3_01std_1_1variant_3_01_t_8_8_8_01_4_01_4.html", null ]
        ] ],
        [ "std::vector", "group__rw__vector.html", [
          [ "hdf5_format_impl<std::vector>", "structh5_1_1hdf5__format__impl_3_01std_1_1vector_3_01_t_01_4_01_4.html", null ],
          [ "hdf5_format_impl<std::vector<std::string>>", "structh5_1_1hdf5__format__impl_3_01std_1_1vector_3_01std_1_1string_01_4_01_4.html", null ]
        ] ],
        [ "Format tags", "group__format.html", [
          [ "hdf5_format_impl<T>", "structh5_1_1hdf5__format__impl.html", null ],
          [ "hdf5_format_impl<bool>", "structh5_1_1hdf5__format__impl_3_01bool_01_4.html", null ],
          [ "hdf5_format_impl<double>", "structh5_1_1hdf5__format__impl_3_01double_01_4.html", null ],
          [ "hdf5_format_impl<float>", "structh5_1_1hdf5__format__impl_3_01float_01_4.html", null ],
          [ "hdf5_format_impl<int>", "structh5_1_1hdf5__format__impl_3_01int_01_4.html", null ],
          [ "hdf5_format_impl<long>", "structh5_1_1hdf5__format__impl_3_01long_01_4.html", null ],
          [ "hdf5_format_impl<long double>", "structh5_1_1hdf5__format__impl_3_01long_01double_01_4.html", null ],
          [ "hdf5_format_impl<long long>", "structh5_1_1hdf5__format__impl_3_01long_01long_01_4.html", null ],
          [ "hdf5_format_impl<std::complex<double>>", "structh5_1_1hdf5__format__impl_3_01std_1_1complex_3_01double_01_4_01_4.html", null ],
          [ "hdf5_format_impl<unsigned int>", "structh5_1_1hdf5__format__impl_3_01unsigned_01int_01_4.html", null ],
          [ "hdf5_format_impl<unsigned long>", "structh5_1_1hdf5__format__impl_3_01unsigned_01long_01_4.html", null ],
          [ "hdf5_format_impl<unsigned long long>", "structh5_1_1hdf5__format__impl_3_01unsigned_01long_01long_01_4.html", null ]
        ] ]
      ] ],
      [ "Serialize/Deserialize", "group__serialize.html", [
        [ "serialize", "group__serialize.html#gad2bcd8f6dac5f1b4d3726c5ba86b0ee6", null ],
        [ "deserialize", "group__serialize.html#gaaaf280034cc7cb7233216238d956a0ad", null ]
      ] ],
      [ "Utilities", "group__utilities.html", [
        [ "hid_t", "group__utilities.html#gac5ce7c09ddf854e73e5bd847e2b067e6", null ],
        [ "hsize_t", "group__utilities.html#ga18c60c084dcd9a6e60485eff3bfdfdbb", null ],
        [ "v_t", "group__utilities.html#ga0133dae33fa46580049cc43ab40f66cc", null ],
        [ "make_runtime_error", "group__utilities.html#ga533849e07bf3ba2cca1f3a831a2df9af", null ],
        [ "Storable", "concepth5_1_1_storable.html", null ]
      ] ],
      [ "File List", "files.html", "files" ]
    ] ],
    [ "Changelog", "changelog.html", null ],
    [ "Issues", "issues.html", null ]
  ] ]
];

var NAVTREEINDEX =
[
"array_8hpp.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';