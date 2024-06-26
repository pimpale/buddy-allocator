TARGET_EXEC ?= ba-example

BUILD_DIR ?= ./obj
SRC_DIRS ?= ./src ./example

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ./inc
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LDFLAGS := 

CC := clang-19
CPPFLAGS ?= $(INC_FLAGS) -std=c23 -MMD -MP -O0 -g3 -Wall -Weverything -pedantic -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-pre-c23-compat -Wno-padded

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
