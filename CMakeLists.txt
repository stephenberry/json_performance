cmake_minimum_required(VERSION 3.16)

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

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

FetchContent_Declare(
        rapidjson
        GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
        GIT_TAG        master
	GIT_SHALLOW TRUE
)
FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
endif()

FetchContent_Declare(
        jsoncons
        GIT_REPOSITORY https://github.com/danielaparker/jsoncons
        GIT_TAG        master
	GIT_SHALLOW TRUE
)
FetchContent_GetProperties(jsoncons)
if(NOT jsoncons_POPULATED)
    FetchContent_Populate(jsoncons)
endif()

FetchContent_Declare(
  daw_json_link
  GIT_REPOSITORY https://github.com/beached/daw_json_link
  GIT_TAG release
GIT_SHALLOW TRUE
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
  GIT_REPOSITORY https://github.com/simdjson/simdjson.git
  GIT_TAG  v3.1.3
  GIT_SHALLOW TRUE)

set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")

FetchContent_MakeAvailable(simdjson)

FetchContent_Declare(
        yyjson
        SYSTEM TRUE
        GIT_TAG 0.6.0
        GIT_REPOSITORY https://github.com/ibireme/yyjson.git
	GIT_SHALLOW TRUE
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

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include ${json_struct_SOURCE_DIR}/include ${rapidjson_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json glaze::glaze daw::daw-json-link simdjson yyjson fmt::fmt)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

