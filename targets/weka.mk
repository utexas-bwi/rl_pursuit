WEKA_BRIDGE_LIB_NAME := libWekaBridge.so
WEKA_BRIDGE_LIB := bin/$(ARCH)/$(WEKA_BRIDGE_LIB_NAME)
# NOTE: libWekaBridge name used below as well

weka: bin/weka/WekaBridge.class $(WEKA_BRIDGE_LIB)

bin/weka/WekaBridge.class: src/learning/WekaBridge.java
	javac -cp bin/weka/weka.jar -d bin/weka $<

$(WEKA_BRIDGE_LIB): src/learning/WekaBridge.cpp src/learning/Communicator.cpp
	@echo "Compling $@"
	@$(CC) -o $@ -shared -fPIC -Wl,-soname,$(WEKA_BRIDGE_LIB_NAME)  -Iinclude  $^ -lc -lboost_thread-mt -lrt

override BINS := $(BINS) bin/$(ARCH)/libWekaBridge.so bin/weka/WekaBridge.class
