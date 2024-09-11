# See LICENSE file for copyright and license details.

include config.mk

NAME = poll4input

SRC = ${NAME}.c
OBJ = ${NAME}

all: ${NAME}

remake: clean all

${NAME}: ${SRC}
	${CC} -o $@ ${SRC} ${CFLAGS}

clean:
	rm -f ${OBJ} ${NAME}-${VERSION}.tar.gz

dist: clean
	mkdir -p ${NAME}-${VERSION}
	cp -R LICENSE Makefile README ${NAME}.1 ${SRC} ${NAME}-${VERSION}
	tar -cf ${NAME}-${VERSION}.tar ${NAME}-${VERSION}
	gzip ${NAME}-${VERSION}.tar
	rm -rf ${NAME}-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < ${NAME}.1 > ${DESTDIR}${MANPREFIX}/man1/${NAME}.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${NAME}.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}\
		${DESTDIR}${MANPREFIX}/man1/${NAME}.1

.PHONY: all clean remake dist install uninstall
