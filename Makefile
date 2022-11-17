
CXX = clang++
CXXFLAGS = -std=c++20 -Wall
OFLAGS = -O2
DEBUGFLAGS = -g -DDEBUG

TARGET = coherence
SRC_FILES := $(wildcard code/*.cpp)
OBJ_FILES := $(patsubst code/%.cpp, obj/%.o, $(SRC_FILES))
DOBJ_FILES := $(patsubst code/%.cpp, obj/%-g.o, $(SRC_FILES))

all: obj $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OFLAGS) -o $(TARGET) $(OBJ_FILES)

obj/%.o: code/%.cpp
	$(CXX) $(CXXFLAGS) $(OFLAGS) -c -o $@ $<

debug: obj $(DOBJ_FILES)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $(TARGET)-g $(DOBJ_FILES)

obj/%-g.o: code/%.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c -o $@ $<

obj:
	mkdir -p obj

clean:
	rm obj/*
	rm $(TARGET)
	rm $(TARGET)-g

seed:
	unzip -o "data/*.zip" -d data
