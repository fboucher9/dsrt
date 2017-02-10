/* See LICENSE for license details. */

/*

Module: dsrt_image.h

Description:

*/

/* Reverse include guard */
#if defined(INC_DSRT_IMAGE_H)
#error include dsrt_image.h once
#endif /* #if defined(INC_DSRT_IMAGE_H) */

/* Predefine context handle */
struct dsrt_ctxt;

/*

Structure: dsrt_image

Description:

    XImage for a scanline.  This is used as a temporary buffer for holding
    pixels in device format.  Once a whole line has been converted, it is
    copied to the pixmap using XPutImage() function.

Comments:

    -   Same width as jpeg file.

    -   Same depth as visual.

*/

struct dsrt_image
{
    XImage * img;

}; /* struct dsrt_image */

/* --- Interface --- */

char
dsrt_image_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const i_width);

void
dsrt_image_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_image.h */
