cmake_minimum_required(VERSION 3.7)

include_directories(AVL-BST)

add_subdirectory(AVL-BST)

add_library(BStrees SHARED
    AVL-BST/AVLbst.c
)
