# Compiler options
CXX = mpic++
CXXFLAGS = -Wall

# Linker options
CC = mpic++

# Paths and variables
bin = bin
vpath %.h src
vpath %.cpp src
vpath %.o $(bin)

objects = main.o process.o task_generator.o

# Linking rule
$(bin)/main: $(objects)
	$(CC) $(LDFLAGS) $(addprefix $(bin)/,$(objects)) $(LOADLIBES) $(LDLIBS) -o $@ 

# Compilation rule
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(bin)/$@ $<

# Header files 
main.o: process.h task_generator.h

process.o: process.h

task_generator.o: task_generator.h process.h

# Make sure directory is created
$(objects): | $(bin)

$(bin):
	mkdir $(bin)

.PHONY: clean
clean:
	rm -r $(bin)
