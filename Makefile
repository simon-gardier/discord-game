CC      = g++
LD      = g++
CFLAGS  = -std=c++20 -Wall -Wextra -pedantic -Wno-c++20-compat -Wunused-function -Wunused-variable
LDFLAGS = -std=c++20 -Wall -Wextra -pedantic -Wno-c++20-compat -Wunused-function -Wunused-variable
LIBS    = -ldpp

EXEC    	= entrapment
SRC_DIR  	= src
BUILD_DIR 	= build
BACKUP_DIR 	= backup
SRCS     	= $(wildcard $(SRC_DIR)/*.cpp)
HEADERS  	= $(wildcard $(SRC_DIR)/*.hpp)
OBJS     	= $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean flash static_test

all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXEC)

$(MAIN_OBJ): main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp 
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(EXEC)

clean_backup:
	rm -rf $(BACKUP_DIR)

static_test:
	cppcheck $(SRCS)
	clang-tidy $(SRCS)
