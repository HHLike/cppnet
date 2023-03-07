cmake_minimum_required(VERSION 3.10)
project(mycppserver)

include_directories(include)
aux_source_directory(src SRC_LIST)

add_executable(mycppserver ${SRC_LIST})