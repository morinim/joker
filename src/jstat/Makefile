# Unix makefile per JStat.

vpath %.cc ../ ./

DEFS += -I../

GENMOD   = bitboard.o chiave_hash.o componenti_valutazione.o direzione.o      \
           fase.o info_aus.o inizializza.o interfaccia.o itos.o lista_mosse.o \
           logger.o mossa.o pesi.o pezzo.o posizione.o random.o scacchiera.o  \
           secondi.o situazione.o situazione_valutata.o valutazione.o
OTHERMOD = get_opt.o main.o

jstat: $(GENMOD) $(OTHERMOD)
	$(CXX) $(DEFS) -o jstat $(GENMOD) $(OTHERMOD)

%.d: %.cc
	@$(SHELL) -ec '$(CXX) $(DEFS) -MM $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

include $(GENMOD:.o=.d)
include $(OTHERMOD:.o=.d)

%.o:
	$(CXX) $(DEFS) -c $< -o $@

.PHONY: clean
clean:
	@echo Eliminazione spazzatura in corso.
	@find ../ | grep ".*~" | xargs rm -f -v
	@find | grep [.]o$ | xargs rm -f -v 
	@rm -f -v gmon.out core log.txt *.d *.gcov *.da *.bb *.bbg jstat
	@echo Operazione terminata.
