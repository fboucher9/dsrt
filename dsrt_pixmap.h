/* See LICENSE for license details. */

/*

Module: dsrt_pixmap.h

Description:

*/

/* Reverse include guard */
#if defined(INC_DSRT_PIXMAP_H)
#error include dsrt_pixmap.h once
#endif /* #if defined(INC_DSRT_PIXMAP_H) */

#define INC_DSRT_PIXMAP_H

/* Predefine context handle */
struct dsrt_ctxt;

/*

Structure: dsrt_pixmap

Description:

*/
struct dsrt_pixmap
{
    Drawable pixmap;

    int width;

    int height;

}; /* struct dsrt_pixmap */

/* --- Interface --- */

char
dsrt_pixmap_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const i_width,
    int const i_height);

void
dsrt_pixmap_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

void
dsrt_pixmap_apply(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_pixmap.h */
