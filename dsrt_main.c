/* See LICENSE for license details. */

/*

Module: dsrt_main.c

Description:

    Main entry-point for dsrt application.

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Options */
#include "dsrt_opts.h"

/* Display */
#include "dsrt_display.h"

/* JPEG decoder */
#include "dsrt_jpeg.h"

/* Image */
#include "dsrt_image.h"

/* Pixmap */
#include "dsrt_pixmap.h"

/* View */
#include "dsrt_view.h"

/* Zoom */
#if defined(DSRT_FEATURE_ZOOM)
#include "dsrt_zoom.h"
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

/* Module */
#include "dsrt_main.h"

struct dsrt_main_ctxt
{
    struct dsrt_ctxt o_ctxt;

    struct dsrt_display o_display;

    struct dsrt_opts o_opts;

    struct dsrt_jpeg o_jpeg;

    struct dsrt_image o_image;

    struct dsrt_pixmap o_pixmap;

    struct dsrt_view o_view;

#if defined(DSRT_FEATURE_ZOOM)
    struct dsrt_zoom o_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

};

static
void
dsrt_main_convert_line(
    struct dsrt_ctxt const * const p_ctxt,
    signed long int dy)
{
    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

#if defined(DSRT_FEATURE_ZOOM)
    struct dsrt_zoom const * const p_zoom = p_ctxt->p_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    u_int32_t * newBuf32 = (u_int32_t *)(p_ctxt->p_image->img->data);

    u_int16_t * newBuf16 = (u_int16_t *)(p_ctxt->p_image->img->data);

    JSAMPLE * lb1;

    JSAMPLE * lb2;

    signed long int i_source_y1;

    unsigned long int i_source_height;

    signed long int sy;

    unsigned long int fy0;

    signed long int sy1;

    signed long int sy2;

    unsigned long int fy1;

    unsigned long int fy2;

    signed long int dx;

    int x_dir;

    int y_dir;

    int i_source_x1;

    unsigned long int i_source_width;

#if defined(DSRT_FEATURE_ZOOM)
    i_source_y1 = p_zoom->i_top;

    i_source_height = p_zoom->u_height;
#else /* #if defined(DSRT_FEATURE_ZOOM) */
    i_source_y1 = 0;

    i_source_height = p_ctxt->p_jpeg->height;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

#if defined(DSRT_FEATURE_ZOOM)
    i_source_x1 = p_zoom->i_left;

    i_source_width = p_zoom->u_width;
#else /* #if defined(DSRT_FEATURE_ZOOM) */
    i_source_x1 = 0;

    i_source_width = p_jpeg->width;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    /* Calculate srcy */
    sy = (((dy * 128 + 64) * (signed long int)(i_source_height)) / (signed long int)(p_ctxt->p_image->height));

    fy0 = ((unsigned long int)(sy) & 0x7F);

    sy >>= 7;

#if defined(DSRT_FEATURE_MIRROR)
    if (p_opts->b_mirror_y)
    {
        sy = i_source_y1 + (signed long int)(i_source_height) - 1 - sy;

        y_dir = -1;
    }
    else
#endif /* #if defined(DSRT_FEATURE_MIRROR) */
    {
        sy = i_source_y1 + sy;

        y_dir = 1;
    }

    if (fy0 <= 64)
    {
        sy1 = sy;

        sy2 = sy - y_dir;

        fy1 = 64 + fy0;

        fy2 = 64 - fy0;
    }
    else
    {
        sy1 = sy;

        sy2 = sy + y_dir;

        fy1 = 128 + 64 - fy0;

        fy2 = fy0 - 64;
    }

    (void)(p_opts);

    if ((unsigned int)(sy1) < p_jpeg->height)
    {
        lb1 = p_jpeg->lineBuf[sy1];
    }
    else
    {
        lb1 = NULL;
    }

    if ((unsigned int)(sy2) < p_jpeg->height)
    {
        lb2 = p_jpeg->lineBuf[sy2];
    }
    else
    {
        lb2 = NULL;
    }

    /* for all destination pixels */
    for (dx = 0; dx < (signed long int)(p_ctxt->p_image->width); dx++)
    {
        u_int32_t col;
        u_int32_t r1;
        u_int32_t g1;
        u_int32_t b1;
        unsigned long int frac;
        signed long int sx1;
        signed long int sx2;
        unsigned long int fx1;
        unsigned long int fx2;

        /* calculate source position */
        signed long int sx = (((dx * 128 + 64) * (signed long int)(i_source_width)) / (signed long int)(p_ctxt->p_image->width));

        frac = (sx & 0x7F);

        sx >>= 7;

#if defined(DSRT_FEATURE_MIRROR)
        if (p_opts->b_mirror_x)
        {
            sx = i_source_x1 + (signed long int)(i_source_width) - 1 - sx;

            x_dir = -1;
        }
        else
#endif /* #if defined(DSRT_FEATURE_MIRROR) */
        {
            sx = i_source_x1 + sx;

            x_dir = 1;
        }

        if (frac <= 64)
        {
            sx1 = sx;

            sx2 = sx - x_dir;

            fx1 = 64 + frac;

            fx2 = 64 - frac;
        }
        else
        {
            sx1 = sx;

            sx2 = sx + x_dir;

            fx1 = 128 + 64 - frac;

            fx2 = frac - 64;
        }

        r1 = 0;

        g1 = 0;

        b1 = 0;

        if ((unsigned int)(sx1) < p_jpeg->width)
        {
            int xr = p_jpeg->xr0 + (sx1 * (int)(p_jpeg->bytesPerPix));

            int xg = p_jpeg->xg0 + (sx1 * (int)(p_jpeg->bytesPerPix));

            int xb = p_jpeg->xb0 + (sx1 * (int)(p_jpeg->bytesPerPix));

            if (lb1)
            {
                r1 += (u_int32_t)(lb1[xr]) * fx1 * fy1;

                g1 += (u_int32_t)(lb1[xg]) * fx1 * fy1;

                b1 += (u_int32_t)(lb1[xb]) * fx1 * fy1;
            }

            if (lb2)
            {
                r1 += (u_int32_t)(lb2[xr]) * fx1 * fy2;

                g1 += (u_int32_t)(lb2[xg]) * fx1 * fy2;

                b1 += (u_int32_t)(lb2[xb]) * fx1 * fy2;
            }
        }

        if ((unsigned int)(sx2) < p_jpeg->width)
        {
            int xr = p_jpeg->xr0 + (sx2 * (int)(p_jpeg->bytesPerPix));

            int xg = p_jpeg->xg0 + (sx2 * (int)(p_jpeg->bytesPerPix));

            int xb = p_jpeg->xb0 + (sx2 * (int)(p_jpeg->bytesPerPix));

            if (lb1)
            {
                r1 += (u_int32_t)(lb1[xr]) * fx2 * fy1;

                g1 += (u_int32_t)(lb1[xg]) * fx2 * fy1;

                b1 += (u_int32_t)(lb1[xb]) * fx2 * fy1;
            }

            if (lb2)
            {
                r1 += (u_int32_t)(lb2[xr]) * fx2 * fy2;

                g1 += (u_int32_t)(lb2[xg]) * fx2 * fy2;

                b1 += (u_int32_t)(lb2[xb]) * fx2 * fy2;
            }
        }

        r1 >>= 14u;

        g1 >>= 14u;

        b1 >>= 14u;

#if defined(DSRT_FEATURE_GRAY)
        if (p_opts->b_gray)
        {
            /* r * 21 */
            /* g * 72 */
            /* b * 7 */
            r1 = g1 = b1 = (((r1 * 215u) + (g1 * 737u) + (b1 * 72u)) >> 10u);
        }
#endif /* #if defined(DSRT_FEATURE_GRAY) */

#if defined(DSRT_FEATURE_SHADOW)
        if (p_opts->b_shadow)
        {
            r1 = (r1 * p_opts->i_shadow) >> 10u;

            g1 = (g1 * p_opts->i_shadow) >> 10u;

            b1 = (b1 * p_opts->i_shadow) >> 10u;
        }
#endif /* #if defined(DSRT_FEATURE_SHADOW) */

        /* get color of source pixel(s) */
        col =
            (u_int32_t)(
                ((u_int32_t)((r1) * p_display->rRatio) & p_display->red_mask) |
                ((u_int32_t)((g1) * p_display->gRatio) & p_display->green_mask) |
                ((u_int32_t)((b1) * p_display->bRatio) & p_display->blue_mask));

        /* store into destination */
        if (p_display->depth > 16)
        {
            *newBuf32 = col;
            ++newBuf32;
        }
        else
        {
            *newBuf16 = (u_int16_t)(col);
            ++newBuf16;
        }
    }
}

