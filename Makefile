
CPPFLAGS = -g -Wall
TARGET = coherence
SRC_FILES := $(wildcard *.cpp)

all:
	g++ $(CPPFLAGS) -o $(TARGET) $(SRC_FILES)

clean:
	rm $(TARGET)

seed:
	unzip -o "data/*.zip" -d data
