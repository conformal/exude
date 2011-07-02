# $exude$

# Attempt to include platform specific makefile.
# OSNAME may be passed in.
OSNAME ?= $(shell uname -s)
OSNAME := $(shell echo $(OSNAME) | tr A-Z a-z)
-include config/Makefile.$(OSNAME)

# Default paths.
LOCALBASE ?= /usr/local
BINDIR ?= ${LOCALBASE}/bin
LIBDIR ?= ${LOCALBASE}/lib
INCDIR ?= ${LOCALBASE}/include
MANDIR ?= $(LOCALBASE)/share/man

# Use obj directory if it exists.
OBJPREFIX ?= obj/
ifeq "$(wildcard $(OBJPREFIX))" ""
	OBJPREFIX =	
endif

# Get shared library version.
-include shlib_version
SO_MAJOR = $(major)
SO_MINOR = $(minor)

# System utils.
AR ?= ar
CC ?= gcc
INSTALL ?= install
LN ?= ln
LNFLAGS ?= -sf
MKDIR ?= mkdir
RM ?= rm -f

# Compiler and linker flags.
CPPFLAGS += -DNEED_LIBCLENS
INCFLAGS += -I $(INCDIR)/clens
WARNFLAGS ?= -Wall -Werror
DEBUG += -g
CFLAGS += $(INCFLAGS) $(WARNFLAGS) $(DEBUG)
LDFLAGS +=
SHARED_OBJ_EXT ?= o

LIB.NAME = exude
LIB.SRCS = exude.c
LIB.HEADERS = exude.h
LIB.MANPAGES = exude.3
LIB.OBJS = $(addprefix $(OBJPREFIX), $(LIB.SRCS:.c=.o))
LIB.SOBJS = $(addprefix $(OBJPREFIX), $(LIB.SRCS:.c=.$(SHARED_OBJ_EXT)))
LIB.DEPS = $(addsuffix .depend, $(LIB.OBJS))
ifneq "$(LIB.OBJS)" "$(LIB.SOBJS)"
	LIB.DEPS += $(addsuffix .depend, $(LIB.SOBJS))
endif
LIB.MDIRS = $(foreach page, $(LIB.MANPAGES), $(subst ., man, $(suffix $(page)))) 
LIB.LDFLAGS = $(LDFLAGS.EXTRA) $(LDFLAGS) 

all: $(OBJPREFIX)$(LIB.SHARED) $(OBJPREFIX)$(LIB.STATIC)

obj:
	-$(MKDIR) obj

$(OBJPREFIX)$(LIB.SHARED): $(LIB.SOBJS)
	$(CC) $(LDFLAGS.SO) $^ $(LIB.LDFLAGS) -o $@

$(OBJPREFIX)$(LIB.STATIC): $(LIB.OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJPREFIX)%.$(SHARED_OBJ_EXT): %.c
	@echo "Generating $@.depend"
	@$(CC) $(INCFLAGS) -MM $(CPPFLAGS) $< | \
	sed 's,$*\.o[ :]*,$@ $@.depend : ,g' > $@.depend
	$(CC) $(CFLAGS) $(PICFLAG) $(CPPFLAGS) -o $@ -c $<

$(OBJPREFIX)%.o: %.c
	@echo "Generating $@.depend"
	@$(CC) $(INCFLAGS) -MM $(CPPFLAGS) $< | \
	sed 's,$*\.o[ :]*,$@ $@.depend : ,g' >> $@.depend
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $< 

depend: 
	@echo "Dependencies are automatically generated.  This target is not necessary."	

install:
	$(INSTALL) -m 0644 $(OBJPREFIX)$(LIB.SHARED) $(LIBDIR)/
	$(LN) $(LNFLAGS) $(LIB.SHARED) $(LIBDIR)/$(LIB.SONAME)
	$(LN) $(LNFLAGS) $(LIB.SHARED) $(LIBDIR)/$(LIB.DEVLNK)
	$(INSTALL) -m 0644 $(OBJPREFIX)$(LIB.STATIC) $(LIBDIR)/
	$(INSTALL) -m 0644 $(LIB.HEADERS) $(INCDIR)/
	$(INSTALL) -m 0755 -d $(addprefix $(MANDIR)/, $(LIB.MDIRS))
	$(foreach page, $(LIB.MANPAGES), \
		$(INSTALL) -m 0444 $(page) $(addprefix $(MANDIR)/, \
		$(subst ., man, $(suffix $(page))))/; \
	)
	
uninstall:
	$(RM) $(LIBDIR)/$(LIB.DEVLNK)
	$(RM) $(LIBDIR)/$(LIB.SONAME)
	$(RM) $(LIBDIR)/$(LIB.SHARED)
	$(RM) $(LIBDIR)/$(LIB.STATIC)
	$(RM) $(addprefix $(INCDIR)/, $(LIB.HEADERS))
	$(foreach page, $(LIB.MANPAGES), \
		$(RM) $(addprefix $(MANDIR)/, \
		$(subst ., man, $(suffix $(page))))/$(page); \
	)
	
clean:
	$(RM) $(LIB.SOBJS)
	$(RM) $(OBJPREFIX)$(LIB.SHARED)
	$(RM) $(OBJPREFIX)/$(LIB.SONAME)
	$(RM) $(OBJPREFIX)/$(LIB.DEVLNK)
	$(RM) $(LIB.OBJS)
	$(RM) $(OBJPREFIX)$(LIB.STATIC)
	$(RM) $(LIB.DEPS)

-include $(LIB.DEPS)

.PHONY: clean depend install uninstall

