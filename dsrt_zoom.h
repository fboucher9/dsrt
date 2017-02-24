/* See LICENSE for license details. */

/*

Module: dsrt_zoom.h

Description:

    Zoom a rectangle within jpeg image.

*/

/* Reverse include guard */
#if defined(INC_DSRT_ZOOM_H)
#error include dsrt_zoom.h once
#endif /* #if defined(INC_DSRT_ZOOM_H) */

#define INC_DSRT_ZOOM_H

/*

Structure: dsrt_zoom

Description:

    State for dsrt_zoom module.

*/
struct dsrt_zoom
{
    /* Left inclusive coordinate of zoom rectangle */
    signed long int x1;

    /* Top inclusive coordinate of zoom rectangle */
    signed long int y1;

    /* Right exclusive coordinate of zoom rectangle */
    signed long int x2;

    /* Bottom exclusive coordinate of zoom rectangle */
    signed long int y2;

}; /* struct dsrt_zoom */

/* --- Interface --- */

char
dsrt_zoom_init(
    struct dsrt_ctxt const * const p_ctxt);

void
dsrt_zoom_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

void
dsrt_zoom_setup(
    struct dsrt_ctxt const * const p_ctxt,
    signed long int const width,
    signed long int const height);

void
dsrt_zoom_event(
    struct dsrt_ctxt const * const p_ctxt,
    char const c_event);

/* end-of-file: dsrt_zoom.h */
