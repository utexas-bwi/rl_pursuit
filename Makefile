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
TEMPLATE_TARGETS := $(filter-out weka, $(TARGETS))
# sources
MODULES := common controller factory learning model planning
SOURCES := $(wildcard $(patsubst %, $(SOURCE_DIR)/%/*.cpp, $(MODULES)))
SOURCES := $(filter-out $(SOURCE_DIR)/learning/WekaBridge.cpp,$(SOURCES)) # don't compile weka bridge, it's an abomination and will be handled separately
# flags
FLAGS_NO_STD = -W -Wall -Werror -pedantic-errors -O3 -I$(SOURCE_DIR) -I$(INCLUDE_DIR) -I/usr/include/python$(PYTHON_VERSION)
FLAGS = $(FLAGS_NO_STD) -std=c++0x
STUDENT_FLAGS = -I$(SOURCE_DIR) -I$(INCLUDE_DIR)
LINK_FLAGS = -L$(LIBS_DIR) -ljson -lpython$(PYTHON_VERSION) -lboost_python -lgflags -llinear -lblas -lrt

default: all

include $(TARGET_MAKEFILES)

all: $(TARGETS)

.PHONY: clean fullclean fclean $(TARGETS)

define TARGET_template
# get the objects
override OBJS := $$(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o,$$($1_SOURCES))

# get the binary, defaulting to the name
ifeq ($$(strip $$($1_BIN)),)
	override BIN := $$(patsubst %, bin/$(ARCH)/%, $1)
else
	override BIN := $$(patsubst %, bin/$(ARCH)/%, $$($1_BIN))
endif

# get the link flags, defaulting to LINK_FLAGS
ifeq ($$(strip $$($1_LINK_FLAGS)),)
	override LINK_FLAGS_TEMP := $$(LINK_FLAGS)
else
	override LINK_FLAGS_TEMP := $$($1_LINK_FLAGS)
endif

# set up the rules
print_$1:
	@echo $$($1_SOURCES)

$1: $$(BIN)
$$(BIN): $$(OBJS)
$$(BIN): LINK_FLAGS:=$$(LINK_FLAGS_TEMP)
DEPS := $$(DEPS) $$(OBJS:.o=.d)
OBJECTS := $$(OBJECTS) $$(OBJS)
override BINS := $$(BINS) $$(BIN)
endef

$(foreach target,$(TEMPLATE_TARGETS),$(eval $(call TARGET_template,$(target))))

OBJECTS_ALL = $(sort $(OBJECTS))

clean: cleandeps
	$(RM) $(OBJECTS_ALL)

cleandeps:
	$(RM) $(DEPS)

fullclean: clean
	$(RM) $(BINS)

fclean: fullclean

# include dependencies for creating dependencies and objects
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),fullclean)
ifneq ($(MAKECMDGOALS),fclean)
ifneq ($(MAKECMDGOALS),cleandeps)
include $(DEPS)
endif
endif
endif
endif

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

bin/$(ARCH)/%:
	@echo "Linking $@"
	@$(CC) $(FLAGS) $^ $(LINK_FLAGS) -o $@

# Special finicky files:
$(BUILD_DIR)/learning/WekaClassifier.o: FLAGS := $(FLAGS) -D COMPILE_ARCH=$(ARCH)
$(BUILD_DIR)/learning/WekaClassifier.d: FLAGS := $(FLAGS) -D COMPILE_ARCH=$(ARCH)
$(BUILD_DIR)/learning/Communicator.o: FLAGS := $(FLAGS_NO_STD)
$(BUILD_DIR)/learning/libsvm.o: FLAGS = -O3 -w
