CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm
SRCDIR = .
OBJDIR = obj
BINDIR = build
INCLUDES = -I./
COMPILE = $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
LINK = $(CC) $(LDFLAGS) $^ -o $@
MKDIR_P = mkdir -p
SRCDIRS = $(SRCDIR) $(SRCDIR)/test

SRC := $(wildcard $(addsuffix /*.c,$(SRCDIRS)))
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
TARGET = $(BINDIR)/test_program_pnk_proc

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(MKDIR_P) $(BINDIR)
	$(LINK)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(MKDIR_P) $(dir $@)
	$(COMPILE)

test: $(TARGET)
	$<

clean:
	@rm -rf $(OBJDIR) $(BINDIR)
