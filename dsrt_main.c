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

struct dsrt_main
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
    signed long int sy1,
    signed long int fy1,
    signed long int sy2,
    signed long int fy2)
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

    signed long int dx;

    int i_source_x1;

    int i_source_width;

    (void)(p_opts);

    if ((sy1 >= 0) && (sy1 < p_jpeg->height))
    {
        lb1 = p_jpeg->lineBuf[sy1];
    }
    else
    {
        lb1 = NULL;
    }

    if ((sy2 >= 0) && (sy2 < p_jpeg->height))
    {
        lb2 = p_jpeg->lineBuf[sy2];
    }
    else
    {
        lb2 = NULL;
    }

#if defined(DSRT_FEATURE_ZOOM)
    i_source_x1 = p_zoom->x1;

    i_source_width = p_zoom->x2 - p_zoom->x1;
#else /* #if defined(DSRT_FEATURE_ZOOM) */
    i_source_x1 = 0;

    i_source_width = p_jpeg->width;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    /* for all destination pixels */
    for (dx = 0; dx < p_ctxt->p_image->width; dx++)
    {
        u_int32_t col;
        u_int32_t r1;
        u_int32_t g1;
        u_int32_t b1;
        u_int32_t r2;
        u_int32_t g2;
        u_int32_t b2;
        u_int32_t r3;
        u_int32_t g3;
        u_int32_t b3;
        u_int32_t r4;
        u_int32_t g4;
        u_int32_t b4;
        signed long int frac;
        signed long int sx1;
        signed long int sx2;
        signed long int fx1;
        signed long int fx2;

        /* calculate source position */
        signed long int sx = (((dx * 100 + 50) * i_source_width) / p_ctxt->p_image->width);

        frac = (sx % 100);

        sx /= 100;

        sx += i_source_x1;

        if (frac <= 50)
        {
            sx1 = sx;

            sx2 = sx - 1;

            fx1 = 50 + frac;

            fx2 = 50 - frac;
        }
        else
        {
            sx1 = sx;

            sx2 = sx + 1;

            fx1 = 150 - frac;

            fx2 = frac - 50;
        }

        if ((sx1 >= 0) && (sx1 < p_jpeg->width))
        {
            int xr = p_jpeg->xr0 + (sx1 * p_jpeg->bytesPerPix);

            int xg = p_jpeg->xg0 + (sx1 * p_jpeg->bytesPerPix);

            int xb = p_jpeg->xb0 + (sx1 * p_jpeg->bytesPerPix);

            if (lb1)
            {
                r1 = (u_int32_t)(lb1[xr]);

                g1 = (u_int32_t)(lb1[xg]);

                b1 = (u_int32_t)(lb1[xb]);
            }
            else
            {
                r1 = 0;

                g1 = 0;

                b1 = 0;
            }

            if (lb2)
            {
                r2 = (u_int32_t)(lb2[xr]);

                g2 = (u_int32_t)(lb2[xg]);

                b2 = (u_int32_t)(lb2[xb]);
            }
            else
            {
                r2 = 0;

                g2 = 0;

                b2 = 0;
            }
        }
        else
        {
            r1 = r2 = 0;

            g1 = g2 = 0;

            b1 = b2 = 0;
        }

        if ((sx2 >= 0) && (sx2 < p_jpeg->width))
        {
            int xr = p_jpeg->xr0 + (sx2 * p_jpeg->bytesPerPix);

            int xg = p_jpeg->xg0 + (sx2 * p_jpeg->bytesPerPix);

            int xb = p_jpeg->xb0 + (sx2 * p_jpeg->bytesPerPix);

            if (lb1)
            {
                r3 = (u_int32_t)(lb1[xr]);

                g3 = (u_int32_t)(lb1[xg]);

                b3 = (u_int32_t)(lb1[xb]);
            }
            else
            {
                r3 = 0;

                g3 = 0;

                b3 = 0;
            }

            if (lb2)
            {
                r4 = (u_int32_t)(lb2[xr]);

                g4 = (u_int32_t)(lb2[xg]);

                b4 = (u_int32_t)(lb2[xb]);
            }
            else
            {
                r4 = 0;

                g4 = 0;

                b4 = 0;
            }
        }
        else
        {
            r3 = r4 = 0;

            g3 = g4 = 0;

            b3 = b4 = 0;
        }

        r1 = (r1 * fx1 * fy1 + r2 * fx1 * fy2 + r3 * fx2 * fy1 + r4 * fx2 * fy2) / 100 / 100;

        g1 = (g1 * fx1 * fy1 + g2 * fx1 * fy2 + g3 * fx2 * fy1 + g4 * fx2 * fy2) / 100 / 100;

        b1 = (b1 * fx1 * fy1 + b2 * fx1 * fy2 + b3 * fx2 * fy1 + b4 * fx2 * fy2) / 100 / 100;

#if defined(DSRT_FEATURE_SHADOW)
        if (p_opts->b_shadow)
        {
            r1 = (r1 * p_opts->i_shadow) / 100u;

            g1 = (g1 * p_opts->i_shadow) / 100u;

            b1 = (b1 * p_opts->i_shadow) / 100u;
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
    unsigned int const y)
{
    struct dsrt_display const * const p_display = p_ctxt->p_display;

    int x_offset;

    int y_offset;

    x_offset = ((p_ctxt->p_pixmap->width - p_ctxt->p_image->width) / 2);

    y_offset = ((p_ctxt->p_pixmap->height - p_ctxt->p_image->height) / 2);

    /* put */
    XPutImage(
        p_display->dis,
        p_ctxt->p_pixmap->pixmap,
        p_display->copyGC,
        p_ctxt->p_image->img,
        0,
        0,
        x_offset,
        y_offset + y,
        p_ctxt->p_image->width,
        1);
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

    signed long int sy;

    signed long int frac;

    signed long int sy1;

    signed long int sy2;

    signed long int fy1;

    signed long int fy2;

#if defined(DSRT_FEATURE_ZOOM)
    struct dsrt_zoom const * const p_zoom = p_ctxt->p_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    int i_source_y1;

    int i_source_height;

    XSetForeground(p_ctxt->p_display->dis, p_ctxt->p_display->copyGC, BlackPixel(p_ctxt->p_display->dis, p_ctxt->p_display->screen));

    XFillRectangle(p_ctxt->p_display->dis, p_ctxt->p_pixmap->pixmap, p_ctxt->p_display->copyGC, 0, 0, p_ctxt->p_pixmap->width, p_ctxt->p_pixmap->height);

#if defined(DSRT_FEATURE_ZOOM)
    i_source_y1 = p_zoom->y1;

    i_source_height = p_zoom->y2 - p_zoom->y1;
#else /* #if defined(DSRT_FEATURE_ZOOM) */
    i_source_y1 = 0;

    i_source_height = p_ctxt->p_jpeg->height;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    for (dy = 0; dy < p_ctxt->p_image->height; ++dy)
    {
        /* Clear the line */
        if (p_ctxt->p_display->depth > 16)
        {
            memset(p_ctxt->p_image->img->data, 0, p_ctxt->p_image->width * 4);
        }
        else
        {
            memset(p_ctxt->p_image->img->data, 0, p_ctxt->p_image->width * 2);
        }

        /* Calculate srcy */
        sy = (((dy * 100 + 50) * i_source_height) / p_ctxt->p_image->height);

        frac = sy % 100;

        sy /= 100;

        sy += i_source_y1;

        if (frac <= 50)
        {
            sy1 = sy;

            sy2 = sy - 1;

            fy1 = 50 + frac;

            fy2 = 50 - frac;
        }
        else
        {
            sy1 = sy;

            sy2 = sy + 1;

            fy1 = 150 - frac;

            fy2 = frac - 50;
        }

        dsrt_main_convert_line(p_ctxt, sy1, fy1, sy2, fy2);

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
    int * const p_pixmap_width,
    int * const p_pixmap_height,
    int * const p_image_width,
    int * const p_image_height)
{
    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    struct dsrt_view const * const p_view = p_ctxt->p_view;

#if defined(DSRT_FEATURE_ZOOM)
    struct dsrt_zoom const * const p_zoom = p_ctxt->p_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    int i_pixmap_width;

    int i_pixmap_height;

    int i_image_width;

    int i_image_height;

    int i_source_width;

    int i_source_height;

#if defined(DSRT_FEATURE_ZOOM)
    i_source_width = p_zoom->x2 - p_zoom->x1;

    i_source_height = p_zoom->y2 - p_zoom->y1;
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

    int i_pixmap_width;

    int i_pixmap_height;

    int i_image_width;

    int i_image_height;

#if defined(DSRT_FEATURE_ZOOM)
    dsrt_zoom_setup(p_ctxt, p_ctxt->p_jpeg->width, p_ctxt->p_jpeg->height);
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    dsrt_main_select_pixmap_size(p_ctxt, &i_pixmap_width, &i_pixmap_height, &i_image_width, &i_image_height);

#if defined(DSRT_FEATURE_LOG)
    {
#if defined(DSRT_FEATURE_ZOOM)
        char a_zoom[32];

        sprintf(a_zoom, "%ldx%ld+%ld+%ld",
            p_ctxt->p_zoom->x2 - p_ctxt->p_zoom->x1,
            p_ctxt->p_zoom->y2 - p_ctxt->p_zoom->y1,
            p_ctxt->p_zoom->x1,
            p_ctxt->p_zoom->y1);
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
    struct dsrt_main * const p_main)
{
    struct dsrt_ctxt * const p_ctxt = &p_main->o_ctxt;

    p_ctxt->p_main = p_main;

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

    struct dsrt_main o_main;

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
