cmake_minimum_required(VERSION 3.1)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

project( spectrograms./ )

add_compile_options( -Wall -Wextra -pedantic -fopenmp )

file( GLOB TARGET_SRC "./src/*.cpp" )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )
set( CMAKE_EXPORT_COMPILE_COMMANDS )

# headers
include_directories( ${CMAKE_HOME_DIRECTORY}/include )
add_subdirectory( lib/FFmpeg )

find_package( FFTW REQUIRED )
find_package( OpenCV REQUIRED )
find_package( OpenMP REQUIRED ) 
find_package( Python COMPONENTS Interpreter Development REQUIRED )
find_package( Boost COMPONENTS unit_test_framework python REQUIRED )

include_directories( 
  ${FFTW_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${OpenMP_CXX_INCLUDE_DIRS}
  ${Python_INCLUDE_DIRS}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_PYTHON_LIBRARY}
)

add_executable( spectrograms ${TARGET_SRC} )

target_link_libraries(
  spectrograms
  avcodec
  avutil
  avformat
  swresample
  fftw3_omp
  ${FFTW_LIBRARIES}
  ${OpenCV_LIBS}
  ${Python_LIBRARIES}
  ${OpenMP_CXX_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_PYTHON_LIBRARY}
)

#Add tests
set (BUILD_TESTS) #set TRUE if you want to build tests

if(BUILD_TESTS)
     list(FILTER TARGET_SRC EXCLUDE REGEX /*/main.cpp)
     find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
     
     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build)
     
     add_executable(basic_test ${CMAKE_HOME_DIRECTORY}/test/test.cpp ${TARGET_SRC})
     target_link_libraries(basic_test 
     avcodec
     avutil
     avformat
     swresample
     ${Python_LIBRARIES}
     ${Boost_PYTHON_LIBRARY}
     ${OpenMP_CXX_LIBRARIES}
     fftw3_omp
     ${FFTW_LIBRARIES}
     ${OpenCV_LIBS} ${Boost_FILESYSTEM_LIBRARY}
     ${Boost_SYSTEM_LIBRARY}
     ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
     )
     
     add_test(AutoTest basic_test)
     enable_testing()
     
endif()


# generating documentation

set(BUILD_DOCS) #set TRUE if you want to build docs

if(BUILD_DOCS)
    find_package(Doxygen)
    option(BUILD_DOCUMENTATION "Create and install the HTML based API        
    documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(NOT DOXYGEN_FOUND)
         message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/spectrogramDox)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    message("Doxygen build started.")

    add_custom_target(docs 
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
endif()
