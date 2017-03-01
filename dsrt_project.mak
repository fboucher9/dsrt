# See LICENSE for license details

# dsrt - /DeSseRT/ - DiSplay jpg on RooT

ifndef DBG
DBG = 0
endif

ifndef DSRT_CC
DSRT_CC = $(CC)
endif

ifndef DSRT_CXX
DSRT_CXX = $(CXX)
endif

ifeq ($(DBG),1)
DSRT_CFG_DBG = chk
else
DSRT_CFG_DBG = fre
endif

DSRT_CFLAGS_chk = -g -O0

DSRT_CFLAGS_fre = -O2 -Os

DSRT_CFLAGS_WARNINGS = \
    -pedantic -Wall -Wextra -Wabi -Waggregate-return -Warray-bounds \
    -Wattributes -Wbad-function-cast -Wbuiltin-macro-redefined -Wc++-compat \
    -Wcast-align -Wcast-qual -Wconversion -Wdeclaration-after-statement \
    -Wdeprecated -Wdiv-by-zero -Wendif-labels -Wfloat-equal \
    -Wformat-contains-nul -Wformat-extra-args -Wformat-nonliteral \
    -Wformat-security -Wformat-y2k -Wformat-zero-length -Wint-to-pointer-cast \
    -Wlarger-than=1024 -Wlong-long -Wmissing-declarations \
    -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-prototypes \
    -Wmultichar -Wnested-externs -Wold-style-definition -Woverflow \
    -Woverlength-strings -Wpacked -Wpacked-bitfield-compat -Wpadded \
    -Wpointer-arith -Wpointer-to-int-cast -Wpragmas -Wredundant-decls \
    -Wsequence-point -Wshadow -Wstrict-overflow=5 -Wstrict-prototypes \
    -Wsync-nand -Wundef -Wunused -Wunused-macros -Wunused-result \
    -Wvariadic-macros -Wvla -Wwrite-strings

DSRT_CXXFLAGS_chk = $(DSRT_CFLAGS_chk)

DSRT_CXXFLAGS_fre = $(DSRT_CFLAGS_fre)

DSRT_CXXFLAGS_WARNINGS = \
    -pedantic -Wall -Wextra -Wabi -Waggregate-return -Warray-bounds \
    -Wattributes -Wbuiltin-macro-redefined -Wc++0x-compat \
    -Wcast-align -Wcast-qual -Wconversion \
    -Wdeprecated -Wdiv-by-zero -Wendif-labels -Wfloat-equal \
    -Wformat-contains-nul -Wformat-extra-args -Wformat-nonliteral \
    -Wformat-security -Wformat-y2k \
    -Wlarger-than=1024 -Wlong-long -Wmissing-declarations \
    -Wmissing-format-attribute -Wmissing-include-dirs \
    -Wmultichar -Woverflow \
    -Woverlength-strings -Wpacked -Wpacked-bitfield-compat -Wpadded \
    -Wpointer-arith -Wpragmas -Wredundant-decls \
    -Wsequence-point -Wshadow -Wstrict-overflow=5 \
    -Wsync-nand -Wundef -Wunused -Wunused-macros -Wunused-result \
    -Wvariadic-macros -Wvla -Wwrite-strings \
    -Wctor-dtor-privacy -Weffc++ -Wenum-compare -Wnon-virtual-dtor \
    -Woverloaded-virtual -Wstrict-null-sentinel -Wsign-promo


DSRT_INCLUDES = -I$(DSRT_DST_PATH)

DSRT_CFLAGS = $(CFLAGS) $(DSRT_CFLAGS_$(DSRT_CFG_DBG)) $(DSRT_CFLAGS_WARNINGS) $(DSRT_INCLUDES)

DSRT_CXXFLAGS = $(CXXFLAGS) $(DSRT_CXXFLAGS_$(DSRT_CFG_DBG)) $(DSRT_CXXFLAGS_WARNINGS) $(DSRT_INCLUDES)

DSRT_LDFLAGS_chk =

DSRT_LDFLAGS_fre = -s

DSRT_LDFLAGS = $(LDFLAGS) $(DSRT_LDFLAGS_$(DSRT_CFG_DBG)) -lX11 -ljpeg -lm

DSRT_SRCS = \
    $(DSRT_DST_PATH)/_obj_dsrt_main.o \
    $(DSRT_DST_PATH)/_obj_dsrt_opts.o \
    $(DSRT_DST_PATH)/_obj_dsrt_display.o \
    $(DSRT_DST_PATH)/_obj_dsrt_jpeg.o \
    $(DSRT_DST_PATH)/_obj_dsrt_image.o \
    $(DSRT_DST_PATH)/_obj_dsrt_pixmap.o \
    $(DSRT_DST_PATH)/_obj_dsrt_view.o \
    $(DSRT_DST_PATH)/_obj_dsrt_zoom.o \
    $(DSRT_DST_PATH)/_obj_dsrt_os.o

.PHONY: all clean

# Default target
all : $(DSRT_DST_PATH)/dsrt

# Link the target
$(DSRT_DST_PATH)/dsrt: $(DSRT_SRCS)
	@echo linking $@
	@echo -o $@ $(DSRT_CFLAGS) $(DSRT_SRCS) $(DSRT_LDFLAGS) > $(DSRT_DST_PATH)/_obj_dsrt.cmd
	@$(DSRT_CC) @$(DSRT_DST_PATH)/_obj_dsrt.cmd

# Build each object file
$(DSRT_DST_PATH)/_obj_%.o : $(DSRT_SRC_PATH)/%.c
	@echo compiling $@
	@echo -c -o $@ $(DSRT_CFLAGS) -MT $@ -MMD -MP -MF $@.d $< > $@.cmd
	@$(DSRT_CXX) -c -x c++ -o $@.cxx $(DSRT_CXXFLAGS) $<
	@$(DSRT_CC) @$@.cmd

# Build the precompiled header
$(DSRT_DST_PATH)/dsrt_os.h.gch : $(DSRT_SRC_PATH)/dsrt_os.h
	@echo generating $@
	@$(DSRT_CXX) -c -o $@.cxx $(DSRT_CXXFLAGS) $(DSRT_SRC_PATH)/dsrt_os.h
	@$(DSRT_CC) -c -o $@ $(DSRT_CFLAGS) $(DSRT_SRC_PATH)/dsrt_os.h

# Indicate that all object files have dependency on precompiled header
$(DSRT_SRCS) : $(DSRT_DST_PATH)/dsrt_os.h.gch

clean:
	-rm -f $(DSRT_DST_PATH)/dsrt
	-rm -f $(DSRT_DST_PATH)/_obj_*.o
	-rm -f $(DSRT_DST_PATH)/_obj_*.d
	-rm -f $(DSRT_DST_PATH)/_obj_*.cmd
	-rm -f $(DSRT_DST_PATH)/*.gch

-include $(DSRT_DST_PATH)/_obj_*.o.d

# end-of-file: dsrt_project.mak
