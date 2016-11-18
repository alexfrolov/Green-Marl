GM_TOP= /home/frolo/wdir/Green-Marl.mine
GM_GRAPH= ${GM_TOP}/apps/output_charm/gm_graph
BINDIR = ../bin
GENDIR = ../generated
CC= g++
CFLAGS = -g -O3 -I${GM_GRAPH}/inc -I${GENDIR} -I. 
LFLAGS = -L${GM_GRAPH}/lib 
include ${GM_TOP}/setup.mk

.SUFFIXES: 
.SUFFIXES: .decl.h .def.h .ci

all: ${BINDIR}/sssp

${BINDIR}/% : ${GENDIR}/%.o %_main.o 
	${CHARMC} $^ ${LFLAGS} -o $@

${GENDIR}/%.o : ${GENDIR}/%.C ${GENDIR}/%.decl.h
	${CHARMC} $< ${CFLAGS} -o $@

%.o : %.C %.decl.h
	${CHARMC} $< ${CFLAGS} -o $@

%.decl.h : %.ci
	${CHARMC} $<

clean:
	rm -f ${BINDIR}/*
	rm -f *.o *.decl.h *.def.h
	rm -f ${GENDIR}/*.o ${GENDIR}/*.decl.h ${GENDIR}/*.def.h
