cmake_minimum_required(VERSION 3.7)

include_directories(
	common
	Mem-Pool
	AVL-BST
)

add_subdirectory(common)
add_subdirectory(Mem-Pool)
add_subdirectory(AVL-BST)

add_library(LorenaBSTs SHARED
	common/Lor_assert
	Mem-Pool/Lor_mem_pool.c
	AVL-BST/Lor_AVLbst.c
)
