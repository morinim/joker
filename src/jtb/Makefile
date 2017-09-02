# Unix makefile per JTB.

###### Inizio configurazione del sistema. ######

export CXX   = g++ -pipe
export SHELL = /bin/sh

vpath %.cpp ../ ./

SISTEMA = -DUNIX
DEFS    = $(SISTEMA)

CPU     = -mcpu=k6 -march=k6

DEBUG   = -D_DEBUG -g
OPTIM   = -DNDEBUG -O2 $(CPU) -fomit-frame-pointer -freg-struct-return
PROFILE = -DNDEBUG -DFAST $(CPU) -fprofile-arcs -ftest-coverage

FLAGS   = -ansi -Wall -W -Wstrict-prototypes -I./ -I../ $(DEBUG) $(DEFS)



###### Fine configurazione sistema. ######

GENMOD   = chiave_hash.o colore.o componenti_valutazione.o direzione.o egtb.o\
	   fase.o indici.o info_aus.o itos.o lista_mosse.o lista_pezzi.o     \
	   log_file.o mossa.o pezzo.o posizione.o random.o  scacchiera.o     \
           secondi.o situazione.o valutazione.o
OTHERMOD = get_opt.o main.o

jtb: $(GENMOD) $(OTHERMOD)
	$(CXX) $(FLAGS) -o jtb $(GENMOD) $(OTHERMOD)

%.d: %.cpp
	@$(SHELL) -ec '$(CXX) $(FLAGS) -MM $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

include $(GENMOD:.o=.d)
include $(OTHERMOD:.o=.d)

egtb.o:
	$(CXX) $(DEFS) $(DEBUG) -I../ -c $< -o $@
%.o:
	$(CXX) $(FLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo Eliminazione spazzatura in corso.
	@find ../ | grep ".*~" | xargs rm -f -v
	@find | grep [.]o$ | xargs rm -f -v 
	@rm -f -v jtb gmon.out core log.txt *.d *.gcov *.da *.bb *.bbg
	@echo Operazione terminata.