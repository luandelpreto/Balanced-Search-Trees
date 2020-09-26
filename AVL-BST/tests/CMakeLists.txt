cmake_minimum_required(VERSION 3.7)

add_executable(utesting.out
    utesting.c
)

target_link_libraries(utesting.out
    BStrees
    cmocka
)
