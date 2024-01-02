TARGET := ginn
SRCDIR := src
OBJDIR := obj
BINDIR := bin

EXE := $(BINDIR)/$(TARGET)
SRC := $(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/main.c)
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o, $(SRC))
SRCDIRS := $(sort $(dir $(SRC)))
OBJDIRS := $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCDIRS))

CFLAGS  := -Wall -Wextra -O2

ARGS    :=

$(TARGET): $(EXE)
	cp $(BINDIR)/$(TARGET) $(TARGET)

$(EXE): $(OBJ) | $(BINDIR)
	$(CC) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIRS) $(BINDIR):
	mkdir -p $@

run: $(TARGET)
	@$(BINDIR)/$(TARGET) $(ARGS)

clean:
	@$(RM) -rv $(BINDIR) $(OBJDIR)
	@$(RM) -rv $(TARGET)

.PHONY: all
