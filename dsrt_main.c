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

/* Module */
#include "dsrt_main.h"

/* Context */
struct dsrt_main;

struct dsrt_display;

struct dsrt_opts;

struct dsrt_jpeg;

struct dsrt_image;

struct dsrt_pixmap;

struct dsrt_ctxt
{
    struct dsrt_main * p_main;

    struct dsrt_display * p_display;

    struct dsrt_opts * p_opts;

    struct dsrt_jpeg * p_jpeg;

    struct dsrt_image * p_image;

    struct dsrt_pixmap * p_pixmap;

}; /* struct dsrt_ctxt */

struct dsrt_display
{
    Display * dis;

    Visual * vis;

    GC copyGC;

    Window root;

    unsigned long int red_mask;

    unsigned long int green_mask;

    unsigned long int blue_mask;

    double rRatio;

    double gRatio;

    double bRatio;

    int screen;

    int depth;

}; /* struct dsrt_display */

struct dsrt_opts
{
    char const * p_filename;

    char b_shadow;

    char b_center;

}; /* struct dsrt_opts */

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

struct dsrt_pixmap
{
    Drawable pixmap;

    int width;

    int height;

}; /* struct dsrt_pixmap */

struct dsrt_jpeg
{
    struct jpeg_decompress_struct cinfo;

    struct jpeg_error_mgr err_mgr;

    FILE * inFile;

    JSAMPARRAY lineBuf;

    int width;

    int height;

    int bytesPerPix;

    int xr0;

    int xg0;

    int xb0;

    int lineOffset;

}; /* struct dsrt_jpeg */

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

static
void
jpeg_error_exit(
    j_common_ptr cinfo)
{
    cinfo->err->output_message (cinfo);
    exit (EXIT_FAILURE);
}

static
char
dsrt_jpeg_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    p_jpeg->inFile = fopen (p_opts->p_filename, "rb");

    if (p_jpeg->inFile)
    {
        p_jpeg->cinfo.err = jpeg_std_error (&p_jpeg->err_mgr);

        p_jpeg->err_mgr.error_exit = &jpeg_error_exit;

        jpeg_create_decompress (&p_jpeg->cinfo);

        jpeg_stdio_src (&p_jpeg->cinfo, p_jpeg->inFile);

        jpeg_read_header (&p_jpeg->cinfo, 1);

        p_jpeg->cinfo.do_fancy_upsampling = 0;

        p_jpeg->cinfo.do_block_smoothing = 0;

        jpeg_start_decompress (&p_jpeg->cinfo);

        p_jpeg->width = p_jpeg->cinfo.output_width;

        p_jpeg->height = p_jpeg->cinfo.output_height;

        p_jpeg->bytesPerPix = p_jpeg->cinfo.output_components;

        p_jpeg->lineOffset = (p_jpeg->width * p_jpeg->bytesPerPix);

        p_jpeg->lineBuf =
            p_jpeg->cinfo.mem->alloc_sarray (
                (j_common_ptr) &p_jpeg->cinfo,
                JPOOL_IMAGE,
                p_jpeg->lineOffset,
                1);

        if (3 == p_jpeg->bytesPerPix)
        {
            p_jpeg->xr0 = 0;

            p_jpeg->xg0 = 1;

            p_jpeg->xb0 = 2;

            b_result = 1;
        }
        else if (1 == p_jpeg->bytesPerPix)
        {
            p_jpeg->xr0 = 0;

            p_jpeg->xg0 = 0;

            p_jpeg->xb0 = 0;

            b_result = 1;
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf (stderr, "Error: the number of color channels is %d.  This program only handles 1 or 3\n", p_jpeg->bytesPerPix);
#endif /* #if defined(DSRT_FEATURE_LOG) */

            b_result = 0;
        }
    }
    else
    {
        b_result = 0;
    }

    return b_result;

} /* dsrt_jpeg_init() */

static
void
dsrt_jpeg_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    if (p_jpeg->inFile)
    {
        jpeg_finish_decompress (&p_jpeg->cinfo);

        jpeg_destroy_decompress (&p_jpeg->cinfo);

        fclose (p_jpeg->inFile);

        p_jpeg->inFile = NULL;
    }
}

static
char
dsrt_image_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    struct dsrt_image * const p_image = p_ctxt->p_image;

    struct dsrt_jpeg const * const p_jpeg = p_ctxt->p_jpeg;

    int const depth_bits_rounded = ((p_ctxt->p_display->depth + 15) & ~15);

    int const depth_bytes = (depth_bits_rounded / 8);

    size_t const i_len = (depth_bytes * p_jpeg->width);

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
            p_jpeg->width,
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

static
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

