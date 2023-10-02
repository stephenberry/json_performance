cmake_minimum_required(VERSION 3.16...3.25)

project(
  json_performance
  VERSION 0.1
  LANGUAGES CXX
)

if(NOT ${CPM_LOCAL_PACKAGE_PATHS} STREQUAL "")
   include(${CPM_LOCAL_PACKAGE_PATHS})
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

#Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

#FetchContent_Declare(glaze URL https://github.com/stephenberry/glaze/archive/refs/tags/v1.1.2.tar.gz)
#FetchContent_MakeAvailable(glaze)

FetchContent_Declare(
        rapidjson
        URL https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.tar.gz
)
FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
endif()

FetchContent_Declare(
        jsoncons
        URL https://github.com/danielaparker/jsoncons/archive/refs/tags/v0.170.0.tar.gz
)
FetchContent_GetProperties(jsoncons)
if(NOT jsoncons_POPULATED)
    FetchContent_Populate(jsoncons)
endif()

FetchContent_Declare(
  daw_json_link
  URL https://github.com/beached/daw_json_link/archive/refs/tags/v3.15.0.tar.gz
)
FetchContent_MakeAvailable(daw_json_link)

FetchContent_Declare(
  json_struct
  GIT_REPOSITORY https://github.com/jorgen/json_struct
  GIT_TAG master
GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json_struct)

FetchContent_Declare(
  simdjson
  URL https://github.com/simdjson/simdjson/archive/refs/tags/v3.1.7.tar.gz)

set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")

FetchContent_MakeAvailable(simdjson)

FetchContent_Declare(
        yyjson
        SYSTEM TRUE
        URL https://github.com/ibireme/yyjson/archive/refs/tags/0.6.0.tar.gz
)

FetchContent_MakeAvailable(yyjson)

FetchContent_Declare(
        fmt
        SYSTEM TRUE
        GIT_TAG 9.1.0
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.tar.xz
)

set(BOOST_INCLUDE_LIBRARIES json)

FetchContent_MakeAvailable(boost)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include ${json_struct_SOURCE_DIR}/include ${rapidjson_SOURCE_DIR}/include)

find_package(Qt5 COMPONENTS Core)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json glaze::glaze daw::daw-json-link simdjson yyjson fmt::fmt Boost::json)

if (Qt5_FOUND)
  target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_QT=1)
  target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

