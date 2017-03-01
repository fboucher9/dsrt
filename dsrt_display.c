/* See LICENSE for license details. */

/*

Module: dsrt_display.c

Description:

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_display.h"

/*

Function: dsrt_display_init

Description:

    Initialize dsrt_display module.

*/
char
dsrt_display_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_display * const p_display = p_ctxt->p_display;

    p_display->dis = XOpenDisplay(NULL);

    if (p_display->dis)
    {
        p_display->root = DefaultRootWindow(p_display->dis);

        p_display->screen = DefaultScreen(p_display->dis);

        p_display->depth = (unsigned int)(DefaultDepth(p_display->dis, p_display->screen));

        p_display->vis = DefaultVisual(p_display->dis, p_display->screen);

        p_display->width = (unsigned int)(DisplayWidth(p_display->dis, p_display->screen));

        p_display->height = (unsigned int)(DisplayHeight(p_display->dis, p_display->screen));

        p_display->copyGC = XCreateGC(p_display->dis, p_display->root, 0, NULL);

        p_display->red_mask = p_display->vis->red_mask;

        p_display->green_mask = p_display->vis->green_mask;

        p_display->blue_mask = p_display->vis->blue_mask;

        p_display->rRatio = p_display->red_mask / 255.0;

        p_display->gRatio = p_display->green_mask / 255.0;

        p_display->bRatio = p_display->blue_mask / 255.0;

        return 1;
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "unable to open X display\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        return 0;
    }

} /* dsrt_display_init() */

/*

Function: dsrt_display_cleanup()

Description:

    Free all resources allocated by dsrt_display module.

*/
void
dsrt_display_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_display * const p_display = p_ctxt->p_display;

    if (p_display->dis)
    {
        XCloseDisplay(p_display->dis);

        p_display->dis = NULL;

        p_display->vis = NULL;
    }

} /* dsrt_display_cleanup() */

/* end-of-file: dsrt_display.c */
