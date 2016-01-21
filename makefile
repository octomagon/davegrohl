CC := clang++
TARGET := dave
UNAME_S := $(shell uname -s)

# Dave
SRCEXT := cpp
SRCDIR := src
BUILDDIR := build
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# mbedTLS
SRCEXT2 := c
SRCDIR2 := src/mbedtls
BUILDDIR2 := build/mbedtls
SOURCES2 := $(wildcard $(SRCDIR2)/*.$(SRCEXT2))
OBJECTS2 := $(patsubst $(SRCDIR2)/%,$(BUILDDIR2)/%,$(SOURCES2:.$(SRCEXT2)=.o))

CFLAGS :=
CPPFLAGS := -std=c++14 -stdlib=libc++
LIB := -lc++ -lpthread
INC := -I include

# Set OS Specific flags
ifeq ($(UNAME_S),Darwin) # OS X
	CPPFLAGS += -ObjC++
	LIB += -framework Foundation
else # Linux
	LIB += -lssl -lcrypto -lcrypt
endif

# Linking
$(TARGET): $(OBJECTS) $(OBJECTS2)
	@echo " Linking..."
	$(CC) $^ -o $(TARGET) $(LIB)
	@printf "\nMake succeeded... Created: '$(TARGET)'\n"

# Compiles dave specific code
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(INC) -c -o $@ $<

# Compiles mbedTLS which is in C
$(BUILDDIR2)/%.o: $(SRCDIR2)/%.$(SRCEXT2)
	@mkdir -p $(BUILDDIR2)
	$(CC) -x c $(CFLAGS) $(INC) -c -o $@ $<

debug: CPPFLAGS += -g -Wall
debug: CFLAGS += -g -Wall
debug: $(TARGET)

test:
	@echo "\nTesting $(TARGET)..."
	@./test/testDave.sh

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(TARGET)

dev: clean debug test

print:
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"
	@echo "SOURCES2: $(SOURCES2)"
	@echo "OBJECTS2: $(OBJECTS2)"

.PHONY: clean test
