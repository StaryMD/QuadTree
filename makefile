CFLAGS = -Wall -lsfml-graphics -lsfml-window -lsfml-system -I header

# don't touch or I'll steal your kneecaps
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
VPATH = src

# black magic
%.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS)

runme: $(OBJS)
	g++ -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f src/*.o
	rm -f runme
	rm -f runme.exe
