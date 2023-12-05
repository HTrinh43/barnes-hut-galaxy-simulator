# Define the C++ compiler to use
CXX = mpicxx

# Define any compile-time flags
CXXFLAGS = -Wall -g

# Define any directories containing header files other than /usr/include
INCLUDES =

# Define the source file directory
SRCDIR = src

# Define the C++ source files
SOURCES = $(SRCDIR)/mpi.cpp $(SRCDIR)/body.cpp $(SRCDIR)/io.cpp $(SRCDIR)/node.cpp

# Define the C++ object files
OBJECTS = $(SOURCES:.cpp=.o)

# Define the executable file
MAIN = nbody

.PHONY: depend clean

all:    $(MAIN)
	@echo  Simple compiler named nbody has been compiled

# This is the main executable
$(MAIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJECTS)

# This is a suffix replacement rule for building .o's from .cpp's
# It uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(SRCDIR)/*.o *~ $(MAIN)

depend: $(SOURCES)
	makedepend $(INCLUDES) $^

# END
