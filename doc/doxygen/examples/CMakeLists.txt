# List of all examples
file(GLOB_RECURSE all_examples RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(ex_src ${all_examples})
    get_filename_component(ex_name ${ex_src} NAME_WE)
    add_executable(${ex_name} ${ex_src})
    target_link_libraries(${ex_name} h5::h5_c hdf5::hdf5 hdf5::hdf5_hl)
endforeach()