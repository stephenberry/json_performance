cmake_minimum_required(VERSION 3.16)

project(
  json_performance
  VERSION 0.1
  LANGUAGES CXX
)

if(NOT ${CPM_LOCAL_PACKAGE_PATHS} STREQUAL "")
   include(${CPM_LOCAL_PACKAGE_PATHS})
endif()

include(cmake/CPM.cmake)

CPMAddPackage(
   NAME glaze
   GIT_REPOSITORY https://github.com/stephenberry/glaze
   GIT_TAG main
)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE include)

target_link_libraries(${PROJECT_NAME} INTERFACE glaze)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
   # This flag allows visual studio projects to be organized into folders
   # in the solution explorer.
   set_property(GLOBAL PROPERTY USE_FOLDERS ON)

   # create dummy target since some editors will not display interface targets
   file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/glaze/*.hpp")
   source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "headers" FILES ${headers})
   file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
   source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "sources" FILES ${sources})
   add_executable(_${PROJECT_NAME} ${headers} ${sources})
   target_link_libraries(_${PROJECT_NAME} PRIVATE ${PROJECT_NAME})
   if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
     target_compile_options(_${PROJECT_NAME} PRIVATE -Wall -Wextra)
   elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
     target_compile_options(_${PROJECT_NAME} PRIVATE /W4)
   endif()
endif()
