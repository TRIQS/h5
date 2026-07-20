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
var menudata={children:[
{text:"Main Page",url:"index.html"},
{text:"Installation",url:"installation.html",children:[
{text:"Dependencies",url:"installation.html#dependencies"},
{text:"Installation steps",url:"installation.html#install_steps"},
{text:"Versions",url:"installation.html#versions"},
{text:"Custom CMake options",url:"installation.html#cmake_options"}]},
{text:"Integration in C++ projects",url:"integration.html",children:[
{text:"CMake",url:"integration.html#cmake",children:[
{text:"FetchContent",url:"integration.html#fetch"},
{text:"find_package",url:"integration.html#find_package"},
{text:"add_subdirectory",url:"integration.html#add_sub"}]}]},
{text:"Examples",url:"examples.html",children:[
{text:"Compiling the examples",url:"examples.html#compiling"},
{text:"Example 1: Write/Read a 2-dimensional array",url:"ex1.html"},
{text:"Example 2: Write/Read a custom C++ type",url:"ex2.html"},
{text:"Example 3: Serialization",url:"ex3.html"}]},
{text:"API Documentation",url:"documentation.html",children:[
{text:"h5 data model",url:"group__data__model.html",children:[
{text:"object",url:"classh5_1_1object.html"},
{text:"file",url:"classh5_1_1file.html"},
{text:"group",url:"classh5_1_1group.html"},
{text:"dataset",url:"group__data__model.html#ga4ebc846edbef1306c242c2d3fad9d786"},
{text:"datatype",url:"group__data__model.html#ga177e858254a3d1ab84a707a225fa4b1f"},
{text:"dataspace",url:"group__data__model.html#ga7702bf25897ae4f687e0f6e2ffd49a4d"},
{text:"proplist",url:"group__data__model.html#gaaeba12898bf632f53beb56f017165142"},
{text:"attribute",url:"group__data__model.html#ga2160229edfcb9c21a9aa5332a1fd155b"}]},
{text:"h5 types",url:"group__h5__types.html",children:[
{text:"dcplx_t",url:"structh5_1_1dcplx__t.html"},
{text:"get_hdf5_type",url:"group__h5__types.html#ga9663a6cefb9589bafc7185f7c45563fc"},
{text:"get_name_of_h5_type",url:"group__h5__types.html#ga9b4181108d25ba467cf17dd150d9fb9a"},
{text:"hdf5_type",url:"group__h5__types.html#ga92724d6ad97c1cbab14a73477693620d"},
{text:"hdf5_type_equal",url:"group__h5__types.html#ga1f1926665da46101206e6ffdddb19600"},
{text:"is_complex_v",url:"group__h5__types.html#ga55e04870d75c17e740a6e757ac25fa7b"}]},
{text:"Read and write from/to HDF5",url:"group__readwrite.html",children:[
{text:"Arithmetic scalar types",url:"group__rw__scalar.html"},
{text:"Array interface",url:"group__rw__arrayinterface.html",children:[
{text:"array_view",url:"structh5_1_1array__interface_1_1array__view.html"},
{text:"dataset_info",url:"structh5_1_1array__interface_1_1dataset__info.html"},
{text:"hyperslab",url:"structh5_1_1array__interface_1_1hyperslab.html"}]},
{text:"Format tags",url:"group__format.html",children:[
{text:"hdf5_format_impl<T>",url:"structh5_1_1hdf5__format__impl.html"},
{text:"hdf5_format_impl<bool>",url:"structh5_1_1hdf5__format__impl_3_01bool_01_4.html"},
{text:"hdf5_format_impl<double>",url:"structh5_1_1hdf5__format__impl_3_01double_01_4.html"},
{text:"hdf5_format_impl<float>",url:"structh5_1_1hdf5__format__impl_3_01float_01_4.html"},
{text:"hdf5_format_impl<int>",url:"structh5_1_1hdf5__format__impl_3_01int_01_4.html"},
{text:"hdf5_format_impl<long>",url:"structh5_1_1hdf5__format__impl_3_01long_01_4.html"},
{text:"hdf5_format_impl<long double>",url:"structh5_1_1hdf5__format__impl_3_01long_01double_01_4.html"},
{text:"hdf5_format_impl<long long>",url:"structh5_1_1hdf5__format__impl_3_01long_01long_01_4.html"},
{text:"hdf5_format_impl<std::complex<double>>",url:"structh5_1_1hdf5__format__impl_3_01std_1_1complex_3_01double_01_4_01_4.html"},
{text:"hdf5_format_impl<unsigned int>",url:"structh5_1_1hdf5__format__impl_3_01unsigned_01int_01_4.html"},
{text:"hdf5_format_impl<unsigned long>",url:"structh5_1_1hdf5__format__impl_3_01unsigned_01long_01_4.html"},
{text:"hdf5_format_impl<unsigned long long>",url:"structh5_1_1hdf5__format__impl_3_01unsigned_01long_01long_01_4.html"}]},
{text:"Generic read/write",url:"group__rw__generic.html"},
{text:"std::array",url:"group__rw__array.html"},
{text:"std::map",url:"group__rw__map.html",children:[
{text:"hdf5_format_impl< std::map >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1map_3_01_key_00_01_t_00_01_compare_01_4_01_4.html"}]},
{text:"std::optional",url:"group__rw__optional.html",children:[
{text:"hdf5_format_impl< std::optional >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1optional_3_01_t_01_4_01_4.html"}]},
{text:"std::pair",url:"group__rw__pair.html",children:[
{text:"hdf5_format_impl< std::pair >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1pair_3_01_t1_00_01_t2_01_4_01_4.html"}]},
{text:"std::string, const char* and char_buf",url:"group__rw__string.html",children:[
{text:"char_buf",url:"structh5_1_1char__buf.html"},
{text:"hdf5_format_impl< std::string >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1string_01_4.html"}]},
{text:"std::tuple",url:"group__rw__tuple.html",children:[
{text:"hdf5_format_impl< std::tuple >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1tuple_3_01_t_8_8_8_01_4_01_4.html"}]},
{text:"std::variant",url:"group__rw__variant.html",children:[
{text:"hdf5_format_impl< std::variant >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1variant_3_01_t_8_8_8_01_4_01_4.html"}]},
{text:"std::vector",url:"group__rw__vector.html",children:[
{text:"hdf5_format_impl< std::vector >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1vector_3_01_t_01_4_01_4.html"},
{text:"hdf5_format_impl< std::vector< std::string > >",url:"structh5_1_1hdf5__format__impl_3_01std_1_1vector_3_01std_1_1string_01_4_01_4.html"}]}]},
{text:"Serialize/Deserialize",url:"group__serialize.html",children:[
{text:"serialize",url:"group__serialize.html#gad2bcd8f6dac5f1b4d3726c5ba86b0ee6"},
{text:"deserialize",url:"group__serialize.html#gaaaf280034cc7cb7233216238d956a0ad"}]},
{text:"Utilities",url:"group__utilities.html",children:[
{text:"Storable",url:"concepth5_1_1_storable.html"}]},
{text:"Python interface utilities",url:"group__python-utils.html"},
{text:"File List",url:"files.html"}]},
{text:"Changelog",url:"../ChangeLog.html"},
{text:"Issues",url:"../issues.html"}]}
