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
CPPFLAGS := -std=c++14
LIB := -lpthread -L$(MBEDDIR)/library -lmbedcrypto
INC := -I include -I $(MBEDDIR)/include

# Set OS Specific flags
ifeq ($(UNAME_S),Darwin) # OS X
	CPPFLAGS += -stdlib=libc++ -ObjC++
	LIB += -lc++ -framework Foundation
else # Linux
	CPPFLAGS += -stdlib=libstdc++
	LIB += -lstdc++ -lssl -lcrypto -lcrypt
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

install:
	/bin/mkdir -p /usr/local/bin
	/bin/mkdir -p /usr/local/share/man/man1
	/bin/mv dave /usr/local/bin/dave
	/bin/cp docs/dave.1 /usr/local/share/man/man1/dave.1
	@echo "Installed!"

uninstall:
	/bin/rm -f /usr/local/share/man/man1/dave.1
	/bin/rm -f /usr/local/bin/dave
	@echo "Uninstalled!"

.PHONY: clean test $(MBEDTLS) install uninstall
