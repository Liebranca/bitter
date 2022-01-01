# AVTOMAT GNU MAKEFILE

#   ---     ---     ---     ---     ---
# module data

FSWAT     = AVTOMAT
MKWAT     = AVTO
NOLINK    = 1

-include ../mkvars

LMODE     = 

LFLG     += 
LIBS     += 

-include ./mkavto

#   ---     ---     ---     ---     ---

.PHONY: all clean

all: | $(AVTO_TRSH)
	@echo -e "\e[38;2;176;176;0m[AR-AVTOMAT]\e[0m\n"

$(TRSH)/%: %.pl
	@echo "Regenerated $<"
	@./$<
	@touch $@

clean:
	@rm -rf $(TRSH)/*

#   ---     ---     ---     ---     ---
