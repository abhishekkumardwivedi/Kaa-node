cmake_minimum_required(VERSION 2.8)

# Addon name
project(notification)

enable_language(CXX)

set(CMAKE_MODULE_PATH
         ${CMAKE_MODULE_PATH}
         ${CMAKE_CURRENT_SOURCE_DIR}/Modules
         ${CMAKE_CURRENT_SOURCE_DIR}/libs/kaa/Modules
    )

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -I/usr/local/include")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/lib/x86_64-linux-gnu -std=c++0x")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/local/lib -std=gnu++11 -stdlib=libc++")
endif()

find_package (Boost 1.55 REQUIRED COMPONENTS log thread system)
find_package (Avro REQUIRED)
find_package (Botan REQUIRED)

include_directories(${Boost_INCLUDE_DIRS}
                    ${AVRO_INCLUDE_DIRS}
                    ${BOTAN_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kaa)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/kaa/build)


file(GLOB SOURCE_FILES "src/*.cc" "src/*.cpp" "src/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

# Essential library files to link to a node addon
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} kaacpp_s ${BOTAN_LIBRARY} ${Boost_LIBRARIES} ${AVRO_LIBRARIES})