static
void
dsrt_main_write_line(
    struct dsrt_ctxt const * const p_ctxt,
    signed long int const y)
{
    struct dsrt_display const * const p_display = p_ctxt->p_display;

    int x_offset;

    int y_offset;

    x_offset = (((int)(p_ctxt->p_pixmap->width) - (int)(p_ctxt->p_image->width)) / 2);

    y_offset = (((int)(p_ctxt->p_pixmap->height) - (int)(p_ctxt->p_image->height)) / 2);

    /* put */
    XPutImage(
        p_display->dis,
        p_ctxt->p_pixmap->pixmap,
        p_display->copyGC,
        p_ctxt->p_image->img,
        0,
        0,
        x_offset,
        (y_offset + y),
        p_ctxt->p_image->width,
        1u);
}

/*

Function: dsrt_main_scan()

Description:

*/
static
void
dsrt_main_scan(
    struct dsrt_ctxt const * const p_ctxt)
{
    signed long int dy;

    XSetForeground(p_ctxt->p_display->dis, p_ctxt->p_display->copyGC, BlackPixel(p_ctxt->p_display->dis, p_ctxt->p_display->screen));

    XFillRectangle(p_ctxt->p_display->dis, p_ctxt->p_pixmap->pixmap, p_ctxt->p_display->copyGC, 0, 0, (unsigned int)(p_ctxt->p_pixmap->width), (unsigned int)(p_ctxt->p_pixmap->height));

