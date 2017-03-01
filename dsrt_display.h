/* See LICENSE for license details. */

/*

Module: dsrt_display.h

Description:

    Utility functions for X11 display

*/

/* Reverse include guard */
#if defined(INC_DSRT_DISPLAY_H)
#error include dsrt_display.h once
#endif /* #if defined(INC_DSRT_DISPLAY_H) */

#define INC_DSRT_DISPLAY_H

/* Predefine context handle */
struct dsrt_ctxt;

/*

Structure: dsrt_display

Description:

    Storage of useful resources related to X11 display.

*/
struct dsrt_display
{
    Display * dis;

    Visual * vis;

    GC copyGC;

    Window root;

    unsigned long int red_mask;

    unsigned long int green_mask;

    unsigned long int blue_mask;

    double rRatio;

    double gRatio;

    double bRatio;

    int screen;

    unsigned int depth;

    unsigned int width;

    unsigned int height;

}; /* struct dsrt_display */

/* --- Interface --- */

char
dsrt_display_init(
    struct dsrt_ctxt const * const p_ctxt);

void
dsrt_display_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_display.h */
