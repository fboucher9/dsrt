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

};

void XDestroyImage(XImage* p_image);

static
void
dsrt_main_convert_line(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    u_int32_t * newBuf32 = (u_int32_t *)(p_ctxt->p_image->img->data);

    u_int16_t * newBuf16 = (u_int16_t *)(p_ctxt->p_image->img->data);

    int xr = p_jpeg->xr0;

    int xg = p_jpeg->xg0;

    int xb = p_jpeg->xb0;

    while (xr < p_jpeg->lineOffset)
    {
        u_int32_t const col =
            (u_int32_t)(
                ((u_int32_t)(p_jpeg->lineBuf[0][xr] * p_display->rRatio) & p_display->red_mask) |
                ((u_int32_t)(p_jpeg->lineBuf[0][xg] * p_display->gRatio) & p_display->green_mask) |
                ((u_int32_t)(p_jpeg->lineBuf[0][xb] * p_display->bRatio) & p_display->blue_mask));

        if (p_display->depth > 16)
        {
            *newBuf32 = col;
            ++newBuf32;
        }
        else
        {
            *newBuf16 = (u_int16_t)col;
            ++newBuf16;
        }

        xr += p_jpeg->bytesPerPix;

        xg += p_jpeg->bytesPerPix;

        xb += p_jpeg->bytesPerPix;
    }
}

static
void
dsrt_main_write_line(
    struct dsrt_ctxt const * const p_ctxt,
    unsigned int const y)
{
    struct dsrt_jpeg const * const p_jpeg = p_ctxt->p_jpeg;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    int x_offset;

    int y_offset;

    (void)(p_opts);

#if defined(DSRT_FEATURE_CENTER)
    if (p_opts->b_center)
    {
        /* Todo: command line options for -fit or -tile */
        /* Todo: default background color for -fit */

        x_offset = ((p_ctxt->p_pixmap->width - p_jpeg->width) / 2);

        y_offset = ((p_ctxt->p_pixmap->height - p_jpeg->height) / 2);
    }
    else
#endif /* #if defined(DSRT_FEATURE_CENTER) */
    {
        x_offset = 0;

        y_offset = 0;
    }

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
        p_jpeg->width,
        1);
}

static
void
dsrt_main_scan_line(
    struct dsrt_ctxt const * const p_ctxt,
    unsigned int y)
{
    dsrt_jpeg_read_line(p_ctxt);

    dsrt_main_convert_line(p_ctxt);

    dsrt_main_write_line(p_ctxt, y);

} /* dsrt_main_scan_line() */

/*

Function: dsrt_main_scan()

Description:

*/
static
void
dsrt_main_scan(
    struct dsrt_ctxt const * const p_ctxt)
{
    unsigned int y;

    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    for (y = 0; y < p_jpeg->cinfo.output_height; ++y)
    {
        dsrt_main_scan_line(
            p_ctxt,
            y);
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
    int * const p_pixmap_height)
{
    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    int i_pixmap_width;

    int i_pixmap_height;

#if defined(DSRT_FEATURE_CENTER)
    if (p_opts->b_center)
    {
#if defined(DSRT_FEATURE_EMBED)
        if (p_opts->b_embed)
        {
            XWindowAttributes wa;

            memset(&wa, 0, sizeof(wa));

            XGetWindowAttributes(p_display->dis, p_opts->i_embed, &wa);

            i_pixmap_width = wa.width;

            i_pixmap_height = wa.height;
        }
        else
#endif /* #if defined(DSRT_FEATURE_EMBED) */
        {
            i_pixmap_width = DisplayWidth(p_display->dis, p_display->screen);

            i_pixmap_height = DisplayHeight(p_display->dis, p_display->screen);
        }
    }
    else
#endif /* #if defined(DSRT_FEATURE_CENTER) */
    {
        struct dsrt_jpeg const * const p_jpeg = p_ctxt->p_jpeg;

        i_pixmap_width = p_jpeg->width;

        i_pixmap_height = p_jpeg->height;
    }

    *(p_pixmap_width) = i_pixmap_width;

    *(p_pixmap_height) = i_pixmap_height;

} /* dsrt_main_select_pixmap_size() */

static
char
dsrt_main_show_file(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    if (dsrt_jpeg_init(p_ctxt, p_opts->p_filename))
    {
        char b_jpeg_cleanup = 1;

        int i_pixmap_width;

        int i_pixmap_height;

        dsrt_main_select_pixmap_size(p_ctxt, &i_pixmap_width, &i_pixmap_height);

        if (dsrt_pixmap_init(p_ctxt, i_pixmap_width, i_pixmap_height))
        {
            char b_pixmap_cleanup = 1;

            if (dsrt_image_init(p_ctxt, p_ctxt->p_jpeg->width))
            {
                dsrt_main_scan(p_ctxt);

                dsrt_image_cleanup(p_ctxt);

                if (b_jpeg_cleanup)
                {
                    dsrt_jpeg_cleanup(p_ctxt);

                    b_jpeg_cleanup = 0;
                }

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

        if (b_jpeg_cleanup)
        {
            dsrt_jpeg_cleanup(p_ctxt);

            b_jpeg_cleanup = 0;
        }
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "error\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

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

    dsrt_main_init_ctxt(&o_main);

    if (dsrt_display_init(&o_main.o_ctxt))
    {
        if (dsrt_opts_init(&o_main.o_ctxt, argc, argv))
        {
            if (dsrt_main_show_file(&o_main.o_ctxt))
            {
                i_result = 0;
            }
            else
            {
                i_result = 1;
            }

            dsrt_opts_cleanup(&o_main.o_ctxt);
        }
        else
        {
            i_result = 1;
        }

        dsrt_display_cleanup(&o_main.o_ctxt);
    }
    else
    {
        i_result = 1;
    }

    return i_result;

} /* dsrt_main() */

/* end-of-file: dsrt_main.c */
