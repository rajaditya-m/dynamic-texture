CXX = g++

CXXFLAGS =  -framework OpenGL -framework GLUT -lGLEW -w 

OPTFLAGS = -O1

DIR1 = includes

DIR2 = data_structures

INC = $(DIR1) $(DIR2)

INC_PARAMS = $(foreach d, $(INC), -I$d)

clean :
	rm -rf opengl.o 

build :
	$(CXX) InitShader.cpp main.cpp $(INC_PARAMS) $(CXXFLAGS) $(OPTFLAGS) -o opengl.o 

run :
	./opengl.o

all : clean build run

build_dbg :
	$(CXX) InitShader.cpp main.cpp $(INC_PARAMS) $(CXXFLAGS) $(OPTFLAGS) -o opengl.o -g

run_dbg :
	gdb opengl.o

dbg : clean build_dbg run_dbg
