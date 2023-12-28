TARGET := ginn
SRCDIR := src
OBJDIR := obj
BINDIR := bin

EXE := $(BINDIR)/$(TARGET)
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

CFLAGS  := -Wall -Wextra -O2

ARGS    :=

$(TARGET): $(EXE)
	cp $(BINDIR)/$(TARGET) $(TARGET)

$(EXE): $(OBJ) | $(BINDIR)
	$(CC) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR) $(OBJDIR):
	mkdir -p $@

run: $(TARGET)
	@$(BINDIR)/$(TARGET) $(ARGS)

clean:
	@$(RM) -rv $(BINDIR) $(OBJDIR)
	@$(RM) -rv $(TARGET)

.PHONY: all
