SYSTEM != uname -s
.if exists(${.CURDIR}/config/Makefile.$(SYSTEM:L))
.  include "${.CURDIR}/config/Makefile.$(SYSTEM:L)"
.endif

LOCALBASE?=/usr/local
BINDIR?=${LOCALBASE}/bin
LIBDIR?=${LOCALBASE}/lib
INCDIR?=${LOCALBASE}/include

#WANTLINT=
LIB= exude
SRCS= exude.c
HDRS= exude.h
MAN= exude.3

BUILDVERSION != sh "${.CURDIR}/buildver.sh"

DEBUG+= -g
CFLAGS+= -Wall -Werror
CFLAGS+= -I${INCDIR} -I${.CURDIR}
.if !${BUILDVERSION} == ""
CPPFLAGS+= -DBUILDSTR=\"$(BUILDVERSION)\"
.endif
CLEANFILES+= exude.cat3

afterinstall:
	@cd ${.CURDIR}; for i in ${HDRS}; do \
	cmp -s $$i ${DESTDIR}${LOCALBASE}/include/$$i || \
	${INSTALL} ${INSTALL_COPY} -m 444 -o $(BINOWN) -g $(BINGRP) $$i ${DESTDIR}${LOCALBASE}/include; \
	echo ${INSTALL} ${INSTALL_COPY} -m 444 -o $(BINOWN) -g $(BINGRP) $$i ${DESTDIR}${LOCALBASE}/include;\
	done

uninstall:
	@for i in ${HDRS}; do \
	echo rm -f ${INCDIR}/$$i; \
	rm -f ${INCDIR}/$$i; \
	done

	@for i in $(_LIBS); do \
	echo rm -f ${LIBDIR}/$$i; \
	rm -f ${LIBDIR}/$$i; \
	done

.include <bsd.lib.mk>
