test_TARGET := test
test_SOURCES := $(SOURCES) $(addprefix $(SOURCE_DIR)/test/, main.cpp DefaultMap.cpp ModelUpdaterBayes.cpp UCTEstimator.cpp World.cpp WorldModel.cpp WorldBeliefMDP.cpp WorldMDP.cpp)
test_LINK_FLAGS := $(LINK_FLAGS)  -lgtest -lpthread
