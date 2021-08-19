# KVRNEL GNU MAKEFILE

#   ---     ---     ---     ---     ---

ifdef x32
PLAT = x32
PFLG = -m32
else
PLAT = x64
PFLG = -m64
endif

#   ---     ---     ---     ---     ---

FSWAT     = KVRNEL
MKWAT     = kvrnel.dll

DIRS      = $(shell find $(BASE_DIR) -type d)
HEDS      = $(wildcard $(DIRS:%=%/*.h))
SRCS      = $(wildcard $(DIRS:%=%/*.c))
BIN       = ../../bin/$(PLAT)
TRSH      = ../../trashcan/$(PLAT)/$(FSWAT)

OBJS      = $(patsubst %.c,$(TRSH)/%.o,$(SRCS))
DEPS     := $(subst .o,.d,$(OBJS))
ODIR      = $(patsubst %,$(TRSH)/%,$(DIRS))

INCLUDES  = -I$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
MAIN      = $(BIN)/$(MKWAT)

#   ---     ---     ---     ---     ---

$(MAIN): | $(ODIR) $(DEPS) $(OBJS)
	@echo $(MAIN)
	@if test -f $(BIN)/$(MKWAT); then rm $(MAIN); fi
	@gcc -shared $(INCLUDES) $(PFLG) $(OBJS) -o $(MAIN)
	@if test -f $(MAIN); echo make -t -s; fi

$(ODIR):
	@if ! test -d $@; then mkdir -p $@; fi

$(TRSH)/%.d: %.c %.h
	@gcc -MM $(INCLUDES) $< -o $@

$(TRSH)/%.o: %.c
	@echo $<
	@gcc $(INCLUDES) $(PFLG) -c $< -o $@

include $(wildcard $(DEPS))

#   ---     ---     ---     ---     ---