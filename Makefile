# Compiler options
CXX = mpic++
CXXFLAGS = -Wall -std=c++11

# Linker options
CC = mpic++

# Paths and variables
bin = bin
vpath %.h src
vpath %.cpp src
vpath %.o $(bin)

objects = main.o process.o task_generator.o professional.o packet.o

# Linking rule
$(bin)/main: $(objects)
	$(CC) $(LDFLAGS) $(addprefix $(bin)/,$(objects)) $(LOADLIBES) $(LDLIBS) -o $@ 

# Compilation rule
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(bin)/$@ $<

# Header files 
main.o: process.h task_generator.h professional.h packet.h logging.h

process.o: process.h packet.h

task_generator.o: task_generator.h process.h packet.h logging.h

professional.o: professional.h process.h packet.h logging.h

packet.o: packet.h

# Make sure directory is created
$(objects): | $(bin)

$(bin):
	mkdir $(bin)

.PHONY: clean
clean:
	rm -r $(bin)
