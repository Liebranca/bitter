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

LFLG      = -flto -ffunction-sections -fdata-sections
LFLG     += -Wl,--gc-sections -Wl,-fuse-ld=bfd

DIRS      = $(shell find $(BASE_DIR) -type d)
SRCS      = $(wildcard $(DIRS:%=%/*.c))
BIN       = $(dir $(abspath ../..)/bin/$(PLAT)/)

TRSH      = $(dir $(abspath ../..)/trashcan/$(PLAT)/$(FSWAT)/)

OBJS      = $(patsubst %.c,$(TRSH)/%.o,$(SRCS))
DEPS      = $(subst %.o,%.d,$(OBJS))
ODIR      = $(patsubst %,$(TRSH)/%,$(DIRS))

INCLUDES  = -I$(dir $(realpath $(firstword $(MAKEFILE_LIST))))

INCLUDES += -I$(dir $(abspath ..)/)
LIBS      = -L$(BIN) -lkvrnel

OUTF      = spwn msafr j3l

#   ---     ---     ---     ---     ---

.PHONY: $(OUTF)

$(OUTF): | $(ODIR) $(OBJS)
	@for i in $(OUTF);do\
  echo $$i;\
	gcc $(LFLG) $(INCLUDES) $(PFLG) $(TRSH)/$$i.o $(LIBS) -o $(BIN)/$$i;\
  done


#   ---     ---     ---     ---     ---

$(ODIR):
	@if ! test -d $@; then mkdir -p $@; fi

%.c:
	@true

$(TRSH)/%.o: %.c $(TRSH)/MKLOG
	@echo $<
	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -MMD -Wa,-a=$(subst .o,.asm,$@) -c $< -o $@

#   ---     ---     ---     ---     ---

.PHONY: clean
clean:
	@rm -rf $(TRSH)/*

$(TRSH)/MKLOG:
	@echo "$(DFLG)" > $(TRSH)/MKLOG

#   ---     ---     ---     ---     ---