static
void
dsrt_main_read_line(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    jpeg_read_scanlines (&p_jpeg->cinfo, p_jpeg->lineBuf, 1);
}

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

    int x_offset = 0;

    int y_offset = 0;

#if defined(DSRT_FEATURE_CENTER)
    {
        /* Todo: command line options for -fit or -tile */
        /* Todo: default background color for -fit */
        int const sw = DisplayWidth(p_ctxt->p_display->dis, p_ctxt->p_display->screen);

        int const sh = DisplayHeight(p_ctxt->p_display->dis, p_ctxt->p_display->screen);

        x_offset = ((sw - p_jpeg->width) / 2);

        y_offset = ((sh - p_jpeg->height) / 2);
    }
#endif /* #if defined(DSRT_FEATURE_CENTER) */

    /* put */
    XPutImage(
        p_ctxt->p_display->dis,
        p_ctxt->p_pixmap->pixmap,
        p_ctxt->p_display->copyGC,
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
    dsrt_main_read_line(p_ctxt);

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

static
char
dsrt_pixmap_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    struct dsrt_pixmap * const p_pixmap = p_ctxt->p_pixmap;

#if defined(DSRT_FEATURE_CENTER)
    p_pixmap->width = DisplayWidth(p_ctxt->p_display->dis, p_ctxt->p_display->screen);

    p_pixmap->height = DisplayHeight(p_ctxt->p_display->dis, p_ctxt->p_display->screen);
#else
    {
        struct dsrt_jpeg const * const p_jpeg = p_ctxt->p_jpeg;

        p_pixmap->width = p_jpeg->width;

        p_pixmap->height = p_jpeg->height;
    }
#endif /* #if defined(DSRT_FEATURE_CENTER) */

    p_pixmap->pixmap = XCreatePixmap(
        p_ctxt->p_display->dis,
        p_ctxt->p_display->root,
        p_pixmap->width,
        p_pixmap->height,
        p_ctxt->p_display->depth);

    b_result = 1;

    return b_result;

} /* dsrt_pixmap_init() */

static
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

static
void
dsrt_pixmap_apply(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_pixmap const * const p_pixmap = p_ctxt->p_pixmap;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    XSetWindowBackgroundPixmap(
        p_display->dis,
        p_display->root,
        p_pixmap->pixmap);

    XClearWindow(
        p_display->dis,
        p_display->root);

    XFlush(p_display->dis);

} /* dsrt_pixmap_apply() */

static
char
dsrt_main_show_file(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    if (dsrt_jpeg_init(p_ctxt))
    {
        char b_jpeg_cleanup = 1;

        if (dsrt_pixmap_init(p_ctxt))
        {
            char b_pixmap_cleanup = 1;

            if (dsrt_image_init(p_ctxt))
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

/*

Function: dsrt_display_init

Description:

    Initialize dsrt_display module.

*/
static
char
dsrt_display_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_display * const p_display = p_ctxt->p_display;

    p_display->dis = XOpenDisplay(NULL);

    if (p_display->dis)
    {
        p_display->root = DefaultRootWindow(p_display->dis);

        p_display->screen = DefaultScreen(p_display->dis);

        p_display->depth = DefaultDepth(p_display->dis, p_display->screen);

        p_display->vis = DefaultVisual(p_display->dis, p_display->screen);

        p_display->copyGC = XCreateGC(p_display->dis, p_display->root, 0, NULL);

        p_display->red_mask = p_display->vis->red_mask;

        p_display->green_mask = p_display->vis->green_mask;

        p_display->blue_mask = p_display->vis->blue_mask;

        p_display->rRatio = p_display->red_mask / 255.0;

        p_display->gRatio = p_display->green_mask / 255.0;

        p_display->bRatio = p_display->blue_mask / 255.0;

        return 1;
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "unable to open X display\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        return 0;
    }

} /* dsrt_display_init() */

/*

Function: dsrt_display_cleanup()

Description:

    Free all resources allocated by dsrt_display module.

*/
static
void
dsrt_display_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_display * const p_display = p_ctxt->p_display;

    if (p_display->dis)
    {
        XCloseDisplay(p_display->dis);

        p_display->dis = NULL;

        p_display->vis = NULL;
    }

} /* dsrt_display_cleanup() */

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

static
char
dsrt_opts_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const argc,
    char const * const * const argv)
{
    char b_result;

    struct dsrt_opts * const p_opts = p_ctxt->p_opts;

    if (2 <= argc)
    {
        p_opts->p_filename = argv[1];

        b_result = 1;
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf (stderr, "please specify a filename\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        b_result = 0;
    }

    return b_result;

} /* dsrt_opts_init() */

static
void
dsrt_opts_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    /* Unused parameter */
    (void)(p_ctxt);

} /* dsrt_opts_cleanup() */

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
