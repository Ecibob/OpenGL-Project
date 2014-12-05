CC = g++
SRC = project.cpp 
LIBS = -lGL -lGLU -lglut
EXEC = project

all:
	$(CC) $(SRC) -o $(EXEC) $(LIBS)

clean:
	rm -rf $(EXEC) *~