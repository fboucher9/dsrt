/* See LICENSE for license details. */

/*

Module: dsrt_jpeg.c

Description:

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_jpeg.h"

static
void
jpeg_error_exit(
    j_common_ptr cinfo)
{
    cinfo->err->output_message (cinfo);
    exit (EXIT_FAILURE);
}

char
dsrt_jpeg_init(
    struct dsrt_ctxt const * const p_ctxt,
    char const * const p_filename)
{
    char b_result;

    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    p_jpeg->inFile = fopen (p_filename, "rb");

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

void
dsrt_jpeg_read_line(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_jpeg * const p_jpeg = p_ctxt->p_jpeg;

    jpeg_read_scanlines (&p_jpeg->cinfo, p_jpeg->lineBuf, 1);
}

/* end-of-file: dsrt_jpeg.c */
