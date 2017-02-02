
# stop after parse and typecheck
ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=charm

SRCS_CHARM = #t1.gm t2.gm t3.gm
SRCS_GPS = gps1.gm gps2.gm #gps3.gm gps4.gm gps5.gm gps6.gm gps7.gm gps8.gm gps9.gm gps10.gm gps11.gm gps12.gm gps13.gm gps14.gm gps15.gm gps16.gm gps17.gm gps18.gm

OUTS_CHARM = $(addprefix  $(ODIR)/charm_, $(SRCS_CHARM:.gm=.C))
OUTS_GPS = $(addprefix $(ODIR)/gps_, $(SRCS_GPS:.gm=.C)) 
all: $(OUTS_CHARM) $(OUTS_GPS)

$(ODIR)/charm_%.C: %.gm
	$(COMP) $(FLAGS) $< 

$(ODIR)/gps_%.C: ../gps/%.gm
	$(COMP) $(FLAGS) $< 

$(ODIR)/result_%.gm: ../gps/%.gm
	echo $(COMP) $(FLAGS) -DS=5 -V=1 $< > $@ 

clean: 
	\rm -rf $(ODIR)/*.C *.C $(ODIR)/*.gm
	\rm -rf $(ODIR)/*.ci *.ci 
	\rm -rf $(ODIR)/*.mk
