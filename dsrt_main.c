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
    int depth;
    unsigned long int red_mask;
    unsigned long int green_mask;
    unsigned long int blue_mask;
    double rRatio;
    double gRatio;
    double bRatio;
};

static
char
dsrt_jpeg_decoder_init(
    struct dsrt_jpeg * const p_dec,
    char const * const filename,
    Display * const dis,
    int const screen)
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

        p_dec->depth = DefaultDepth (dis, screen);

        {
            Visual * const vis = DefaultVisual (dis, screen);

            p_dec->red_mask = vis->red_mask;

            p_dec->green_mask = vis->green_mask;

            p_dec->blue_mask = vis->blue_mask;
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

} /* dsrt_jpeg_decoder_init() */

static
void
dsrt_jpeg_decoder_cleanup(
    struct dsrt_jpeg * const p_dec)
{
    jpeg_finish_decompress (&p_dec->cinfo);

    jpeg_destroy_decompress (&p_dec->cinfo);

    fclose (p_dec->inFile);

}

static
void *
dsrt_jpeg_decoder_scan(
    struct dsrt_jpeg * const p_dec)
{
    size_t const numNewBufBytes = (((p_dec->depth + 15) / 8) * (p_dec->width * p_dec->height));

    void * const imgBuf = malloc(numNewBufBytes);

    if (imgBuf)
    {
        if ((3 == p_dec->bytesPerPix) ||
            (1 == p_dec->bytesPerPix))
        {
            int const xr0 = 0 % p_dec->bytesPerPix;
            int const xg0 = 1 % p_dec->bytesPerPix;
            int const xb0 = 2 % p_dec->bytesPerPix;
            int const lineOffset = (p_dec->width * p_dec->bytesPerPix);
            unsigned int y;
            u_int32_t * newBuf32 = imgBuf;
            u_int16_t * newBuf16 = imgBuf;

            for (y = 0; y < p_dec->cinfo.output_height; ++y)
            {
                int xr;
                int xg;
                int xb;

                jpeg_read_scanlines (&p_dec->cinfo, p_dec->lineBuf, 1);

                for (xr = xr0, xg = xg0, xb = xb0; xr < lineOffset; xr += p_dec->bytesPerPix, xg += p_dec->bytesPerPix, xb += p_dec->bytesPerPix)
                {
                    u_int32_t const col =
                        (u_int32_t)(
                            ((u_int32_t)(p_dec->lineBuf[0][xr] * p_dec->rRatio) & p_dec->red_mask) |
                            ((u_int32_t)(p_dec->lineBuf[0][xg] * p_dec->gRatio) & p_dec->green_mask) |
                            ((u_int32_t)(p_dec->lineBuf[0][xb] * p_dec->bRatio) & p_dec->blue_mask));

                    if (p_dec->depth > 16)
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
            }
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf (stderr, "Error: the number of color channels is %d.  This program only handles 1 or 3\n", p_dec->bytesPerPix);
#endif /* #if defined(DSRT_FEATURE_LOG) */

            return NULL;
        }
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "error out of memory\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

        return NULL;
    }

    return imgBuf;
}

/*This returns an array for a 24 bit image.*/
static
XImage *
dsrt_main_create_image (
    Display * dis,
    int screen,
    char const *filename,
    int *widthPtr,
    int *heightPtr)
{
    XImage * img = NULL;

    struct dsrt_jpeg o_dec;

    if (dsrt_jpeg_decoder_init(&o_dec, filename, dis, screen))
    {
        void * const imgBuf = dsrt_jpeg_decoder_scan(&o_dec);

        if (imgBuf)
        {
            img = XCreateImage(
                dis,
                CopyFromParent,
                o_dec.depth,
                ZPixmap,
                0,
                imgBuf,
                o_dec.width, o_dec.height,
                ((o_dec.depth + 15) & ~15),
                0);

            XInitImage (img);

            /*Set the client's byte order, so that XPutImage knows what to do with the data.*/
            /*The default in a new X image is the server's format, which may not be what we want.*/
#if defined(DSRT_FEATURE_MSB)

            if ((LSBFirst == get_byte_order ())) {
                img->byte_order = LSBFirst;
            } else {
                img->byte_order = MSBFirst;
            }
#else /* #if defined(DSRT_FEATURE_MSB) */

            img->byte_order = LSBFirst;

#endif /* #if defined(DSRT_FEATURE_MSB) */

            /*The bitmap_bit_order doesn't matter with ZPixmap images.*/
            img->bitmap_bit_order = MSBFirst;

            dsrt_jpeg_decoder_cleanup(&o_dec);

            *widthPtr = o_dec.width;

            *heightPtr = o_dec.height;

            return img;
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf(stderr, "error\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */
            return NULL;
        }
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf(stderr, "error\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */
        return NULL;
    }

}

static
int
dsrt_main_show_file(
    char const * const p_file_name)
{
    int i_result;
    int imageWidth;
    int imageHeight;

    Display * const dis = XOpenDisplay(NULL);
    if (dis)
    {
        int const screen = DefaultScreen (dis);
        Window const root = DefaultRootWindow(dis);
        XImage * const img = dsrt_main_create_image (dis, screen, p_file_name, &imageWidth, &imageHeight);
        if (img)
        {
            GC copyGC;
            Drawable pixmap;
            copyGC = XCreateGC (dis, root, 0, NULL);
#if defined(DSRT_FEATURE_CENTER)
            /* Todo: command line options for -fit or -tile */
            /* Todo: default background color for -fit */
            {
                int sw = DisplayWidth(dis, screen);
                int sh = DisplayHeight(dis, screen);
                int const x = ((sw-imageWidth)/2);
                int const y = ((sh-imageHeight)/2);
                pixmap = XCreatePixmap(dis, root, sw, sh, DefaultDepth(dis, screen));
                XPutImage (dis, pixmap, copyGC, img, 0, 0, x, y, imageWidth, imageHeight);
            }
#else /* #if defined(DSRT_FEATURE_CENTER) */
            pixmap = XCreatePixmap(dis, root, imageWidth, imageHeight, DefaultDepth(dis, screen));
            XPutImage (dis, pixmap, copyGC, img, 0, 0, 0, 0, imageWidth, imageHeight);
#endif /* #if defined(DSRT_FEATURE_CENTER) */

            XDestroyImage(img);
            XSetWindowBackgroundPixmap(dis, root, pixmap);
            XClearWindow(dis, root);
            XFlush(dis);
            i_result = 0;
        }
        else
        {
#if defined(DSRT_FEATURE_LOG)
            fprintf(stderr, "unable to create X image\n");
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
}

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

    if (2 <= argc)
    {
        i_result = dsrt_main_show_file(argv[1]);
    }
    else
    {
#if defined(DSRT_FEATURE_LOG)
        fprintf (stderr, "please specify a filename\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */
        i_result = 1;
    }

    return i_result;

} /* dsrt_main() */

/* end-of-file: dsrt_main.c */
