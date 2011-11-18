# basic commands
CC = g++
RM = rm -f
# get some information about our system
ifeq ($(shell uname -m),x86_64)
ARCH=64
else
ARCH=32
endif
ifeq ($(shell bin/onLabMachine.sh),yes)
PYTHON_VERSION=2.6
else
PYTHON_VERSION=2.7
endif
# dirs
SOURCE_DIR := src
MAIN_DIR := $(SOURCE_DIR)/main
INCLUDE_DIR := include
LIBS_DIR := libs/$(ARCH)
BUILD_DIR := build/$(ARCH)
TARGET_DIR := targets
# targets
TARGET_MAKEFILES := $(wildcard $(TARGET_DIR)/*.mk)
TARGETS := $(patsubst $(TARGET_DIR)/%.mk, %, $(TARGET_MAKEFILES))
# sources
MODULES := common controller factory model planning
SOURCES := $(wildcard $(patsubst %, $(SOURCE_DIR)/%/*.cpp, $(MODULES)))
SOURCE_STUDENTS := $(wildcard $(SOURCE_DIR)/studentAgents/agents/*/Predator.cpp)
SOURCE_STUDENTS += $(wildcard $(SOURCE_DIR)/studentAgents/agentsNew/*/cppPredator/MyPredator.cpp)
SOURCES += $(SOURCE_STUDENTS)
# objects
OBJECTS_STUDENT := $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(SOURCE_STUDENTS:.cpp=.o))
# flags
FLAGS = -W -Wall -Werror -pedantic-errors -O2 -I$(SOURCE_DIR) -I$(INCLUDE_DIR) -I/usr/include/python$(PYTHON_VERSION) -std=c++0x
STUDENT_FLAGS = -I$(SOURCE_DIR) -I$(INCLUDE_DIR)
LINK_FLAGS = -L$(LIBS_DIR) -ljson -lpython$(PYTHON_VERSION) -lboost_python

include $(TARGET_MAKEFILES)

default: main

all: $(TARGETS)

define TARGET_template
override OBJS := $$(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o,$$($1_SOURCES))
override BIN := $$(patsubst %, bin/%$(ARCH), $$($1_BIN))
$1: $$(BIN)
$$(BIN): $$(OBJS)
$$(BIN): LINK_FLAGS=$$($1_LINK_FLAGS)
DEPS := $$(DEPS) $$(OBJS:.o=.d)
OBJECTS := $$(OBJECTS) $$(OBJS)
BINS := $$(BINS) $$(BIN)
endef

$(foreach target,$(TARGETS),$(eval $(call TARGET_template,$(target))))

OBJECTS_ALL = $(sort $(OBJECTS))

clean:
	$(RM) $(OBJECTS_ALL) $(DEPS)


fullclean: clean
	$(RM) $(BINS)

fclean: fullclean

# include dependencies for creating dependencies and objects
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

# change the flags for the students
$(OBJECTS_STUDENT) : FLAGS = $(STUDENT_FLAGS)
# compile the objects
$(OBJECTS_ALL):
	@mkdir -p $(dir $@)
	@echo "Compiling $< for $(ARCH)"
	@$(CC) $(FLAGS) -c $< -o $@

# calculate the dependencies, note that I hacked the sed line to make it correctly set the targets
$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Calculating the dependencies for $<"
	@set -e; rm -f $@; \
	$(CC) -MM $(FLAGS) $< > $@.$$$$;                  \
	sed 's,\(.*\).o:,$(@:.d=.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

bin/%$(ARCH):
	@echo "Linking $@"
	@$(CC) $(FLAGS) $^ $(LINK_FLAGS) -o $@
