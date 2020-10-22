cmake_minimum_required(VERSION 3.7)

add_executable(AVL-utesting.out
    AVL-utesting.c
)

target_link_libraries(AVL-utesting.out
    LorenaBSTs
    cmocka
)
