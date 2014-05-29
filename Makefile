QUIET    := @

# Redefining some compilation flags
CC       := gcc
CFLAGS   += -g3 -std=gnu99 -Wall -pedantic
CPPFLAGS += -I include
SED      := sed
RM       := rm -f
MV       := mv

# Source, header, and object files
program  := typing-assistant
sources  := $(wildcard src/*.c)
objects  := $(sources:src/%.c=%.o)

dependencies := $(sources:src/%.c=%.d)
dictionary   := large

# Lookup source/header files in the following directories
vpath %.c  src
vpath %.h  include
vpath %.gz dictionaries

.PHONY: all
all: $(program)

# Linking
$(program): $(objects)

# Compilation
$(objects): %.o: %.c


# Cleaning targets

.PHONY: clean
clean:
	$(RM) $(objects)
	$(RM) $(dependencies)

.PHONY: cleanall
cleanall: clean
	$(RM) $(program)


# Automatic Dependency Generation

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(dependencies)
endif

%.d: %.c
	$(QUIET) $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -MM $< |  \
	$(SED) 's,\($*.o\) *:,\1 $@: ,' > $@.tmp
	$(QUIET) $(MV) $@.tmp $@


# Unpacking dictionary

.INTERMEDIATE: $(dictionary)
$(dictionary): %: %.gz
	gunzip -c $< > $@

install-dict: $(dictionary)
	install $< $(HOME)/.dict
