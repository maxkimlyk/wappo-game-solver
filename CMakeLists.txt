cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find BOOST package")
endif()

file(GLOB SRC src/*.cpp)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(wappo_game_solver ${SRC})
target_link_libraries(wappo_game_solver LINK_PUBLIC ${Boost_LIBRARIES})
