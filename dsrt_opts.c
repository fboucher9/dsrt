/* See LICENSE for license details. */

/*

Module: dsrt_opts.c

Description:

    Command line options for dsrt application.

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_opts.h"

/*

Function: dsrt_opts_init

Description:

*/
char
dsrt_opts_init(
    struct dsrt_ctxt const * const p_ctxt,
    int const argc,
    char const * const * const argv)
{
    char b_result;

    char b_valid;

    int argi;

    struct dsrt_opts * const p_opts = p_ctxt->p_opts;

    p_opts->p_filename = NULL;

#if defined(DSRT_FEATURE_CENTER)
    p_opts->b_center = 0;
#endif /* #if defined(DSRT_FEATURE_CENTER) */

    b_valid = 1;

    argi = 1;

    while (b_valid && (argi < argc))
    {
#if defined(DSRT_FEATURE_CENTER)
        if (0 == strcmp(argv[argi], "--center"))
        {
            p_opts->b_center = 1;

            argi ++;
        }
        else
#endif /* #if defined(DSRT_FEATURE_CENTER) */
        if (0 == strcmp(argv[argi], "--"))
        {
            argi ++;

            b_valid = 0;
        }
        else
        {
            b_valid = 0;
        }
    }

    if (argi < argc)
    {
        p_opts->p_filename = argv[argi];

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

/*

Function: dsrt_opts_cleanup

Description:

*/
void
dsrt_opts_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    /* Unused parameter */
    (void)(p_ctxt);

} /* dsrt_opts_cleanup() */

/* end-of-file: dsrt_opts.c */
