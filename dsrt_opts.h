/* See LICENSE for license details. */

/* Reverse include guard */
#if defined(INC_DSRT_OPTS_H)
#error include dsrt_opts.h once
#endif /* #if defined(INC_DSRT_OPTS_H) */

#define INC_DSRT_OPTS_H

struct dsrt_opts
{
    char const * p_filename;

    /* char b_shadow; */

#if defined(DSRT_FEATURE_CENTER)
    char b_center;
#endif /* #if defined(DSRT_FEATURE_CENTER) */

}; /* struct dsrt_opts */

char
dsrt_opts_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const argc,
    char const * const * const argv);

void
dsrt_opts_cleanup(
    struct dsrt_ctxt const * const p_ctxt);

/* end-of-file: dsrt_opts.h */
