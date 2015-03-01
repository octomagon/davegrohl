# Thanks to Hilton Lipschitz for this makefile!

CC := clang++
SRCDIR := src
BUILDDIR := build
#TRGTDIR := bin
LIBDIR := lib
TARGET := dave
JOYMESG := "\nMake succeeded...\nCreated: $(TARGET)"

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7 -fobjc-arc -fobjc-nonfragile-abi -arch x86_64 -ObjC++ -O3 
LIB := -L $(LIBDIR) -framework Foundation
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB);
	@echo $(JOYMESG)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(LIBDIR)
	#@mkdir -p $(TRGTDIR)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
#tester:
#	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

## Spikes
#ticket:
#	$(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
