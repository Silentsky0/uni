TARGET = main

CC = gcc
CFLAGS = -Wall -I.

LINKER   = gcc
LFLAGS   = -Wall -I. -lm

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

rm = rm -f

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "linking done, executable "$(BINDIR)/$(TARGET)" is ready"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo $<" has been compiled successfully"
	@echo ""

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "done cleaning up"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "executable removed"


