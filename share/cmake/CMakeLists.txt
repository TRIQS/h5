configure_file(h5-config.cmake.in h5-config.cmake @ONLY)
configure_file(h5-config-version.cmake.in h5-config-version.cmake @ONLY)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/h5-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/h5-config-version.cmake
  DESTINATION lib/cmake/h5
)

install(EXPORT h5-targets NAMESPACE h5:: DESTINATION lib/cmake/h5)
