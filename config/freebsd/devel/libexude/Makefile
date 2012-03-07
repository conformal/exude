# New ports collection makefile for:    libexude
# Date created:         March 5, 2012
# Whom:                 Dale Rahn <drahn@conformal.com>
#
# $FreeBSD$
#

PORTNAME=       libexude
PORTVERSION=    0.6.0
CATEGORIES=     devel
MASTER_SITES=   https://opensource.conformal.com/snapshots/exude/ \
		LOCAL/bf
DISTNAME=       exude-${PORTVERSION}

MAINTAINER=     drahn@conformal.com
COMMENT=        Exude is a simple to use memory allocation debug library

BUILD_DEPENDS=  ${LOCALBASE}/lib/libclens.a:${PORTSDIR}/devel/clens
LIB_DEPENDS+=   clog.2:${PORTSDIR}/devel/libclog

LICENSE=        ISCL
LICENSE_COMB=   single

.include <bsd.port.mk>
