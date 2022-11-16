
CPPFLAGS = -std=c++20 -O2 -Wall
TARGET = coherence
SRC_FILES := $(wildcard *.cpp)

all:
	g++ $(CPPFLAGS) -o $(TARGET) $(SRC_FILES)

clean:
	rm $(TARGET)

seed:
	unzip -o "data/*.zip" -d data