    for (dy = 0; dy < (signed long int)(p_ctxt->p_image->height); ++dy)
    {
        /* Clear the line */
        if (p_ctxt->p_display->depth > 16)
        {
            memset(p_ctxt->p_image->img->data, 0, (size_t)(p_ctxt->p_image->width * 4));
        }
        else
        {
            memset(p_ctxt->p_image->img->data, 0, (size_t)(p_ctxt->p_image->width * 2));
        }

        dsrt_main_convert_line(p_ctxt, dy);

        dsrt_main_write_line(p_ctxt, dy);
    }
} /* dsrt_main_scan() */

/*

Function: dsrt_main_select_pixmap_size()

Description:

    Select the size of the pixmap using options and other available infor-
    mation.

*/
static
void
dsrt_main_select_pixmap_size(
    struct dsrt_ctxt const * const p_ctxt,
    unsigned int * const p_pixmap_width,
    unsigned int * const p_pixmap_height,
    unsigned int * const p_image_width,
    unsigned int * const p_image_height)
{
    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    struct dsrt_view const * const p_view = p_ctxt->p_view;

#if defined(DSRT_FEATURE_ZOOM)
    struct dsrt_zoom const * const p_zoom = p_ctxt->p_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    unsigned int i_pixmap_width;

    unsigned int i_pixmap_height;

    unsigned int i_image_width;

    unsigned int i_image_height;

    unsigned int i_source_width;

    unsigned int i_source_height;

#if defined(DSRT_FEATURE_ZOOM)
    i_source_width = p_zoom->u_width;

    i_source_height = p_zoom->u_height;
#else /* #if defined(DSRT_FEATURE_ZOOM) */
    i_source_width = p_ctxt->p_jpeg->width;

    i_source_height = p_ctxt->p_jpeg->height;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

#if defined(DSRT_FEATURE_CENTER)
    if (p_opts->b_center)
    {
        i_pixmap_width = p_view->width;

        i_pixmap_height = p_view->height;

        i_image_width = i_source_width;

        i_image_height = i_source_height;
    }
    else
#endif /* #if defined(DSRT_FEATURE_CENTER) */
    {
        /* Calculate best fit */
        if (p_opts->b_fit)
        {
            i_pixmap_width = p_view->width;

            i_pixmap_height = p_view->height;

            i_image_height = ((i_source_height * i_pixmap_width) / i_source_width);

            if (i_image_height > i_pixmap_height)
            {
                i_image_width = ((i_source_width * i_pixmap_height) / i_source_height);

                i_image_height = i_pixmap_height;
            }
            else
            {
                i_image_width = i_pixmap_width;
            }
        }
        else
        {
            i_pixmap_width = i_source_width;

            i_pixmap_height = i_source_height;

            i_image_width = i_pixmap_width;

            i_image_height = i_pixmap_height;
        }
    }

    *(p_pixmap_width) = i_pixmap_width;

    *(p_pixmap_height) = i_pixmap_height;

    *(p_image_width) = i_image_width;

    *(p_image_height) = i_image_height;

} /* dsrt_main_select_pixmap_size() */

