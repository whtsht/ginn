TARGET   := ginn
SRCDIR   := src
OBJDIR   := obj
BINDIR   := bin
TESTDIR  := tests

EXE      := $(BINDIR)/$(TARGET)
SRC      := $(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/main.c)
TEST     := $(wildcard $(TESTDIR)/**/*.c $(TESTDIR)/*.c)
SRC_OBJ  := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o, $(SRC))
TEST_OBJ := $(patsubst $(TESTDIR)/%.c,$(OBJDIR)/$(TESTDIR)/%.o, $(TEST)) $(SRC_OBJ)
TEST_OBJ := $(filter-out $(OBJDIR)/main.o, $(TEST_OBJ))

TEST_BIN := $(BINDIR)/__test

SRCDIRS  := $(sort $(dir $(SRC)))
TESTDIRS := $(sort $(dir $(TEST)))
OBJDIRS  := $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCDIRS))
OBJDIRS  += $(patsubst $(TESTDIR)%,$(OBJDIR)/$(TESTDIR)%,$(TESTDIRS))

CFLAGS   := -Wall -Wextra -O2
LIBS     :=
TESTLIBS := -lcunit

ARGS     :=

$(TARGET): $(EXE)
	cp $(BINDIR)/$(TARGET) $(TARGET)

$(EXE): $(SRC_OBJ) | $(BINDIR)
	$(CC) $^ -o $@ $(LIBS)

test: $(TEST_BIN)
	@$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) | $(BINDIR)
	$(CC) $^ -o $@ $(TESTLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.c | $(OBJDIRS)
	$(CC) -DTEST $(CFLAGS) -c $< -o $@

$(OBJDIRS) $(BINDIR):
	mkdir -p $@

run: $(TARGET)
	@$(BINDIR)/$(TARGET) $(ARGS)

clean:
	$(RM) -rv $(BINDIR) $(OBJDIR)
	$(RM) -rv $(TARGET)

.PHONY: all
