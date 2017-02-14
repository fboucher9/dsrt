/* See LICENSE for license details. */

/*

Module: dsrt_view.h

Description:

*/

/* Reverse include guard */
#if defined(INC_DSRT_VIEW_H)
#error include dsrt_view.h once
#endif /* #if defined(INC_DSRT_VIEW_H) */

#define INC_DSRT_VIEW_H

/* Predefine context handle */
struct dsrt_ctxt;

/*

Structure: dsrt_view

Description:

*/
struct dsrt_view
{
    Window h;

    int width;

    int height;

}; /* struct dsrt_view */

/* --- Interface --- */

char
dsrt_view_init(
    struct dsrt_ctxt const * const p_ctxt);

void
dsrt_view_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

char
dsrt_view_event(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_view.h */
