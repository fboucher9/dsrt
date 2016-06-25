/* See LICENSE for license details. */

/*

Module: dsrt_main.h

Description:

    Main entry-point for dsrt application.

*/

/* Reverse include guard */
#if defined(INC_DSRT_MAIN_H)
#error include dsrt_main.h once
#endif /* #if defined(INC_DSRT_MAIN_H) */

#define INC_DSRT_MAIN_H

/* --- Interface --- */

int
dsrt_main(
    int const argc,
    char const * const * const argv);

/* end-of-file: dsrt_main.h */
