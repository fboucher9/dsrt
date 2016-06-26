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

struct dsrt_ctxt
{
    struct dsrt_main * p_main;

    struct dsrt_display * p_display;

    struct dsrt_opts * p_opts;

}; /* struct dsrt_ctxt */

struct dsrt_display
{
    Display * dis;

    Visual * vis;

    Window root;

    int screen;

    int depth;

    GC copyGC;

};

struct dsrt_opts
{
    char b_shadow;

    char b_center;

};

struct dsrt_main
{
    struct dsrt_ctxt o_ctxt;

    struct dsrt_display o_display;

    struct dsrt_opts o_opts;

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

struct dsrt_jpeg
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr err_mgr;
    FILE * inFile;
    JSAMPARRAY lineBuf;
    int width;
    int height;
    int bytesPerPix;
    unsigned long int red_mask;
    unsigned long int green_mask;
    unsigned long int blue_mask;
    double rRatio;
    double gRatio;
    double bRatio;
};

static
char
dsrt_jpeg_init(
    struct dsrt_ctxt const * const p_ctxt,
    struct dsrt_jpeg * const p_dec,
    char const * const filename)
{
    char b_result;

    p_dec->inFile = fopen (filename, "rb");

    if (p_dec->inFile)
    {
        p_dec->cinfo.err = jpeg_std_error (&p_dec->err_mgr);

        p_dec->err_mgr.error_exit = &jpeg_error_exit;

        jpeg_create_decompress (&p_dec->cinfo);

        jpeg_stdio_src (&p_dec->cinfo, p_dec->inFile);

        jpeg_read_header (&p_dec->cinfo, 1);

        p_dec->cinfo.do_fancy_upsampling = 0;

        p_dec->cinfo.do_block_smoothing = 0;

        jpeg_start_decompress (&p_dec->cinfo);

        p_dec->width = p_dec->cinfo.output_width;

        p_dec->height = p_dec->cinfo.output_height;

        p_dec->bytesPerPix = p_dec->cinfo.output_components;

        p_dec->lineBuf =
            p_dec->cinfo.mem->alloc_sarray (
                (j_common_ptr) &p_dec->cinfo,
                JPOOL_IMAGE,
                (p_dec->width * p_dec->bytesPerPix),
                1);

        {
            p_dec->red_mask = p_ctxt->p_display->vis->red_mask;

            p_dec->green_mask = p_ctxt->p_display->vis->green_mask;

            p_dec->blue_mask = p_ctxt->p_display->vis->blue_mask;
        }

        p_dec->rRatio = p_dec->red_mask / 255.0;

        p_dec->gRatio = p_dec->green_mask / 255.0;

        p_dec->bRatio = p_dec->blue_mask / 255.0;

        b_result = 1;
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
    struct dsrt_jpeg * const p_dec)
{
    jpeg_finish_decompress (&p_dec->cinfo);

    jpeg_destroy_decompress (&p_dec->cinfo);

    fclose (p_dec->inFile);

}

struct dsrt_img
{
    Drawable pixmap;

    int width;

    int height;

};

static
XImage *
dsrt_ximage_create(
    struct dsrt_ctxt const * const p_ctxt,
    int const width)
{
    XImage * img;

    size_t const i_len = ((((p_ctxt->p_display->depth + 15) & ~15) / 8) * (width * 1));

    void * const p_data = malloc(i_len);

    if (p_data)
    {
        img = XCreateImage(
            p_ctxt->p_display->dis,
            CopyFromParent,
            p_ctxt->p_display->depth,
            ZPixmap,
            0,
            p_data,
            width,
            1,
            ((p_ctxt->p_display->depth + 15) & ~15),
            0);

        if (img)
        {
            XInitImage (img);

            /*Set the client's byte order, so that XPutImage knows what to do with the data.*/
            /*The default in a new X image is the server's format, which may not be what we want.*/
#if defined(DSRT_FEATURE_MSB)

            if ((LSBFirst == get_byte_order ()))
            {
                img->byte_order = LSBFirst;
            }
            else
            {
                img->byte_order = MSBFirst;
            }
#else /* #if defined(DSRT_FEATURE_MSB) */

            img->byte_order = LSBFirst;

#endif /* #if defined(DSRT_FEATURE_MSB) */

            /*The bitmap_bit_order doesn't matter with ZPixmap images.*/
            img->bitmap_bit_order = MSBFirst;
        }
    }
    else
    {
        img = NULL;
    }

    return img;

}

static
char
dsrt_jpeg_scan(
    struct dsrt_ctxt const * const p_ctxt,
    struct dsrt_jpeg * const p_dec,
    struct dsrt_img * const p_img)
{
    char b_result;

    XImage * const img = dsrt_ximage_create(p_ctxt, p_dec->width);

    if (img)
    {
        if ((3 == p_dec->bytesPerPix) ||
            (1 == p_dec->bytesPerPix))
        {
            int const xr0 = 0 % p_dec->bytesPerPix;
            int const xg0 = 1 % p_dec->bytesPerPix;
            int const xb0 = 2 % p_dec->bytesPerPix;
            int const lineOffset = (p_dec->width * p_dec->bytesPerPix);
            unsigned int y;

            for (y = 0; y < p_dec->cinfo.output_height; ++y)
            {
                int xr;
                int xg;
                int xb;
                u_int32_t * newBuf32 = (u_int32_t *)(img->data);
                u_int16_t * newBuf16 = (u_int16_t *)(img->data);

                jpeg_read_scanlines (&p_dec->cinfo, p_dec->lineBuf, 1);

                for (xr = xr0, xg = xg0, xb = xb0; xr < lineOffset; xr += p_dec->bytesPerPix, xg += p_dec->bytesPerPix, xb += p_dec->bytesPerPix)
                {
                    u_int32_t const col =
                        (u_int32_t)(
                            ((u_int32_t)(p_dec->lineBuf[0][xr] * p_dec->rRatio) & p_dec->red_mask) |
                            ((u_int32_t)(p_dec->lineBuf[0][xg] * p_dec->gRatio) & p_dec->green_mask) |
                            ((u_int32_t)(p_dec->lineBuf[0][xb] * p_dec->bRatio) & p_dec->blue_mask));

                    if (p_ctxt->p_display->depth > 16)
                    {
                        *newBuf32 = col;
                        ++newBuf32;
                    }
                    else
                    {
                        *newBuf16 = (u_int16_t)col;
                        ++newBuf16;
                    }
                }

                /* put */
                XPutImage(
                    p_ctxt->p_display->dis,
                    p_img->pixmap,
                    p_ctxt->p_display->copyGC,
                    img,
                    0,
                    0,
                    0,
                    y,
                    p_img->width,
                    1);
            }

            b_result = 1;
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf (stderr, "Error: the number of color channels is %d.  This program only handles 1 or 3\n", p_dec->bytesPerPix);
#endif /* #if defined(DSRT_FEATURE_LOG) */

            b_result = 0;
        }

        XDestroyImage(img);
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "error out of memory\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        b_result = 0;
    }

    return b_result;
}

/*This returns an array for a 24 bit image.*/
static
char
dsrt_main_create_image (
    struct dsrt_ctxt const * const p_ctxt,
    char const *filename,
    struct dsrt_img * const p_img)
{
    char b_result;

    struct dsrt_jpeg o_dec;

    if (dsrt_jpeg_init(p_ctxt, &o_dec, filename))
    {
        p_img->width = o_dec.width;

        p_img->height = o_dec.height;

        p_img->pixmap = XCreatePixmap(
            p_ctxt->p_display->dis,
            p_ctxt->p_display->root,
            o_dec.width,
            o_dec.height,
            p_ctxt->p_display->depth);

        b_result = dsrt_jpeg_scan(p_ctxt, &o_dec, p_img);

        dsrt_jpeg_cleanup(&o_dec);
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "error\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        b_result = 0;
    }

    return b_result;

}

static
int
dsrt_main_show_file(
    struct dsrt_ctxt const * const p_ctxt,
    char const * const p_file_name)
{
    int i_result;

    struct dsrt_img o_img;

    if (dsrt_main_create_image (p_ctxt, p_file_name, &o_img))
    {
        Drawable pixmap;

#if 0 /* defined(DSRT_FEATURE_CENTER) */
        /* Todo: command line options for -fit or -tile */
        /* Todo: default background color for -fit */
        {
            int sw = DisplayWidth(p_ctxt->p_display->dis, p_ctxt->p_display->screen);
            int sh = DisplayHeight(p_ctxt->p_display->dis, p_ctxt->p_display->screen);
            int const x = ((sw-o_img.width)/2);
            int const y = ((sh-o_img.height)/2);
            pixmap = XCreatePixmap(p_ctxt->p_display->dis, p_ctxt->p_display->root, sw, sh, p_ctxt->p_display->depth);
            XPutImage (p_ctxt->p_display->dis, pixmap, p_ctxt->p_display->copyGC, o_img.img, 0, 0, x, y, o_img.width, o_img.height);
        }
#else /* #if defined(DSRT_FEATURE_CENTER) */
        pixmap = o_img.pixmap;
#endif /* #if defined(DSRT_FEATURE_CENTER) */

        XSetWindowBackgroundPixmap(p_ctxt->p_display->dis, p_ctxt->p_display->root, pixmap);
        XClearWindow(p_ctxt->p_display->dis, p_ctxt->p_display->root);
        XFlush(p_ctxt->p_display->dis);
        i_result = 0;
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "unable to create X image\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        i_result = 1;
    }

    return i_result;
}

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

        return 1;
    }
    else
    {
        return 0;
    }

} /* dsrt_display_init() */

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

    o_main.o_ctxt.p_main = &o_main;

    o_main.o_ctxt.p_display = &o_main.o_display;

    o_main.o_ctxt.p_opts = &o_main.o_opts;

    if (dsrt_display_init(&o_main.o_ctxt))
    {
        if (2 <= argc)
        {
            i_result = dsrt_main_show_file(&o_main.o_ctxt, argv[1]);
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf (stderr, "please specify a filename\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */
            i_result = 1;
        }
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "unable to open X display\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */
        i_result = 1;
    }

    return i_result;

} /* dsrt_main() */

/* end-of-file: dsrt_main.c */
