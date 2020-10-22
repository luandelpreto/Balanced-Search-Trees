cmake_minimum_required(VERSION 3.7)

add_executable(MP-utesting.out
    MP-utesting.c
)

target_link_libraries(MP-utesting.out
    LorenaBSTs
    cmocka
)
