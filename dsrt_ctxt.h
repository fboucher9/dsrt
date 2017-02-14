/* See LICENSE for license details. */

/*

Module: dsrt_ctxt.h

Description:

    Definitions of context structures for dsrt application.

Comments:

    -   The context is a method of sharing handles between modules of dsrt
        application.

*/

/* Reverse include guard */
#if defined(INC_DSRT_CTXT_H)
#error include dsrt_ctxt.h once
#endif /* #if defined(INC_DSRT_CTXT_H) */

#define INC_DSRT_CTXT_H

/* Predefine pointer to module */
struct dsrt_main;

/* Predefine pointer to module */
struct dsrt_display;

/* Predefine pointer to module */
struct dsrt_opts;

/* Predefine pointer to module */
struct dsrt_jpeg;

/* Predefine pointer to module */
struct dsrt_image;

/* Predefine pointer to module */
struct dsrt_pixmap;

/* Predefine pointer to module */
struct dsrt_view;

/* Context */
struct dsrt_ctxt
{
    struct dsrt_main * p_main;

    struct dsrt_display * p_display;

    struct dsrt_opts * p_opts;

    struct dsrt_jpeg * p_jpeg;

    struct dsrt_image * p_image;

    struct dsrt_pixmap * p_pixmap;

    struct dsrt_view * p_view;

}; /* struct dsrt_ctxt */

/* end-of-file: dsrt_ctxt.h */
