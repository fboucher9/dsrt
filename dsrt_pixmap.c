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

/* View */
#include "dsrt_view.h"

char
dsrt_pixmap_init(
    struct dsrt_ctxt const * const p_ctxt,
    unsigned int const i_width,
    unsigned int const i_height)
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

    struct dsrt_view const * const p_view = p_ctxt->p_view;

    XSetWindowBackgroundPixmap(
        p_display->dis,
        p_view->h,
        p_pixmap->pixmap);

    XClearWindow(
        p_display->dis,
        p_view->h);

    XFlush(p_display->dis);

} /* dsrt_pixmap_apply() */

/* end-of-file: dsrt_pixmap.c */
