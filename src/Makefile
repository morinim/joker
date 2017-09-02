# Unix makefile per Joker++.

###############################################
###### Sezione modificabile dall'utente. ######
###############################################

# Valori accettabili per CPU: 
#   0 =        i386
#   1 =        i486
#   2 =    iPentium
#   3 =  iPentiumII
#   4 =       amdK6
#   5 = iPentiumIII
#   6 =   amdAthlon
ID_CPU = 6

# Valori accettabili per SISTEMA:
#   0 =             unix
#   1 = windows 95/98/ME
SISTEMA = 0

# Impostazione dei nomi dei file
STANDARD_BOOK    = \"opening.dat\"
STANDARD_LOG     =     \"log.txt\"
STANDARD_WEIGHTS = \"weights.dat\"

# Impostazioni che riguardano la tecnica di ricerca:
ASPIRATION_SEARCH =  true
ASPIRATION_WINDOW =    25
ASSEMBLER         =  true
HASH_BIT          =    20
LAZY_EVAL         =  true
MATERIAL_DRAW     =  true
NULL_MOVE         =  true
E_CACHE_BIT       =    16
VALIDATION_SEARCH = false

# Valori accettabili per COMPILAZIONE:
#   0 =   release
#   1 =     debug
#   2 = profiling
ifndef COMPILAZIONE
COMPILAZIONE = 0
endif



###################################################
###### Sezione NON MODIFICABILE dall'utente. ######
###################################################
export CXX   = g++ -pipe
export SHELL = /bin/sh
export WARN  = -ansi -Wall -W -Wstrict-prototypes
export DEFS  = -$(WARN) -I./ -DVERSIONE=\"0.7.7\"

vpath %.cc ./test

ifeq ($(ID_CPU),0)
DEFS += -DMICROPROCESSORE=M_i386 -march=i386 -DNODI_FRA_CONTROLLI=300
endif
ifeq ($(ID_CPU),1)
DEFS += -DMICROPROCESSORE=M_i486 -march=i486 -DNODI_FRA_CONTROLLI=500
endif
ifeq ($(ID_CPU),2)
DEFS += -DMICROPROCESSORE=M_iPentium -march=pentium -DNODI_FRA_CONTROLLI=1000
endif
ifeq ($(ID_CPU),3)
DEFS += -DMICROPROCESSORE=M_iPentiumII -march=i686 -DNODI_FRA_CONTROLLI=2000
endif
ifeq ($(ID_CPU),4)
DEFS += -DMICROPROCESSORE=M_amdK6 -march=k6 -DNODI_FRA_CONTROLLI=1000
endif
ifeq ($(ID_CPU),5)
DEFS += -DMICROPROCESSORE=M_iPentiumIII -march=i686 -DNODI_FRA_CONTROLLI=2000
endif
ifeq ($(ID_CPU),6)
DEFS += -DMICROPROCESSORE=M_amdAthlon -march=athlon -DNODI_FRA_CONTROLLI=2000
endif

ifeq ($(SISTEMA),0)
DEFS += -DUNIX
endif
ifeq ($(SISTEMA),1)
DEFS += -DWIN32
endif

DEFS += -DSTANDARD_BOOK=$(STANDARD_BOOK)
DEFS += -DSTANDARD_LOG=$(STANDARD_LOG)
DEFS += -DSTANDARD_WEIGHTS=$(STANDARD_WEIGHTS)

ifeq ($(ASPIRATION_SEARCH),true)
DEFS += -DASPIRATION_SEARCH -DASPIRATION_WINDOW=$(ASPIRATION_WINDOW)
endif
ifeq ($(ASSEMBLER),true)
DEFS += -DASSEMBLER
endif
DEFS += -DHASH_BIT=$(HASH_BIT)
ifeq ($(LAZY_EVAL),true)
DEFS += -DLAZY_EVAL
endif
ifeq ($(MATERIAL_DRAW),true)
DEFS += -DMATERIAL_DRAW
endif
ifeq ($(NULL_MOVE),true)
DEFS += -DNULL_MOVE
endif
DEFS += -DE_CACHE_BIT=$(E_CACHE_BIT)
ifeq ($(VALIDATION_SEARCH),true)
DEFS += -DVALIDATION_SEARCH
endif

