
BUILD_DIR := build


ifeq ($(OS),Windows_NT)
$(BUILD_DIR)/tree.exe: tree.c
	gcc -o $(BUILD_DIR)/ttree tree.c libs/stringex.c libs/argLib.c
else
$(BUILD_DIR)/tree: tree.c
	gcc -o $(BUILD_DIR)/ttree ltree.c libs/stringex.c libs/argLib.c
endif

