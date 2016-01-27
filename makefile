CC := clang++
TARGET := dave
MAKE := make --no-print-directory
UNAME_S := $(shell uname -s)

# Dave
SRCEXT := cpp
SRCDIR := src
BUILDDIR := build
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
MBEDDIR := mbedtls
MBEDTLS := $(MBEDDIR)/library/libmbedcrypto.a

CFLAGS :=
CPPFLAGS := -std=c++14 -stdlib=libc++
LIB := -lc++ -lpthread -L$(MBEDDIR)/library -lmbedcrypto
INC := -I include -I $(MBEDDIR)/include

# Set OS Specific flags
ifeq ($(UNAME_S),Darwin) # OS X
	CPPFLAGS += -ObjC++
	LIB += -framework Foundation
else # Linux
	LIB += -lssl -lcrypto -lcrypt
endif

# Linking
$(TARGET): $(OBJECTS) $(MBEDTLS)
	@echo "  Linking..."
	$(CC) $^ -o $(TARGET) $(LIB)
	@printf "\nMake succeeded... Created: '$(TARGET)'\n"

# Compiles dave specific code
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(INC) -c -o $@ $<

$(MBEDTLS):
	@$(MAKE) -C $(MBEDDIR)

debug: CPPFLAGS += -g -Wall
debug: CFLAGS += -g -Wall
debug: $(TARGET)

test:
	@echo "  Testing mbedTLS..."
	@$(MAKE) -C $(MBEDDIR) test && printf "\nmbedTLS testing passed!\n"
	@$(MAKE) -C tests

clean:
	@echo "  Cleaning..."; 
	@$(MAKE) -C $(MBEDDIR) clean
	$(RM) -r $(BUILDDIR) $(TARGET)

dev: clean debug test

print:
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"

.PHONY: clean test $(MBEDTLS)
