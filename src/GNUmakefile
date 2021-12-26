# KVR GNU MAKEFILE

#   ---     ---     ---     ---     ---

.PHONY: kvrnel mamm sin chasm utils shb7 nrun reb clean all

kvrnel:
	@$(MAKE) -C AVTOMAT all
	@$(MAKE) -C KVRNEL

mamm: kvrnel
	@$(MAKE) -C MAMMOTH

sin: kvrnel
	@$(MAKE) -C SIN

chasm: sin
	@$(MAKE) -C CHASM

utils:
	@$(MAKE) -C UTILS

shb7: chasm utils
	@$(MAKE) -C SHB7

nrun: shb7
	@clear; ../bin/shb7

rrun:
	@../bin/shb7

clean:
	@echo Cleaning up...
	@echo kvrnel
	@$(MAKE) -C KVRNEL clean
	@echo mamm
	@$(MAKE) -C MAMMOTH clean
	@echo sin
	@$(MAKE) -C SIN clean
	@echo chasm
	@$(MAKE) -C CHASM clean
	@echo utils
	@$(MAKE) -C UTILS clean
	@echo shb7
	@$(MAKE) -C SHB7 clean
	@rm -f $(CURDIR)/AVTO

all: | kvrnel mamm sin chasm utils shb7
reb: | clean kvrnel mamm sin chasm utils shb7

#   ---     ---     ---     ---     ---