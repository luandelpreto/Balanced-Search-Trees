cmake_minimum_required(VERSION 3.7)

include_directories(common)
include_directories(AVL-BST)

add_subdirectory(common)
add_subdirectory(AVL-BST)

add_library(LorenaBSTs SHARED
    common/Lor_assert
    AVL-BST/Lor_AVLbst.c
)
