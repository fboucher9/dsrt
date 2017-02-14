/* See LICENSE for license details. */

/*

Module: dsrt_image.c

Description:

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_image.h"

/* Display */
#include "dsrt_display.h"

void XDestroyImage(XImage* p_image);

#if defined(DSRT_FEATURE_MSB)

static
int
get_byte_order(void)
{
    union {
        char c[sizeof(short)];
        short s;
    } order;

    order.s = 1;
    if ((1 == order.c[0])) {
        return LSBFirst;
    } else {
        return MSBFirst;
    }
} /* get_byte_order() */

#endif /* #if defined(DSRT_FEATURE_MSB) */

char
dsrt_image_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const i_width,
    int const i_height)
{
    char b_result;

    struct dsrt_image * const p_image = p_ctxt->p_image;

    int const depth_bits_rounded = ((p_ctxt->p_display->depth + 15) & ~15);

    int const depth_bytes = (depth_bits_rounded / 8);

    size_t const i_len = (depth_bytes * i_width);

    void * const p_data = malloc(i_len);

    if (p_data)
    {
        p_image->img = XCreateImage(
            p_ctxt->p_display->dis,
            CopyFromParent,
            p_ctxt->p_display->depth,
            ZPixmap,
            0,
            p_data,
            i_width,
            1,
            depth_bits_rounded,
            0);

        if (p_image->img)
        {
            XInitImage (p_image->img);

            /*Set the client's byte order, so that XPutImage knows what to do with the data.*/
            /*The default in a new X image is the server's format, which may not be what we want.*/
#if defined(DSRT_FEATURE_MSB)

            if ((LSBFirst == get_byte_order ()))
            {
                p_image->img->byte_order = LSBFirst;
            }
            else
            {
                p_image->img->byte_order = MSBFirst;
            }
#else /* #if defined(DSRT_FEATURE_MSB) */

            p_image->img->byte_order = LSBFirst;

#endif /* #if defined(DSRT_FEATURE_MSB) */

            /*The bitmap_bit_order doesn't matter with ZPixmap images.*/
            p_image->img->bitmap_bit_order = MSBFirst;

            p_image->width = i_width;

            p_image->height = i_height;

            b_result = 1;
        }
        else
        {
            b_result = 0;
        }

        if (!b_result)
        {
            free(p_data);
        }
    }
    else
    {
        b_result = 0;
    }

    return b_result;

} /* dsrt_image_init() */

void
dsrt_image_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_image * const p_image = p_ctxt->p_image;

    if (p_image->img)
    {
        XDestroyImage(p_image->img);

        p_image->img = NULL;
    }
} /* dsrt_image_cleanup() */

