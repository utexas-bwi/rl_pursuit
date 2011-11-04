TEST_TARGET = bin/test
# base dirs
SOURCE_DIR = src
INCLUDE_DIR = include
KERNEL_TYPE := $(shell uname -m)
ifeq ($(KERNEL_TYPE),x86_64)
ARCH=64
else
ARCH=32
endif
LIBS_DIR = libs/$(ARCH)
MAIN_TARGET = bin/main$(ARCH)
WEKA_TARGET = bin/weka/addWeights$(ARCH)
BUILD_DIR = build/$(ARCH)
# sub directories for source
COMMON_DIR = common
CONTROLLER_DIR = controller
FACTORY_DIR = factory
MODEL_DIR = model
PLANNING_DIR = planning
TEST_DIR = test
STUDENT_DIR= studentAgents/agents
# specify compile and flags
ifeq ($(shell bin/onLabMachine.sh),yes)
PYTHON_VERSION=2.6
else
PYTHON_VERSION=2.7
endif
#CC = condor_compile g++
CC = g++
FLAGS = -W -Wall -Werror -pedantic-errors -O2 -I$(SOURCE_DIR) -I$(INCLUDE_DIR) -I/usr/include/python$(PYTHON_VERSION) -std=c++0x
LINK_FLAGS = -L$(LIBS_DIR) -ljson -lpython$(PYTHON_VERSION) -lboost_python
TEST_LINK_FLAGS = $(LINK_FLAGS) -lgtest -lpthread
STUDENT_FLAGS = -I$(SOURCE_DIR) -I$(INCLUDE_DIR)

RM = rm -f
# source files
MAIN_SOURCES = main.cpp
WEKA_SOURCES = mainWekaAddWeights.cpp common/DecisionTree.cpp common/WekaParser.cpp common/Point2D.cpp
COMMON_SOURCES = DecisionTree.cpp FeatureExtractor.cpp OutputDT.cpp Point2D.cpp tinymt32.cpp Util.cpp WekaParser.cpp
CONTROLLER_SOURCES = AStar.cpp ModelUpdater.cpp ModelUpdaterBayes.cpp ModelUpdaterSilver.cpp PredatorDecisionTree.cpp PredatorGreedy.cpp PredatorGreedyProbabilistic.cpp PredatorMCTS.cpp PredatorProbabilisticDestinations.cpp PredatorStudentCpp.cpp PredatorStudentCpp_gen.cpp PredatorStudentPython.cpp PredatorStudentPythonNew.cpp PredatorSurround.cpp PredatorSurroundWithPenalties.cpp PredatorTeammateAware.cpp PreyAvoidNeighbor.cpp State.cpp World.cpp WorldBeliefMDP.cpp WorldMDP.cpp# WorldMultiModelMDP.cpp WorldSilverMDP.cpp WorldSilverWeightedMDP.cpp
FACTORY_SOURCES = AgentFactory.cpp PlanningFactory.cpp WorldFactory.cpp
MODEL_SOURCES = AgentModel.cpp Common.cpp WorldModel.cpp
PLANNING_SOURCES = 
STUDENT_SOURCES = $(patsubst $(SOURCE_DIR)/%, %, $(wildcard $(SOURCE_DIR)/$(STUDENT_DIR)/*/Predator.cpp))
#TEST_SOURCES = planningSpeed.cpp
#TEST_SOURCES = pursuitTest.cpp
#TEST_SOURCES = jsonTest.cpp
#TEST_SOURCES = AStarTest.cpp
TEST_SOURCES = main.cpp DefaultMap.cpp ModelUpdaterBayes.cpp UCTEstimator.cpp World.cpp WorldModel.cpp WorldBeliefMDP.cpp WorldMDP.cpp

##############################################################

# full paths to the source files
COMMON_SOURCES_PATH = $(addprefix $(COMMON_DIR)/, $(COMMON_SOURCES))
CONTROLLER_SOURCES_PATH = $(addprefix $(CONTROLLER_DIR)/, $(CONTROLLER_SOURCES))
FACTORY_SOURCES_PATH = $(addprefix $(FACTORY_DIR)/, $(FACTORY_SOURCES))
MODEL_SOURCES_PATH = $(addprefix $(MODEL_DIR)/, $(MODEL_SOURCES))
PLANNING_SOURCES_PATH = $(addprefix $(PLANNING_DIR)/, $(PLANNING_SOURCES))
# the sources without tests
SOURCES = $(COMMON_SOURCES_PATH) $(CONTROLLER_SOURCES_PATH) $(FACTORY_SOURCES_PATH) $(MODEL_SOURCES_PATH) $(PLANNING_SOURCES_PATH) $(STUDENT_SOURCES)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
MAIN_OBJECTS = $(OBJECTS) $(addprefix $(BUILD_DIR)/, $(MAIN_SOURCES:.cpp=.o))
WEKA_OBJECTS = $(addprefix $(BUILD_DIR)/, $(WEKA_SOURCES:.cpp=.o))
# tests
TEST_SOURCES_PATH = $(addprefix $(TEST_DIR)/, $(TEST_SOURCES))
TEST_SOURCES_COMBINED = $(SOURCES) $(TEST_SOURCES_PATH)
TEST_OBJECTS = $(addprefix $(BUILD_DIR)/, $(TEST_SOURCES_COMBINED:.cpp=.o))
# all sources
SOURCES_ALL = $(SOURCES) $(MAIN_SOURCES) $(WEKA_SOURCES) $(TEST_SOURCES_PATH)
DEPS_ALL = $(addprefix $(BUILD_DIR)/,$(SOURCES_ALL:.cpp=.d))

.PHONY: all run build test default clean fullclean

default: build

all: build weka

run: $(MAIN_TARGET)
	$(MAIN_TARGET)

build: $(MAIN_TARGET)

weka: $(WEKA_TARGET)

test: $(TEST_TARGET)
	$(TEST_TARGET)

clean:
	$(RM) $(TEST_OBJECTS) $(MAIN_OBJECTS) $(DEPS_ALL)

fullclean: clean
	$(RM) $(TEST_TARGET) $(MAIN_TARGET) $(WEKA_TARGET)

fclean: fullclean

$(MAIN_TARGET): $(MAIN_OBJECTS)
	@echo "Linking $@"
	@$(CC) $(FLAGS) $^ $(LINK_FLAGS) -o $@

$(WEKA_TARGET): $(WEKA_OBJECTS)
	@echo "Linking $@"
	@$(CC) $(FLAGS) $^ $(LINK_FLAGS) -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	@echo "Linking $@"
	@$(CC) $(FLAGS) $^ $(TEST_LINK_FLAGS) -o $@

# include dependencies for creating dependencies and objects
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS_ALL)
endif

# change the flags for the students
$(BUILD_DIR)/$(STUDENT_DIR)/%.o : FLAGS = $(STUDENT_FLAGS)
# compile the objects
$(BUILD_DIR)/%.o: 
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
