# Attempt to include platform specific makefile.
# OSNAME may be passed in.
OSNAME ?= $(shell uname -s | sed -e 's/[-_].*//g')
OSNAME := $(shell echo $(OSNAME) | tr A-Z a-z)
-include config/Makefile.$(OSNAME)

# Default paths.
DESTDIR ?=
LOCALBASE ?= /usr/local
BINDIR ?= ${LOCALBASE}/bin
LIBDIR ?= ${LOCALBASE}/lib
INCDIR ?= ${LOCALBASE}/include
MANDIR ?= $(LOCALBASE)/share/man

BUILDVERSION=$(shell sh ${CURDIR}/buildver.sh)
ifneq ("${BUILDVERSION}", "")
CPPFLAGS+= -DBUILDSTR=\"$(BUILDVERSION)\"
endif

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
INCFLAGS += -I $(INCDIR)/clens -I . -I $(INCDIR)
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
	$(INSTALL) -m 0755 -d $(DESTDIR)$(LIBDIR)/
ifeq ($(OSNAME),cygwin)
	$(INSTALL) -m 0755 -d $(DESTDIR)$(BINDIR)/
	$(INSTALL) -m 0755 $(OBJPREFIX)$(LIB.SHARED) $(DESTDIR)$(BINDIR)/
	$(INSTALL) -m 0644 $(OBJPREFIX)$(LIB.DEVLNK) $(DESTDIR)$(LIBDIR)/
else
	$(INSTALL) -m 0644 $(OBJPREFIX)$(LIB.SHARED) $(DESTDIR)$(LIBDIR)/
	$(LN) $(LNFLAGS) $(LIB.SHARED) $(DESTDIR)$(LIBDIR)/$(LIB.SONAME)
	$(LN) $(LNFLAGS) $(LIB.SHARED) $(DESTDIR)$(LIBDIR)/$(LIB.DEVLNK)
endif
	$(INSTALL) -m 0644 $(OBJPREFIX)$(LIB.STATIC) $(DESTDIR)$(LIBDIR)/
	$(INSTALL) -m 0755 -d $(DESTDIR)$(INCDIR)/
	$(INSTALL) -m 0644 $(LIB.HEADERS) $(DESTDIR)$(INCDIR)/
	$(INSTALL) -m 0755 -d $(addprefix $(DESTDIR)$(MANDIR)/, $(LIB.MDIRS))
	$(foreach page, $(LIB.MANPAGES), \
		$(INSTALL) -m 0444 $(page) $(addprefix $(DESTDIR)$(MANDIR)/, \
		$(subst ., man, $(suffix $(page))))/; \
	)

uninstall:
	$(RM) $(DESTDIR)$(LIBDIR)/$(LIB.DEVLNK)
ifeq ($(OSNAME),cygwin)
	$(RM) $(DESTDIR)$(BINDIR)/$(LIB.SHARED)
else
	$(RM) $(DESTDIR)$(LIBDIR)/$(LIB.SONAME)
	$(RM) $(DESTDIR)$(LIBDIR)/$(LIB.SHARED)
endif
	$(RM) $(DESTDIR)$(LIBDIR)/$(LIB.STATIC)
	$(RM) $(addprefix $(DESTDIR)$(INCDIR)/, $(LIB.HEADERS))
	$(foreach page, $(LIB.MANPAGES), \
		$(RM) $(addprefix $(DESTDIR)$(MANDIR)/, \
		$(subst ., man, $(suffix $(page))))/$(page); \
	)

clean:
	$(RM) $(LIB.SOBJS)
	$(RM) $(OBJPREFIX)$(LIB.SHARED)
	$(RM) $(OBJPREFIX)$(LIB.SONAME)
	$(RM) $(OBJPREFIX)$(LIB.DEVLNK)
	$(RM) $(LIB.OBJS)
	$(RM) $(OBJPREFIX)$(LIB.STATIC)
	$(RM) $(LIB.DEPS)

-include $(LIB.DEPS)

.PHONY: clean depend install uninstall
