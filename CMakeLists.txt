cmake_minimum_required(VERSION 3.0.0)
project(typeconv VERSION 0.1.0)
message("CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")

set(PROJECT_PATH ${CMAKE_SOURCE_DIR})
set(TARGET_NAME ${CMAKE_PROJECT_NAME})

add_executable(typeconv
    main.c
)

include_directories(

)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
