# ESPECTRO GNU MAKEFILE

#   ---     ---     ---     ---     ---

OFLG      = -s -Os -fno-unwind-tables -fno-asynchronous-unwind-tables
OFLG     += -fsingle-precision-constant -ffast-math -fno-ident
LFLG      = -s -Os -flto -ffunction-sections -fdata-sections
LFLG     += -Wl,--gc-sections -Wl,-fuse-ld=bfd

FSWAT     = ESPECTRO
MKWAT     = es-shell.exe

DIRS      = $(shell find $(BASE_DIR) -type d)
HEDS      = $(wildcard $(DIRS:%=%/*.h))
SRCS      = $(wildcard $(DIRS:%=%/*.c))
BIN       = $(dir $(abspath ../..)/bin/)
TRSH      = $(dir $(abspath ../..)/trashcan/$(FSWAT)/)

OBJS      = $(patsubst %.c,$(TRSH)/%.o,$(SRCS))
DEPS     := $(subst .o,.d,$(OBJS))
ODIR      = $(patsubst %,$(TRSH)/%,$(DIRS))

INCLUDES  = -I$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
INCLUDES += -I$(dir $(abspath ..)/)
LIBS      = -L$(BIN) -lkvrnel -lgl -lglew -lopengl32 -lsin -lSDL2main -lSDL2 -lchasm 

MAIN      = $(BIN)/$(MKWAT)

#   ---     ---     ---     ---     ---
# D_FLAGS switches

DFLG  = 

ifdef DEBUG
DFLG += -D KVR_DEBUG=$(DEBUG)
else
DFLG += $(shell cat $(TRSH)/MKLOG)
endif

#   ---     ---     ---     ---     ---

$(MAIN): $(ODIR) $(HEDS) $(SRCS) $(DEPS) $(OBJS) $(TRSH)/MKLOG
	@echo $(MAIN)
	@if ! test -d $(BIN); then mkdir -p $(BIN); fi
	@if test -f $(MAIN); then rm $(MAIN); fi
	@gcc $(LFLG) $(INCLUDES) $(PFLG) $(OBJS) $(LIBS) -o $(MAIN)
	@if test -f $(MAIN); then make -t -s; fi

#   ---     ---     ---     ---     ---

$(ODIR):
	@if ! test -d $@; then mkdir -p $@; fi

%.c:: %.h $(TRSH)/MKLOG
	@true
#	@if test -f %.h; then touch $@; @echo $<; fi

$(TRSH)/%.d: %.c %.h $(TRSH)/MKLOG
	@echo $@
	@gcc -MM $(INCLUDES) $< > $@

$(TRSH)/%.o: %.c %.h $(TRSH)/MKLOG
	@echo $<
	@gcc $(OFLG) $(INCLUDES) $(DFLG) $(PFLG) -c $< -o $@

#   ---     ---     ---     ---     ---

.PHONY: clean
clean:
	@rm -rf $(TRSH)/*
	@if test -f $(MAIN); then rm $(MAIN); fi

$(TRSH)/MKLOG:
	@echo "$(DFLG)" > $(TRSH)/MKLOG

#   ---     ---     ---     ---     ---