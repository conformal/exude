# $OpenBSD: Makefile,v 1.2 2011/07/04 16:40:21 dhill Exp $

COMMENT=	memory allocation debugger library

DISTNAME=	exude-0.7.3
EPOCh=		0
PKGNAME=	lib${DISTNAME}
CATEGORIES=    	devel
SHARED_LIBS=	exude	3.1

HOMEPAGE=	http://opensource.conformal.com/wiki/Exude	
MASTER_SITES=	http://opensource.conformal.com/snapshots/exude/

MAINTAINER=	Conformal Systems LLC <info@conformal.com>

NO_TEST=	Yes

# ISC
PERMIT_PACKAGE_CDROM=	Yes

BUILD_DEPENDS=	devel/libclog

FAKE_FLAGS=	MANDIR=${PREFIX}/man/man

do-configure:
	printf "major=${LIBexude_VERSION:R}\nminor=${LIBexude_VERSION:E}\n" \
            > ${WRKSRC}/shlib_version

.include <bsd.port.mk>
