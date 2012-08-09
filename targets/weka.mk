WEKA_BRIDGE_LIB_NAME := libWekaBridge.so
WEKA_BRIDGE_LIB := bin/$(ARCH)/$(WEKA_BRIDGE_LIB_NAME)
WEKA_DIR := bin/weka
SRC_DIR := src/learning
CLASSES := $(WEKA_DIR)/WekaBridge.class $(WEKA_DIR)/REPRandomTree.class
CPP_SRC := $(SRC_DIR)/WekaBridge.cpp $(SRC_DIR)/Communicator.cpp
# NOTE: libWekaBridge name used below as well

weka: $(CLASSES) $(WEKA_BRIDGE_LIB)

$(WEKA_DIR)/%.class: $(SRC_DIR)/%.java
	javac -cp $(WEKA_DIR)/weka.jar -d $(WEKA_DIR) $<

$(WEKA_BRIDGE_LIB): $(CPP_SRC)
	@echo "Compling $@"
	@$(CC) -o $@ -shared -fPIC -Wl,-soname,$(WEKA_BRIDGE_LIB_NAME)  -Iinclude  $^ -lc -lboost_thread-mt -lrt

override BINS := $(BINS) bin/$(ARCH)/libWekaBridge.so bin/weka/WekaBridge.class
