# UTILS GNU MAKEFILE

#   ---     ---     ---     ---     ---
# module data

FSWAT     = UTILS

OUTL      = 
OUTF      = spwn msafr j3l cal

DIRS      = $(shell find $(BASE_DIR) -type d)
SRCS      = $(wildcard $(DIRS:%=%/*.c))
HEDS      = $(wildcard $(DIRS:%=%/*.h))
BIN       = $(dir $(abspath ../..)/bin/)

TRSH      = $(dir $(abspath ../..)/trashcan/$(FSWAT)/)

OBJS      = $(patsubst %.c,$(TRSH)/%.o,$(SRCS))
LOBJS     = $(patsubst %.h,$(TRSH)/%.lo,$(HEDS))
DEPS      = $(subst %.o,%.d,$(OBJS))
ODIR      = $(patsubst %,$(TRSH)/%,$(SRCS))
LDIR      = $(patsubst %,$(TRSH)/%,$(HEDS))

INCLUDES  = -I$(dir $(realpath $(firstword $(MAKEFILE_LIST))))

INCLUDES += -I$(dir $(abspath ..)/)
LIBS      = -L$(BIN) -lkvrnel

#   ---     ---     ---     ---     ---

.PHONY: $(OUTF) $(OUTL) MAIN

MAIN: $(OUTF) $(OUTL)
	@true

$(OUTL): | $(LDIR) $(LOBJS)
	@echo $@
	@gcc -shared $(LFLG) $(INCLUDES) $(PFLG) $(TRSH)/LIBS/$@.lo $(LIBS) -o $(BIN)/$@.dll;

$(TRSH)/%.lo: %.h $(TRSH)/MKLOG
	@echo $<
	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -MMD -Wa,-a=$(subst .lo,.asm,$@) -x c -c $< -o $@

#    ---     ---     ---     ---     ---

$(OUTF): | $(OBJS)
	@true

$(TRSH)/%.o: %.c $(TRSH)/MKLOG
	@echo $<
	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -MMD -Wa,-a=$(subst .o,.asm,$@) -c $< -o $@
	@gcc $(LFLG) $(INCLUDES) $(PFLG) $@ $(LIBS) -o $(patsubst %.c,$(BIN)/%,$<);

#   ---     ---     ---     ---     ---

.PHONY: clean
clean:
	@rm -rf $(TRSH)/*

$(TRSH)/MKLOG:
	@echo "$(DFLG)" > $(TRSH)/MKLOG

#   ---     ---     ---     ---     ---