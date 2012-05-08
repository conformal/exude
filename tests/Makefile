SYSTEM != uname -s
.if exists(${.CURDIR}/../config/Makefile.$(SYSTEM:L))
.  include "${.CURDIR}/../config/Makefile.$(SYSTEM:L)"
.endif

LOCALBASE?=/usr/local
BINDIR?=${LOCALBASE}/bin
LIBDIR?=${LOCALBASE}/lib
INCDIR?=${LOCALBASE}/include

PROG= exude_threadtest
NOMAN=
NO_MAN=

.if exists(${.CURDIR}/../obj)
LDADD+=	-L${.CURDIR}/../obj
DPADD+= ${.CURDIR}/../obj/libexude.a
.else
LDADD+=	-L${.CURDIR}/..
DPADD+= ${.CURDIR}/../libexude.a
.endif

SRCS= exude_threadtest.c
COPT+= -O2
CFLAGS+= -Wall -Werror -g
LDADD+=-pthread -L${LOCALBASE}/lib -lclog -lexude

CFLAGS+= -I${.CURDIR}/..

.include <bsd.prog.mk>

