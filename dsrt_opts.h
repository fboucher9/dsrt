/* See LICENSE for license details. */

/*

Module: dsrt_opts.h

Description:

    Command line options for dsrt application.

*/

/* Reverse include guard */
#if defined(INC_DSRT_OPTS_H)
#error include dsrt_opts.h once
#endif /* #if defined(INC_DSRT_OPTS_H) */

#define INC_DSRT_OPTS_H

/* Predefine context handle */
struct dsrt_ctxt;

/*

Structure: dsrt_opts

Description:

    Values of all command line options.  An instance of this structure must be
    accessible via the context handle, so that options are available from
    each function.

*/
struct dsrt_opts
{
    char * * a_filename;

#if defined(DSRT_FEATURE_EMBED)
    unsigned long int i_embed;
#endif /* #if defined(DSRT_FEATURE_EMBED) */

#if defined(DSRT_FEATURE_SHADOW)
    unsigned long int i_shadow;
#endif /* #if defined(DSRT_FEATURE_SHADOW) */

    unsigned int n_files;

#if defined(DSRT_FEATURE_GEOM)
    unsigned int i_width;

    unsigned int i_height;
#endif /* #if defined(DSRT_FEATURE_GEOM) */

#if defined(DSRT_FEATURE_ZOOM)
    int i_zoom_left;

    int i_zoom_top;

    unsigned int i_zoom_width;

    unsigned int i_zoom_height;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

    char b_fit;

#if defined(DSRT_FEATURE_GEOM)
    char b_geometry;
#endif /* #if defined(DSRT_FEATURE_GEOM) */

#if defined(DSRT_FEATURE_ZOOM)
    char b_zoom;
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

#if defined(DSRT_FEATURE_SHADOW)
    char b_shadow;
#endif /* #if defined(DSRT_FEATURE_SHADOW) */

#if defined(DSRT_FEATURE_CENTER)
    char b_center;
#endif /* #if defined(DSRT_FEATURE_CENTER) */

#if defined(DSRT_FEATURE_PREVIEW)
    char b_preview;
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */

#if defined(DSRT_FEATURE_EMBED)
    char b_embed;
#endif /* #if defined(DSRT_FEATURE_EMBED) */

#if defined(DSRT_FEATURE_GRAY)
    char b_gray;
#endif /* #if defined(DSRT_FEATURE_GRAY) */

#if defined(DSRT_FEATURE_MIRROR)
    char b_mirror_x;

    char b_mirror_y;
#endif /* #if defined(DSRT_FEATURE_MIRROR) */

    char a_padding[6u];

}; /* struct dsrt_opts */

/* --- Interface --- */

char
dsrt_opts_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const argc,
    char const * const * const argv);

void
dsrt_opts_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_opts.h */
