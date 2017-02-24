# See LICENSE for license details

# dsrt - /DeSseRT/ - DiSplay jpg on RooT

ifndef DBG
DBG = 0
endif

ifeq ($(DBG),1)
DSRT_CFG_DBG = chk
else
DSRT_CFG_DBG = fre
endif

DSRT_CFLAGS_chk = -g -O0

DSRT_CFLAGS_fre = -O2 -Os

DSRT_CFLAGS = $(CFLAGS) $(DSRT_CFLAGS_$(DSRT_CFG_DBG)) -pedantic -Wall -Wextra -I$(DSRT_DST_PATH)

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
	@$(DSRT_CC) @$@.cmd

# Build the precompiled header
$(DSRT_DST_PATH)/dsrt_os.h.gch : $(DSRT_SRC_PATH)/dsrt_os.h
	@echo generating $@
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
