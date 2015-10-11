#
# --- Configuration ---
#

TARGET  = arena1
CC      = gcc

CFLAGS  = -Wall
CFLAGS += $(shell pkg-config --cflags sdl)

LFLAGS  = -Wall -lSDL_ttf -lSDL_image -lSDL_mixer
LFLAGS += $(shell pkg-config --libs sdl)

#
# --- Directories ---
#

SRCDIR  = src
OBJDIR  = obj
RESDIR  = res
BINDIR  = bin

CFLAGS += -I$(SRCDIR)

#
# --- Generate File Names ---
#

SOURCES   := $(shell find $(SRCDIR) -name "*.c" | cut -sd / -f 2-)
RESOURCES := $(shell find $(RESDIR) -type f | cut -sd / -f 2-)

SRC_P := $(foreach var,$(SOURCES),$(SRCDIR)/$(var))
OBJ_P := $(foreach var,$(SOURCES),$(OBJDIR)/$(patsubst %.c,%.o,$(var)))
RES_P := $(foreach var,$(RESOURCES),$(BINDIR)/$(var))

undefine SOURCES
undefine RESOURCES

#
# --- Default ---
#

.PHONY: all
all: $(BINDIR)/$(TARGET) $(RES_P)

#
# --- Linking ---
#

$(BINDIR)/$(TARGET): $(OBJ_P)
	@mkdir -p $(BINDIR)
	$(CC) $(LFLAGS) -o $(BINDIR)/$(TARGET) $(OBJ_P)

#
# --- Compiling ---
#

-include $(OBJ_P:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@
	@echo -n $(dir $@) > $(patsubst %.o,%.d,$@)
	@$(CC) -MM $(CFLAGS) $< >> $(patsubst %.o,%.d,$@)

#
# --- Resources ---
#

$(BINDIR)/%: $(RESDIR)/%
	@mkdir -p $(dir $@)
	cp $< $@

#
# --- Phony Targets ---
#

.PHONY: clean
clean:
	rm -rf $(BINDIR) $(OBJDIR) 