ifeq ($(COMPILAZIONE),0)
DEFS += -DNDEBUG -O2 -s -fstrict-aliasing -fomit-frame-pointer \
        -finline-limit=20000 -malign-double
endif
ifeq ($(COMPILAZIONE),1)
DEFS += -D_DEBUG -O0 -g
endif
ifeq ($(COMPILAZIONE),2)
DEFS += -DNDEBUG -fprofile-arcs -ftest-coverage
endif

DEFS += -fno-rtti



GENMOD   = bitboard.o chiave_hash.o componenti_valutazione.o direzione.o    \
           fase.o giocatore.o info_aus.o inizializza.o interfaccia.o itos.o \
           libro_apertura.o lista_mosse.o logger.o mossa.o pesi.o pezzo.o   \
           posizione.o random.o san.o scacchiera.o secondi.o situazione.o   \
           situazione_valutata.o supporto_ricerca.o test.o trasposizioni.o  \
           valutazione.o
OTHERMOD = gestore_tempo.o get_opt.o main.o partita.o
TESTMOD  = t_gm.o t_ricerca.o t_vgm.o
TEST     = test_gm test_ricerca test_vgm

joker: $(GENMOD) $(OTHERMOD)
	$(CXX) $(DEFS) -o joker $(GENMOD) $(OTHERMOD)

.PHONY: jbook
jbook:
	cd jbook && $(MAKE) -w

.PHONY: jstat
jstat:
	cd jstat && $(MAKE) -w

test_%: t_%.o $(GENMOD)
	$(CXX) $(DEFS) -o $@ $< $(GENMOD)
	#@$@

%.d: %.cc
	@$(SHELL) -ec '$(CXX) $(DEFS) -MM $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

include $(GENMOD:.o=.d)
include $(OTHERMOD:.o=.d)
include $(TESTMOD:.o=.d)

%.o:
	$(CXX) $(DEFS) -c $< -o $@

.PHONY: backup
backup: clean
	@echo Creazione e compressione file di backup.
	@tar --exclude opening.dat -cvf - ../* | bzip2 -9 > ../../backup/Joker++.tar.bz2
	@echo Operazione terminata.

.PHONY: clean
clean:
	@echo Eliminazione spazzatura in corso.
	@find ../ | grep ".*~" | xargs rm -f -v
	@find | grep [.]o$ | xargs rm -f -v 
	@rm -f -v test_* crea_tab joker
	@rm -f -v gmon.out core $(STANDARD_LOG) *.d *.gcov *.da *.bb *.bbg
	@cd jbook && $(MAKE) -w clean
	@cd jstat && $(MAKE) -w clean
	@echo Operazione terminata.

.PHONY: copia_da_win
copia_da_win:
	@cp /mnt/win/Joker/src/*.[h|cc] .
	@cp /mnt/win/Joker/src/test/*.[h|cc] test
	@cp /mnt/win/Joker/src/jbook/*.[h|cc] jbook
	@cp /mnt/win/Joker/src/jstat/*.[h|cc] jstat
	@cp /mnt/win/Joker/src/crea_tabelle/*.[h|cc] crea_tabelle
	@cp /mnt/win/Joker/src/test/input/* test/input
	@cp /mnt/win/Joker/src/test/output/* test/output
	@cp -r /mnt/win/Joker/src/tabelle .
	@2unx *.[h|cc]                                   \
              crea_tabelle/*.[h|cc]                      \
              jbook/*.[h|cc] jstat/*.[h|cc] jtb/*.[h|cc] \
              test/*.[h|cc] tabelle/* test/output/* test/input/*

.PHONY: copia_su_win
copia_su_win: clean
	@su -c 'cp -r -v * /mnt/win/Joker/src/'