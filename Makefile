MAIN_TARGET = bin/main
TEST_TARGET = bin/test
# base dirs
BUILD_DIR = build
SOURCE_DIR = src
INCLUDE_DIR = include
KERNEL_TYPE := $(shell uname -m)
ifeq ($(KERNEL_TYPE),x86_64)
LIBS_DIR = libs/64
else
LIBS_DIR = libs/32
endif
# sub directories for source
COMMON_DIR = common
CONTROLLER_DIR = controller
FACTORY_DIR = factory
MODEL_DIR = model
PLANNING_DIR = planning
TEST_DIR = test
# specify compile and flags
CC = g++
FLAGS = -W -Wall -pedantic-errors -O2 -I$(SOURCE_DIR) -I$(INCLUDE_DIR) -fPIC -std=c++0x
LINK_FLAGS = -L$(LIBS_DIR) -ljson
TEST_LINK_FLAGS = $(LINK_FLAGS) -lgtest -lpthread

RM = rm -f
# source files
MAIN_SOURCES = $(SOURCE_DIR)/main.cpp
COMMON_SOURCES = Point2D.cpp tinymt32.cpp Util.cpp
CONTROLLER_SOURCES = AStar.cpp PredatorGreedy.cpp PredatorGreedyProbabilistic.cpp PredatorMCTS.cpp PredatorProbabilisticDestinations.cpp PredatorTeammateAware.cpp World.cpp WorldMDP.cpp
FACTORY_SOURCES = AgentFactory.cpp PlanningFactory.cpp WorldFactory.cpp
MODEL_SOURCES = AgentModel.cpp Common.cpp WorldModel.cpp
PLANNING_SOURCES = 
# Headers
COMMON_HEADERS = DefaultMap.h Point2D.h RNG.h tinymt32.h Util.h
CONTROLLER_HEADERS = Agent.h AgentDummy.h AgentRandom.h AStar.h PredatorGreedy.h PredatorGreedyProbabilistic.h PredatorMCTS.h PredatorProbabilisticDestinations.h PredatorTeammateAware.h World.h WorldMDP.h
FACTORY_HEADERS = AgentFactory.h PlanningFactory.h WorldFactory.h
MODEL_HEADERS = AgentModel.h Common.h WorldModel.h
PLANNING_HEADERS = MCTS.h Model.h UCTEstimator.h ValueEstimator.h
#TEST_SOURCES = planningSpeed.cpp
#TEST_SOURCES = pursuitTest.cpp
#TEST_SOURCES = jsonTest.cpp
#TEST_SOURCES = AStarTest.cpp
TEST_SOURCES = main.cpp DefaultMap.cpp UCTEstimator.cpp World.cpp WorldModel.cpp

##############################################################

# full paths to the source files
COMMON_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(COMMON_DIR)/, $(COMMON_SOURCES))
CONTROLLER_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(CONTROLLER_DIR)/, $(CONTROLLER_SOURCES))
FACTORY_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(FACTORY_DIR)/, $(FACTORY_SOURCES))
MODEL_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(MODEL_DIR)/, $(MODEL_SOURCES))
PLANNING_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(PLANNING_DIR)/, $(PLANNING_SOURCES))
# full paths to the headers
COMMON_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(COMMON_DIR)/, $(COMMON_HEADERS))
CONTROLLER_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(CONTROLLER_DIR)/, $(CONTROLLER_HEADERS))
FACTORY_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(FACTORY_DIR)/, $(FACTORY_HEADERS))
MODEL_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(MODEL_DIR)/, $(MODEL_HEADERS))
PLANNING_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(PLANNING_DIR)/, $(PLANNING_HEADERS))
# the sources - tests
SOURCES = $(COMMON_SOURCES_PATH) $(CONTROLLER_SOURCES_PATH) $(FACTORY_SOURCES_PATH) $(MODEL_SOURCES_PATH) $(PLANNING_SOURCES_PATH)
OBJECTS = $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.cpp=.o))
MAIN_OBJECTS = $(OBJECTS) $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(MAIN_SOURCES:.cpp=.o))
# headers
HEADERS = $(COMMON_HEADERS_PATH) $(CONTROLLER_HEADERS_PATH) $(FACTORY_HEADERS_PATH) $(MODEL_HEADERS_PATH) $(PLANNING_HEADERS_PATH)

# tests
TEST_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(TEST_DIR)/, $(TEST_SOURCES))
TEST_SOURCES_COMBINED = $(SOURCES) $(TEST_SOURCES_PATH)
TEST_OBJECTS = $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(TEST_SOURCES_COMBINED:.cpp=.o))

.PHONY: all run build test default clean fullclean

default: build

run: $(MAIN_TARGET)
	$(MAIN_TARGET)

build: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_OBJECTS)
	$(CC) $(FLAGS) $(MAIN_OBJECTS) $(LINK_FLAGS) -o $@

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJECTS) $(HEADERS)
	$(CC) $(FLAGS) $(TEST_OBJECTS) $(TEST_LINK_FLAGS) -o $@

clean:
	$(RM) $(TEST_OBJECTS)

fullclean: clean
	$(RM) $(TEST_TARGET)

fclean: fullclean

