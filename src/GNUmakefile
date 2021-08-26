# KVR GNU MAKEFILE

#   ---     ---     ---     ---     ---
# platform switch

ifdef x32
PLAT = x32
PFLG = -m32
else
PLAT = x64
PFLG = -m64
endif

#   ---     ---     ---     ---     ---

.PHONY: kvrnel chasm espectro nrun

kvrnel:
	@cd $(CURDIR)/KVRNEL; make

chasm: kvrnel
	@cd $(CURDIR)/CHASM; make

espectro: | kvrnel chasm
	@cd $(CURDIR)/ESPECTRO; make

nrun: espectro
	@clear; ../bin/$(PLAT)/espectro

#   ---     ---     ---     ---     ---