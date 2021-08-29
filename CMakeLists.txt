cmake_minimum_required(VERSION 3.1)
project(asyncdemo CXX)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_EXTENSIONS OFF)

find_package(boost COMPONENTS boost thread REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/std_async.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/await_future.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/await_boost_future.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scope_printer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/future_coroutine.hpp
)
 
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        BOOST_THREAD_PROVIDES_FUTURE
        BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        Boost::boost
        Boost::thread
)

