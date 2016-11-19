GM_TOP= /home/frolo/wdir/Green-Marl.mine
GM_GRAPH= ${GM_TOP}/apps/output_charm/gm_graph
CC= g++
CFLAGS = -g -O3 -I${GM_GRAPH}/inc -I. -Igraph500
LFLAGS = -L${GM_GRAPH}/lib
include ${GM_TOP}/setup.mk

all: libgm_graph.a

libgm_graph.a: gm_graph.o
	ar r libgm_graph.a gm_graph.o

gm_graph.o: gm_graph.C gm_graph.decl.h gm_graph.def.h
	${CHARMC} $< ${CFLAGS} -o $@

%.o : %.C %.decl.h
	${CHARMC} $< ${CFLAGS} -o $@

%.decl.h : %.ci
	${CHARMC} $<


clean:
	rm -f libgm_graph.a *.o *.def.h *.decl.h
