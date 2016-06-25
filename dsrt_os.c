/* See LICENSE for license details. */

/*

Module: dsrt_os.c

Description:

    OS-specific application entry point.

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Main module */
#include "dsrt_main.h"

/*

Function: main()

Description:

    OS-specific application entry point.

*/
int
main(
    int argc,
    char * argv[])
{
    return dsrt_main(
        argc,
        (char const * const *)(argv));
} /* main() */

/* end-of-file: dsrt_os.c */
