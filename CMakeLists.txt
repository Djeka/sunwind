# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.


project( sunwind_example )

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
set(CMAKE_CXX_FLAGS_RELEASE " -march=core2 -msse -msse2 -malign-double -mssse3 -mmmx -mpc32 -mtune=core2 -O3 -pg") 
if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )  
endif()
 
find_package(CGAL QUIET COMPONENTS Core )
find_package(MPI REQUIRED)
INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(mpicxx "MPI C++ Compiler")

if ( CGAL_FOUND )

  include( ${CGAL_USE_FILE} )

  include( CGAL_CreateSingleSourceCGALProgram )

  create_single_source_cgal_program( "main.cpp" )
  create_single_source_cgal_program( "main-parallel.cpp" )

else()
  
    message(STATUS "This program requires the CGAL library, and will not be compiled.")
  
endif()

