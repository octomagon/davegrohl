# Thanks to Hilton Lipschitz for this makefile!

CC := clang++
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
TARGET := dave
JOYMESG := "\nMake succeeded...\nCreated: $(TARGET)"

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -std=c++11 -stdlib=libc++ -fobjc-arc -fobjc-nonfragile-abi -arch x86_64 -ObjC++ -O3 
LIB := -L $(LIBDIR) -framework Foundation
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB);
	@echo $(JOYMESG)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