static
char
dsrt_main_show_file(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    unsigned int i_pixmap_width;

    unsigned int i_pixmap_height;

    unsigned int i_image_width;

    unsigned int i_image_height;

#if defined(DSRT_FEATURE_ZOOM)
    dsrt_zoom_setup(p_ctxt, p_ctxt->p_jpeg->width, p_ctxt->p_jpeg->height);
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    dsrt_main_select_pixmap_size(p_ctxt, &i_pixmap_width, &i_pixmap_height, &i_image_width, &i_image_height);

#if defined(DSRT_FEATURE_LOG)
    {
#if defined(DSRT_FEATURE_ZOOM)
        char a_zoom[32];

        sprintf(a_zoom, "%lux%lu+%ld+%ld",
            p_ctxt->p_zoom->u_width,
            p_ctxt->p_zoom->u_height,
            p_ctxt->p_zoom->i_left,
            p_ctxt->p_zoom->i_top);
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

        fprintf(stderr, "        - j %4dx%-4d "
#if defined(DSRT_FEATURE_ZOOM)
            "> z %-20s "
#endif /* #if defined(DSRT_FEATURE_ZOOM) */
            "> i %4dx%-4d "
            "> p %4dx%-4d "
            "> v %4dx%-4d\n",
            p_ctxt->p_jpeg->width,
            p_ctxt->p_jpeg->height,
#if defined(DSRT_FEATURE_ZOOM)
            a_zoom,
#endif /* #if defined(DSRT_FEATURE_ZOOM) */
            i_image_width,
            i_image_height,
            i_pixmap_width,
            i_pixmap_height,
            p_ctxt->p_view->width,
            p_ctxt->p_view->height);
    }
#endif /* #if defined(DSRT_FEATURE_LOG) */

    if (dsrt_pixmap_init(p_ctxt, i_pixmap_width, i_pixmap_height))
    {
        char b_pixmap_cleanup = 1;

        if (dsrt_image_init(p_ctxt, i_image_width, i_image_height))
        {
            dsrt_main_scan(p_ctxt);

            dsrt_image_cleanup(p_ctxt);

            dsrt_pixmap_apply(p_ctxt);

            if (b_pixmap_cleanup)
            {
                dsrt_pixmap_cleanup(p_ctxt);

                b_pixmap_cleanup = 0;
            }

            /* Launch a viewer... */
            b_result = 1;
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf(stderr, "error out of memory\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

            b_result = 0;
        }

        if (b_pixmap_cleanup)
        {
            dsrt_pixmap_cleanup(p_ctxt);

            b_pixmap_cleanup = 0;
        }
    }
    else
    {
        b_result = 0;
    }

    return b_result;

} /* dsrt_main_show_file() */

static
void
dsrt_main_init_ctxt(
    struct dsrt_main_ctxt * const p_main)
{
    struct dsrt_ctxt * const p_ctxt = &p_main->o_ctxt;

    p_ctxt->p_display = &p_main->o_display;

    p_ctxt->p_opts = &p_main->o_opts;

    p_ctxt->p_jpeg = &p_main->o_jpeg;

    p_ctxt->p_image = &p_main->o_image;

    p_ctxt->p_pixmap = &p_main->o_pixmap;

    p_ctxt->p_view = &p_main->o_view;

#if defined(DSRT_FEATURE_ZOOM)
    p_ctxt->p_zoom = &p_main->o_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

} /* dsrt_main_init_ctxt() */

/*

Function: dsrt_main

Description:

    OS-independant application entry point.

*/
int
dsrt_main(
    int const argc,
    char const * const * const argv)
{
    int i_result;

    struct dsrt_main_ctxt o_main;

    struct dsrt_ctxt const * p_ctxt = &o_main.o_ctxt;

    dsrt_main_init_ctxt(&o_main);

    if (dsrt_display_init(p_ctxt))
    {
        if (dsrt_opts_init(p_ctxt, argc, argv))
        {
            /* Create a zoom object */
#if defined(DSRT_FEATURE_ZOOM)
            dsrt_zoom_init(p_ctxt);
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

            /* Create a preview window */
            if (dsrt_view_init(p_ctxt))
            {
                char c_event;

                char b_jpeg_cleanup;

                unsigned int i_file_iterator;

                unsigned int i_file_previous;

                i_file_iterator = 0;

                i_file_previous = ~0u;

                c_event = ' ';

                b_jpeg_cleanup = 0;

                while ('q' != c_event)
                {
                    char b_result;

                    char const * p_filename = p_ctxt->p_opts->a_filename[i_file_iterator];

                    if (i_file_previous != i_file_iterator)
                    {
#if defined(DSRT_FEATURE_LOG)
                        fprintf(stderr, "%3u/%-3u %s\n",
                            i_file_iterator + 1,
                            p_ctxt->p_opts->n_files,
                            p_ctxt->p_opts->a_filename[i_file_iterator]);
#endif /* #if defined(DSRT_FEATURE_LOG) */

                        if (b_jpeg_cleanup)
                        {
                            dsrt_jpeg_cleanup(p_ctxt);

                            b_jpeg_cleanup = 0;
                        }

                        b_result = dsrt_jpeg_init(p_ctxt, p_filename);

                        if (b_result)
                        {
                            b_jpeg_cleanup = 1;

                            i_file_previous = i_file_iterator;
                        }
                        else
                        {
                            i_file_previous = ~0u;
                        }
                    }
                    else
                    {
                        b_result = 1;
                    }

                    if (b_result)
                    {
                        if (dsrt_main_show_file(p_ctxt))
                        {
                            /* Wait for event from preview window... */
                            int i_mouse_x;

                            int i_mouse_y;

                            i_mouse_x = 0;

                            i_mouse_y = 0;

                            c_event = dsrt_view_event(p_ctxt, &i_mouse_x, &i_mouse_y);

                            if ('n' == c_event)
                            {
                                i_file_iterator ++;
                                if (i_file_iterator >= p_ctxt->p_opts->n_files)
                                {
                                    i_file_iterator = 0;
                                }
                            }
                            else if ('p' == c_event)
                            {
                                if (0 == i_file_iterator)
                                {
                                    i_file_iterator = p_ctxt->p_opts->n_files;
                                }
                                i_file_iterator --;
                            }

#if defined(DSRT_FEATURE_ZOOM)
                            dsrt_zoom_event(p_ctxt, c_event, i_mouse_x, i_mouse_y);
#endif /* #if defined(DSRT_FEATURE_ZOOM) */
                        }
                        else
                        {
                            c_event = 'q';
                        }
                    }
                    else
                    {
                        c_event = 'q';
                    }
                }

                i_result = 0;

                dsrt_view_cleanup(p_ctxt);
            }
            else
            {
                i_result = 1;
            }

            dsrt_opts_cleanup(p_ctxt);
        }
        else
        {
            i_result = 1;
        }

        dsrt_display_cleanup(p_ctxt);
    }
    else
    {
        i_result = 1;
    }

    return i_result;

} /* dsrt_main() */

/* end-of-file: dsrt_main.c */
