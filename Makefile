LIBPATH += -L"/System/Library/Frameworks/OpenGL.framework/Libraries"

FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL

COMPILERFLAGS = -Wall -L/sw/lib -I/sw/include
CC = g++ 
CFLAGS = $(COMPILERFLAGS)
CPPFLAGS = $(COMPILERFLAGS)
LIBRARIES = -lGL -lGLU -lm -lobjc -lstdc++ -lpng -ljpeg
SRCS = Sparkletts.cpp Scope.cpp KS_VECTOR.cpp LoadJPEGMask.cpp Snapshot.cpp scopeData.cpp Doublebuffer.cpp KaleidoUtils.cpp
OBJECTS = $(SRCS:.cpp=.o)
All: Sparkletts 

Sparkletts: $(OBJECTS)
	$(CC) $(FRAMEWORK) $(CFLAGS) -o $@ $(LIBPATH) $(OBJECTS) $(LIBRARIES)

