# CMakeList.txt : CMake project for 477-hw2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("477-hw2")

# Add source to this project's executable.
file (GLOB a_src "*.h" "*.cpp")
add_compile_options(-mavx2)
add_executable (477-hw2  ${a_src})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 477-hw2 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
