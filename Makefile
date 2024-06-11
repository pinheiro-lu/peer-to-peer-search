# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11

# Name of the output file
OUTPUT = ep_distsys

# List of source files
SOURCES = main.cpp functions.cpp SocketManager.cpp ConnectionManager.cpp KeyValueManager.cpp Message.cpp MessageHandler.cpp MessageSender.cpp NeighborManager.cpp SearchManager.cpp

# Default target
all: $(OUTPUT)

# Rule to link the program
$(OUTPUT): $(SOURCES:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files
%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to clean intermediate files
clean:
	rm -f *.o

# Rule to clean all files
distclean: clean
	rm -f $(OUTPUT)