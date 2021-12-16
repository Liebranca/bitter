# UTILS GNU MAKEFILE

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
# D_FLAGS switches

DFLG  = 

ifdef DEBUG
DFLG += -D KVR_DEBUG=$(DEBUG)
else
DFLG += $(shell cat $(TRSH)/MKLOG)
endif

#   ---     ---     ---     ---     ---

FSWAT     = UTILS

OFLG      = -s -Os -fno-unwind-tables
OFLG     += -fno-asynchronous-unwind-tables -ffast-math
OFLG     += -fsingle-precision-constant -fno-ident

LFLG      = $(OFLG)
LFLG     += -flto -ffunction-sections -fdata-sections
LFLG     += -Wl,--gc-sections -Wl,-fuse-ld=bfd

DIRS      = $(shell find $(BASE_DIR) -type d)
SRCS      = $(wildcard $(DIRS:%=%/*.c))
HEDS      = $(wildcard $(DIRS:%=%/*.h))
BIN       = $(dir $(abspath ../..)/bin/$(PLAT)/)

TRSH      = $(dir $(abspath ../..)/trashcan/$(PLAT)/$(FSWAT)/)

OBJS      = $(patsubst %.c,$(TRSH)/%.o,$(SRCS))
LOBJS     = $(patsubst %.h,$(TRSH)/%.lo,$(HEDS))
DEPS      = $(subst %.o,%.d,$(OBJS))
ODIR      = $(patsubst %,$(TRSH)/%,$(SRCS))
LDIR      = $(patsubst %,$(TRSH)/%,$(HEDS))

INCLUDES  = -I$(dir $(realpath $(firstword $(MAKEFILE_LIST))))

INCLUDES += -I$(dir $(abspath ..)/)
LIBS      = -L$(BIN) -lkvrnel

OUTL      = test
OUTF      = spwn msafr j3l scpx makepy

#   ---     ---     ---     ---     ---

.PHONY: $(OUTF) $(OUTL) MAIN

MAIN: $(OUTF) $(OUTL)
	@true

$(OUTL): | $(LDIR) $(LOBJS)
	@echo $@
	@gcc -shared $(LFLG) $(INCLUDES) $(PFLG) $(TRSH)/LIBS/$@.lo $(LIBS) -o $(BIN)/$@.dll;

%.h:
	@true

$(TRSH)/%.lo: %.h $(TRSH)/MKLOG
	@echo $<
	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -MMD -Wa,-a=$(subst .lo,.asm,$@) -x c -c $< -o $@

#    ---     ---     ---     ---     ---

$(OUTF): | $(ODIR) $(OBJS)
	@echo $@
	@gcc $(LFLG) $(INCLUDES) $(PFLG) $(TRSH)/$@.o $(LIBS) -o $(BIN)/$@;

%.c:
	@true

$(TRSH)/%.o: %.c $(TRSH)/MKLOG
	@echo $<
	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -MMD -Wa,-a=$(subst .o,.asm,$@) -c $< -o $@

#   ---     ---     ---     ---     ---

$(LDIR):
	@if ! test -d $@; then mkdir -p $@; fi

$(ODIR):
	@if ! test -d $@; then mkdir -p $@; fi

#   ---     ---     ---     ---     ---

.PHONY: clean
clean:
	@rm -rf $(TRSH)/*

$(TRSH)/MKLOG:
	@echo "$(DFLG)" > $(TRSH)/MKLOG

#   ---     ---     ---     ---     ---