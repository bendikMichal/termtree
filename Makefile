
BUILD_DIR := build

i_stringEx := -L./stringEx -I./stringEx -lstringEx
BUILD_CMD_WIN := gcc -o $(BUILD_DIR)/ttree tree.c libs/argLib.c libs/common.c libs/specificWindows.c $(i_stringEx)
BUILD_CMD_LINUX := gcc -o $(BUILD_DIR)/ttree tree.c libs/argLib.c libs/common.c libs/specificLinux.c $(i_stringEx)

ifeq ($(OS),Windows_NT)
$(BUILD_DIR)/tree.exe: tree.c stringEx $(BUILD_DIR)
	cd stringEx && git pull && make
	$(BUILD_CMD_WIN)
else
$(BUILD_DIR)/tree: tree.c stringEx $(BUILD_DIR)
	cd stringEx && git pull && make
	$(BUILD_CMD_LINUX)
endif

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

stringEx:
	git clone https://github.com/bendikMichal/stringEx && cd stringEx && make

ifeq ($(OS),Windows_NT)
install:
	@echo Install is only supported on linux
uninstall:
	@echo Uninstall is only supported on linux
else
install:
	@sudo cp ttree.1 /usr/share/man/man1
	@sudo cp build/ttree /usr/bin/ttree
uninstall:
	@sudo rm /usr/share/man/man1/ttree.1
	@sudo rm /usr/bin/ttree
endif
