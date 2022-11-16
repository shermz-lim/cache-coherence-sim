
CXX = g++
CXXFLAGS = -std=c++20 -Wall
OFLAGS = -O2
DEBUGFLAGS = -g -DDEBUG

TARGET = coherence
SRC_FILES := $(wildcard code/*.cpp)
OBJ_FILES := $(patsubst code/%.cpp, out/%.o, $(SRC_FILES))
DOBJ_FILES := $(patsubst code/%.cpp, out/%-g.o, $(SRC_FILES))

all: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OFLAGS) -o $(TARGET) $(OBJ_FILES)

out/%.o: code/%.cpp
	$(CXX) $(CXXFLAGS) $(OFLAGS) -c -o $@ $<

debug: $(DOBJ_FILES)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $(TARGET)-g $(DOBJ_FILES)

out/%-g.o: code/%.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c -o $@ $<

clean:
	rm $(TARGET)
	rm $(TARGET)-g
	rm out/*

seed:
	unzip -o "data/*.zip" -d data
