TEST_TARGET = bin/test

BUILD_DIR = build
SOURCE_DIR = src
INCLUDE_DIR = include
LIBS_DIR = libs
# sub directories for source
COMMON_DIR = common
CONTROLLER_DIR = controller
FACTORY_DIR = factory
MODEL_DIR = model
PLANNING_DIR = planning
TEST_DIR = test
# specify compile and flags
CC = g++
FLAGS = -W -Wall -pedantic-errors -O2 -I$(SOURCE_DIR) -I$(INCLUDE_DIR) -fPIC
LINK_FLAGS = -L$(LIBS_DIR) -ljson
RM = rm -f
# source files
COMMON_SOURCES = Point2D.cpp tinymt32.cpp Util.cpp
CONTROLLER_SOURCES = PredatorGreedy.cpp PredatorMCTS.cpp World.cpp WorldMDP.cpp
FACTORY_SOURCES = AgentFactory.cpp WorldFactory.cpp
MODEL_SOURCES = Common.cpp WorldModel.cpp
PLANNING_SOURCES = 
# Headers
COMMON_HEADERS = DefaultMap.h Point2D.h RNG.h tinymt32.h Util.h
CONTROLLER_HEADERS = Agent.h AgentDummy.h Prey.h World.h WorldMDP.h
FACTORY_HEADERS = AgentFactory.h WorldFactory.h
MODEL_HEADERS = AgentModel.h Common.h WorldModel.h
PLANNING_HEADERS = MCTS.h Model.h UCTEstimator.h ValueEstimator.h
#TEST_SOURCES = planningSpeed.cpp
TEST_SOURCES = pursuitTest.cpp
# full paths to the source files
COMMON_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(COMMON_DIR)/, $(COMMON_SOURCES))
CONTROLLER_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(CONTROLLER_DIR)/, $(CONTROLLER_SOURCES))
FACTORY_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(FACTORY_DIR)/, $(FACTORY_SOURCES))
MODEL_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(MODEL_DIR)/, $(MODEL_SOURCES))
PLANNING_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(PLANNING_DIR)/, $(PLANNING_SOURCES))
TEST_SOURCES_PATH = $(addprefix $(SOURCE_DIR)/$(TEST_DIR)/, $(TEST_SOURCES))
# full paths to the headers
COMMON_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(COMMON_DIR)/, $(COMMON_HEADERS))
CONTROLLER_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(CONTROLLER_DIR)/, $(CONTROLLER_HEADERS))
FACTORY_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(FACTORY_DIR)/, $(FACTORY_HEADERS))
MODEL_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(MODEL_DIR)/, $(MODEL_HEADERS))
PLANNING_HEADERS_PATH = $(addprefix $(SOURCE_DIR)/$(PLANNING_DIR)/, $(PLANNING_HEADERS))
# the sources - tests
SOURCES = $(COMMON_SOURCES_PATH) $(CONTROLLER_SOURCES_PATH) $(FACTORY_SOURCES_PATH) $(MODEL_SOURCES_PATH) $(PLANNING_SOURCES_PATH)
OBJECTS = $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.cpp=.o))
# sources + tests
TEST_SOURCES_COMBINED = $(SOURCES) $(TEST_SOURCES_PATH)
TEST_OBJECTS = $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(TEST_SOURCES_COMBINED:.cpp=.o))
# headers
HEADERS = $(COMMON_HEADERS_PATH) $(CONTROLLER_HEADERS_PATH) $(MODEL_HEADERS_PATH) $(PLANNING_HEADERS_PATH)

.PHONY: all test default clean fullclean

default: test

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJECTS) $(HEADERS)
	$(CC) $(FLAGS) $(TEST_OBJECTS) $(LINK_FLAGS) -o $@

clean:
	$(RM) $(TEST_OBJECTS)

fullclean: clean
	$(RM) $(TEST_TARGET)

fclean: fullclean

