
# stop after parse and typecheck
ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=charm
CHARMC = /home/frolo/wdir/charm-6.7.0/bin/charmc

SRCS_CHARM = t1.gm t2.gm t3.gm
SRCS_GPS = gps1.gm gps2.gm gps3.gm gps4.gm gps5.gm gps6.gm gps7.gm gps8.gm gps9.gm gps10.gm gps11.gm gps12.gm gps13.gm gps14.gm gps15.gm gps16.gm gps17.gm gps18.gm

OUTS_CHARM = $(addprefix  $(ODIR)/, $(SRCS_CHARM:.gm=.o))
OUTS_GPS = $(addprefix $(ODIR)/, $(SRCS_GPS:.gm=.o)) 
all: $(OUTS_CHARM) $(OUTS_GPS)

$(ODIR)/%.C: %.gm
	$(COMP) $(FLAGS) $< 

$(ODIR)/%.C: ../gps/%.gm
	$(COMP) $(FLAGS) $< 

$(ODIR)/%.o: ./output/%.C
	$(CHARMC) $(patsubst %.C, %.ci, $<)
	mv  $(notdir $(patsubst %.C, %.decl.h, $<)) ./output
	mv  $(notdir $(patsubst %.C, %.def.h, $<)) ./output
	$(CHARMC) -o $@ $< -Ioutput

clean: 
	\rm -rf $(ODIR)/*.C *.C $(ODIR)/*.gm
	\rm -rf $(ODIR)/*.ci $(ODIR)/*.decl.h $(ODIR)/*.def.h 
	\rm -rf $(ODIR)/*.o
	\rm -rf $(ODIR)/*.mk
