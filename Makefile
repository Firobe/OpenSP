COMPILER = c++
ADD_CFLAGS = -Wall -std=c++11 -O3
LIBS = sfml-all
NO_PKG_LIBS = -lBox2D
CXXFLAGS = `pkg-config --static --cflags $(LIBS)` $(ADD_CFLAGS)
LINKS = `pkg-config --static --libs $(LIBS)` $(NO_PKG_LIBS)
SOURCES = $(wildcard src/*.cpp)
EXEC_NAME = openSP
OBJECTS = $(SOURCES:.cpp=.o)
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(OBJECTS) $(LINKS)

%.o : %.cpp %.h
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: clean all
	./$(EXEC_NAME)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)
