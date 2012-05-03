cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
project(Box2D)

if(UNIX)
    set(BOX2D_INSTALL_BY_DEFAULT ON)
else(UNIX)
    set(BOX2D_INSTALL_BY_DEFAULT OFF)
endif(UNIX)

option(BOX2D_INSTALL "Install Box2D libs, includes, and CMake scripts" ${BOX2D_INSTALL_BY_DEFAULT})
option(BOX2D_INSTALL_DOC "Install Box2D documentation" OFF)
option(BOX2D_BUILD_SHARED "Build Box2D shared libraries" OFF)
option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)
option(BOX2D_BUILD_EXAMPLES "Build Box2D examples" ON)

set(BOX2D_VERSION 2.1.0)

SET(BOOST_ROOT "C:/Users/Lubo/Documents/Visual Studio 2010/Libraries/boost_1_49_0")
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
# The Box2D library.
add_subdirectory(Box2D)

if(BOX2D_BUILD_EXAMPLES)
    # Testbed and dependencies.
    find_package(OpenGL REQUIRED)
    add_subdirectory(freeglut)
    add_subdirectory(glui)
    add_subdirectory(json_spirit)
    add_subdirectory(PictureCollage)
endif(BOX2D_BUILD_EXAMPLES)

if(BOX2D_INSTALL_DOC)
    install(DIRECTORY Documentation DESTINATION share/doc/Box2D PATTERN ".svn" EXCLUDE)
endif(BOX2D_INSTALL_DOC)
