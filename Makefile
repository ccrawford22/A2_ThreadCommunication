# CS480 - Spring 2023
# Cody Crawford
# RedID:824167663
# Program 2

## MacOS VS Code installation tutorials
# https://eecs280staff.github.io/tutorials/setup_vscode.html 

# CXX Make variable for compiler
CXX=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = dictionary.o thread_manager.o

# Program name
PROGRAM = countprefix

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS)
	$(CXX) -pthread -o $(PROGRAM) $^

dictionary.o : dictionary.h dictionary.cpp
	$(CXX) $(CXXFLAGS) dictionary.cpp
	
thread_manager.o : dictionary.o countprefix.h populatetree.h readprefix.h thread_manager.cpp
	$(CXX) $(CXXFLAGS) thread_manager.cpp

clean :
	rm -f *.o *.exe $(PROGRAM)

