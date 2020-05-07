add_subdirectory(cmake)

if(NOT IS_SUBPROJECT)

  if(PythonSupport)
    set(EXPORT_PYTHON_PATH "export PYTHONPATH=${CMAKE_INSTALL_PREFIX}/${CPP2PY_PYTHON_LIB_DEST_ROOT}:$PYTHONPATH")
    set(MODFILE_PYTHON_PATH "prepend-path    PYTHONPATH          $root/${CPP2PY_PYTHON_LIB_DEST_ROOT}")
  endif()

  configure_file(h5.modulefile.in h5.modulefile @ONLY)
  configure_file(h5vars.sh.in h5vars.sh @ONLY)

  install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/h5.modulefile
      ${CMAKE_CURRENT_BINARY_DIR}/h5vars.sh
    DESTINATION share
  )

  message(STATUS "***************************************************************")
  message(STATUS "* Use:                                                         ")
  message(STATUS "*                                                              ")
  message(STATUS "*   source ${CMAKE_INSTALL_PREFIX}/share/h5vars.sh            ")
  message(STATUS "*                                                              ")
  message(STATUS "* to set up the environment variables                          ")
  message(STATUS "***************************************************************")

endif()
