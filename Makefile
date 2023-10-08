
BUILD_DIR := build

i_stringEx := -L./stringEx -I./stringEx -lstringEx
BUILD_CMD_WIN := gcc -o $(BUILD_DIR)/ttree tree.c libs/argLib.c $(i_stringEx)
BUILD_CMD_LINUX := gcc -o $(BUILD_DIR)/ttree tree.c libs/argLib.c $(i_stringEx)

ifeq ($(OS),Windows_NT)
$(BUILD_DIR)/tree.exe: tree.c stringEx $(BUILD_DIR)
	$(BUILD_CMD_WIN)
else
$(BUILD_DIR)/tree: tree.c stringEx $(BUILD_DIR)
	$(BUILD_CMD_LINUX)
endif

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

stringEx:
	git clone https://github.com/bendikMichal/stringEx && cd stringEx && make

