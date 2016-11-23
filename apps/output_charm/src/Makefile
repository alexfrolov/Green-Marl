GM_TOP= /home/frolo/wdir/Green-Marl.mine
GM_GRAPH= ${GM_TOP}/apps/output_charm/gm_graph
BINDIR = ../bin
GENDIR = ../generated
CC= g++
CFLAGS = -g -I${GM_GRAPH} -I${GM_GRAPH}/graph500 -I${GENDIR} -DHOOK_ENABLE -I. 
LFLAGS = -L${GM_GRAPH} -lgm_graph -L${BOOST_ROOT}/lib -lboost_system -lboost_timer -lboost_chrono
include ${GM_TOP}/setup.mk

.SUFFIXES: 
.SUFFIXES: .decl.h .def.h .xi .ci

all: ${BINDIR}/sssp ${BINDIR}/avg_teen_cnt

${BINDIR}/% : %_main.o 
	${CHARMC} $^ ${LFLAGS} -o $@

#${BINDIR}/% : ${GENDIR}/%.o %_main.o 
#	${CHARMC} $^ ${LFLAGS} -o $@

#${GENDIR}/%.o : ${GENDIR}/%.C ${GENDIR}/%.decl.h
#	${CHARMC} $< ${CFLAGS} -o $@

%_main.o : %_main.C %_main.decl.h ${GENDIR}/%.decl.h
	${CHARMC} $< ${CFLAGS} -o $@

#%.o : %.C %.decl.h 
#	${CHARMC} $< ${CFLAGS} -o $@

%.xi : %.ci
	gcc -x c++ -E $(CFLAGS) -P $< > $@

%.decl.h : %.xi
	${CHARMXI} $<
	sed -i 's/CkIndex_graphlib__/graphlib::CkIndex_/g' $(notdir $(basename $<)).decl.h $(notdir $(basename $<)).def.h
	sed -i 's/graphlib__/graphlib::/g' $(notdir $(basename $<)).decl.h $(notdir $(basename $<)).def.h


clean:
	rm -f charmrun ${BINDIR}/*
	rm -f *.o *.xi *.decl.h *.def.h
	rm -f ${GENDIR}/*.o ${GENDIR}/*.xi ${GENDIR}/*.decl.h ${GENDIR}/*.def.h
