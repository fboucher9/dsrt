/* See LICENSE for license details. */

/*

Module: dsrt_jpeg.h

Description:

*/

/* Reverse include guard */
#if defined(INC_DSRT_JPEG_H)
#error include dsrt_jpeg.h once
#endif /* #if defined(INC_DSRT_JPEG_H) */

#define INC_DSRT_JPEG_H

/* Predefine context handle */
struct dsrt_ctxt;

/*

Structure: dsrt_jpeg

Description:

*/
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

/* --- Interface --- */

char
dsrt_jpeg_init(
    struct dsrt_ctxt const * const p_ctxt,
    char const * const p_filename);

void
dsrt_jpeg_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_jpeg.h */