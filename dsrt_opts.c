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

    int argi;

    struct dsrt_opts * const p_opts = p_ctxt->p_opts;

    p_opts->a_filename = (char * *)(malloc(argc * sizeof(char *)));

    p_opts->n_files = 0;

    p_opts->b_fit = 0;

#if defined(DSRT_FEATURE_CENTER)
    p_opts->b_center = 0;
#endif /* #if defined(DSRT_FEATURE_CENTER) */

#if defined(DSRT_FEATURE_SHADOW)
    p_opts->i_shadow = 100u;

    p_opts->b_shadow = 0;
#endif /* #if defined(DSRT_FEATURE_SHADOW) */

#if defined(DSRT_FEATURE_PREVIEW)

    p_opts->b_preview = 0;

#if defined(DSRT_FEATURE_EMBED)

    p_opts->i_embed = 0;

    p_opts->b_embed = 0;

#endif /* #if defined(DSRT_FEATURE_EMBED) */

#endif /* #if defined(DSRT_FEATURE_PREVIEW) */

    b_result = 1;

    argi = 1;

    while (b_result && (argi < argc))
    {
#if defined(DSRT_FEATURE_CENTER)
        if ((0 == strcmp(argv[argi], "--center"))
            || (0 == strcmp(argv[argi], "-c")))
        {
            p_opts->b_center = 1;

            argi ++;
        }
        else
#endif /* #if defined(DSRT_FEATURE_CENTER) */
#if defined(DSRT_FEATURE_PREVIEW)
        if ((0 == strcmp(argv[argi], "--preview"))
            || (0 == strcmp(argv[argi], "-v")))
        {
            p_opts->b_preview = 1;

            argi ++;
        }
        else
#if defined(DSRT_FEATURE_EMBED)
        if ((0 == strcmp(argv[argi], "--embed"))
            || (0 == strcmp(argv[argi], "-e")))
        {
            char * p_window_id;

            p_window_id = getenv("WINDOWID");

            if (p_window_id)
            {
                p_opts->b_embed = 1;

                p_opts->b_preview = 1;

                sscanf(p_window_id, "%lu", &(p_opts->i_embed));
            }

            argi ++;
        }
        else
#endif /* #if defined(DSRT_FEATURE_EMBED) */
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */
#if defined(DSRT_FEATURE_SHADOW)
        if ((0 == strcmp(argv[argi], "--shadow"))
            || (0 == strcmp(argv[argi], "-s")))
        {
            argi ++;

            if (argi < argc)
            {
                sscanf(argv[argi], "%u", &p_opts->i_shadow);

                p_opts->b_shadow = 1;

                argi ++;
            }
            else
            {
#if defined(DSRT_FEATURE_LOG)
                fprintf(stderr, "missing shadow argument\n");
#endif /* #if defined(DSRT_FEATURE_LOG) */

                b_result = 0;
            }
        }
        else
#endif /* #if defined(DSRT_FEATURE_SHADOW) */
        if ((0 == strcmp(argv[argi], "--fit"))
            || (0 == strcmp(argv[argi], "-f")))
        {
            /* */
            p_opts->b_fit = 1;

            argi ++;
        }
        else
        if (0 == strcmp(argv[argi], "--"))
        {
            argi ++;

            while (argi < argc)
            {
                p_opts->a_filename[p_opts->n_files] = strdup(argv[argi]);

                p_opts->n_files ++;

                argi ++;
            }
        }
        else if ('-' != argv[argi][0])
        {
            p_opts->a_filename[p_opts->n_files] = strdup(argv[argi]);

            p_opts->n_files ++;

            argi ++;
        }
        else
        {
            b_result = 0;
        }
    }

    if (!p_opts->n_files)
    {
        b_result = 0;
    }

#if defined(DSRT_FEATURE_LOG)
    if (!b_result)
    {
        fprintf(stderr, "Usage: dsrt [options] [--] <files>\n"
            "Options:\n"
            "  -c --center     Centered image instead of tiled\n"
            "  -v --preview    Preview image in a child window\n"
            "  -e --embed      Preview image embedded into parent\n"
            "  -f --fit        Stretch to fir into window\n"
            "  -s --shadow     Blend image with black\n");
    }
#endif /* #if defined(DSRT_FEATURE_LOG) */

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
