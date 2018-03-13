COMPILER = c++
ADD_CFLAGS = -Wall -std=c++11 -O0 -g -pthread
NO_PKG_LIBS = -lBox2D -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system -lpthread
CXXFLAGS = $(ADD_CFLAGS)
LINKS = $(NO_PKG_LIBS)
SOURCES = $(wildcard src/*.cpp)
EXEC_NAME = openSP
OBJECTS = $(SOURCES:.cpp=.o)
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run


all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(OBJECTS) $(LINKS)

up:
	git commit -am "Autocommit" && git push

%.o : %.cpp %.h
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: clean all
	./$(EXEC_NAME)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)
