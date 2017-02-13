/* See LICENSE for license details. */

/*

Module: dsrt_pixmap.c

Description:

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_pixmap.h"

/* Options */
#include "dsrt_opts.h"

/* Display */
#include "dsrt_display.h"

char
dsrt_pixmap_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const i_width,
    int const i_height)
{
    char b_result;

    struct dsrt_pixmap * const p_pixmap = p_ctxt->p_pixmap;

    p_pixmap->width = i_width;

    p_pixmap->height = i_height;

    p_pixmap->pixmap = XCreatePixmap(
        p_ctxt->p_display->dis,
        p_ctxt->p_display->root,
        p_pixmap->width,
        p_pixmap->height,
        p_ctxt->p_display->depth);

    b_result = 1;

    return b_result;

} /* dsrt_pixmap_init() */

void
dsrt_pixmap_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_pixmap * const p_pixmap = p_ctxt->p_pixmap;

    if (None != p_pixmap->pixmap)
    {
        XFreePixmap(
            p_ctxt->p_display->dis,
            p_pixmap->pixmap);

        p_pixmap->pixmap = None;
    }

} /* dsrt_pixmap_cleanup() */

void
dsrt_pixmap_apply(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_pixmap const * const p_pixmap = p_ctxt->p_pixmap;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    Window i_window;

#if defined(DSRT_FEATURE_PREVIEW)
    if (p_opts->b_preview)
    {
        Window i_parent;

#if defined(DSRT_FEATURE_EMBED)
        if (p_opts->b_embed)
        {
            i_parent = p_opts->i_embed;
        }
        else
#endif /* #if defined(DSRT_FEATURE_EMBED) */
        {
            i_parent = p_display->root;
        }

        i_window = XCreateSimpleWindow(
            p_display->dis,
            i_parent,
            0,
            0,
            p_pixmap->width,
            p_pixmap->height,
            0,
            0,
            0);

        XMapRaised(
            p_display->dis,
            i_window);

        XSelectInput(
            p_display->dis,
            i_window,
            KeyPressMask);
    }
    else
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */
    {
        i_window = p_display->root;
    }

    XSetWindowBackgroundPixmap(
        p_display->dis,
        i_window,
        p_pixmap->pixmap);

    XClearWindow(
        p_display->dis,
        i_window);

    XFlush(p_display->dis);

#if defined(DSRT_FEATURE_PREVIEW)
    if (p_opts->b_preview)
    {
        char b_continue;

        b_continue = 1;

        while (b_continue)
        {
            XEvent o_event;

            XNextEvent(
                p_display->dis,
                &(
                    o_event));

            if (KeyPress == o_event.type)
            {
                if (XK_q == XLookupKeysym(&(o_event.xkey), 0))
                {
                    b_continue = 0;
                }
            }
        }
    }
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */

} /* dsrt_pixmap_apply() */

/* end-of-file: dsrt_pixmap.c */
