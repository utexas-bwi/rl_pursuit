addWekaWeights_BIN := weka/addWeights
addWekaWeights_SOURCES := $(addprefix $(SOURCE_DIR)/, main/mainWekaAddWeights.cpp common/DecisionTree.cpp common/WekaParser.cpp common/Point2D.cpp)
addWekaWeights_LINK_FLAGS := $(LINK_FLAGS)
