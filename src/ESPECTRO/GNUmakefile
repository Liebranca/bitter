# ESPECTRO GNU MAKEFILE

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
DFLG += -D KVR_DEBUG=0x00 
endif

#   ---     ---     ---     ---     ---

OFLG      = -s -Os -fno-unwind-tables -fno-asynchronous-unwind-tables
OFLG     += -fsingle-precision-constant -ffast-math -fno-ident
LFLG      = -ffunction-sections -fdata-sections -Wl,--gc-sections

FSWAT     = ESPECTRO
MKWAT     = espectro.exe

DIRS      = $(shell find $(BASE_DIR) -type d)
HEDS      = $(wildcard $(DIRS:%=%/*.h))
SRCS      = $(wildcard $(DIRS:%=%/*.c))
BIN       = $(dir $(abspath ../..)/bin/$(PLAT)/)
TRSH      = $(dir $(abspath ../..)/trashcan/$(PLAT)/$(FSWAT)/)

OBJS      = $(patsubst %.c,$(TRSH)/%.o,$(SRCS))
DEPS     := $(subst .o,.d,$(OBJS))
ODIR      = $(patsubst %,$(TRSH)/%,$(DIRS))

INCLUDES  = -I$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
INCLUDES += -I$(dir $(abspath ..)/)
LIBS      = -L$(BIN) -lkvrnel -lpng16 -lchasm

# -lgl -lopengl32 -lSDL2main -lSDL2

MAIN      = $(BIN)/$(MKWAT)

@DFLG_OLD = $(shell cat $(TRSH)/MKLOG)

#   ---     ---     ---     ---     ---

1: $(ODIR)
	@gcc $(OFLG) $(INCLUDES) $(PFLG) AVTOMAT/esp_werg.c $(LIBS) -o $(MAIN)

#$(MAIN): $(ODIR) $(HEDS) $(SRCS) $(DEPS) $(OBJS) $(TRSH)/MKLOG
#	@echo $(MAIN)
#	@if ! test -d $(BIN); then mkdir -p $(BIN); fi
#	@if test -f $(MAIN); then rm $(MAIN); fi
#	@gcc -shared $(OFLG) $(INCLUDES) $(PFLG) $(OBJS) -o $(MAIN)
#	@if test -f $(MAIN); then make -t -s; fi

#   ---     ---     ---     ---     ---

$(ODIR):
	@if ! test -d $@; then mkdir -p $@; fi

#%.c:: %.h $(TRSH)/MKLOG
#	@true
##	@if test -f %.h; then touch $@; @echo $<; fi
#
#$(TRSH)/%.d: %.c %.h $(TRSH)/MKLOG
#	@echo $@
#	@gcc -MM $(INCLUDES) $< > $@
#
#$(TRSH)/%.o: %.c %.h $(TRSH)/MKLOG
#	@echo $<
#	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -c $< -o $@

#   ---     ---     ---     ---     ---

#.PHONY: clean flags
#clean:
#	@rm -rf $(TRSH)/*
#	@if test -f $(MAIN); then rm $(MAIN); fi
#
#ifeq "$(DFLG)" "$(DFLG_OLD)"
#flags:
#	@if ! test -f $(TRSH)/MKLOG; then echo $(DFLG) > $(TRSH)/MKLOG; fi
#	@echo Using old flags -- no rebuild required
#else
#flags:
#	@echo Using new flags -- rebuild is needed
#	@echo "$(DFLG)" > $(TRSH)/MKLOG
#endif

#   ---     ---     ---     ---     ---
# dummies

#$(TRSH)/MKLOG:
#	@true

#   ---     ---     ---     ---     ---