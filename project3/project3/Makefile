COMPILING = example_teapot.cpp InitShader.cpp loadModels.cpp

#Specifies the executable's name
EXECUTABLE = runnable

#Specifies the compilier we will be using
CC = g++

#Specifies the compiler flags to be used
# -g3 adds debugging symbols
# -Wall shows all warnings
CFLAGS = -g3 -Wall -Wno-non-template-friend -fpermissive

#Specifies the locations and libraries needed to compile the executable

INCLUDE =  -I"../include" 

LIBRARY_PATH = -L"/usr/X11/lib" -L"../lib"
LDLIBRARIES_STATIC =-Bstatic -lglut -lGLEW
LDLIBRARIES_DYNAMIC =-Bdynamic -lGL -lm 

all:
	$(CC) $(CFLAGS) $(COMPILING) -o $(EXECUTABLE) $(INCLUDE) $(LIBRARY_PATH) $(LDLIBRARIES_STATIC) $(LDLIBRARIES_DYNAMIC)

clean:
	rm $(EXECUTABLE)


