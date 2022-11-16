
CPPFLAGS = -std=c++20 -Wall
TARGET = coherence
SRC_FILES := $(wildcard *.cpp)

all:
	g++ $(CPPFLAGS) -O2 -o $(TARGET) $(SRC_FILES)

debug:
	g++ $(CPPFLAGS) -g -DDEBUG -o $(TARGET)-g $(SRC_FILES)

clean:
	rm $(TARGET)

seed:
	unzip -o "data/*.zip" -d data
