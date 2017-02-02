/* See LICENSE for license details. */

/*

Module: dsrt_cfg.h

Description:

    Configuration options.

*/

/* Reverse include guard */
#if defined(INC_DSRT_CFG_H)
#error include dsrt_cfg.h once
#endif /* #if defined(INC_DSRT_CFG_H) */

#define INC_DSRT_CFG_H

/*

    List of features to include at compile-time.
    Uncomment features that are required.
    Comment features that are not required.

*/

/* Center the image in root window */
#define DSRT_FEATURE_CENTER

/* Detection of byte order */
#define DSRT_FEATURE_MSB

/* Display error messages on stderr */
#define DSRT_FEATURE_LOG

/* Preview the image in a new window */
#define DSRT_FEATURE_PREVIEW

/* Embed the preview window into another */
#if defined(DSRT_FEATURE_PREVIEW)
#define DSRT_FEATURE_EMBED
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */

/* end-of-file: dsrt_cfg.h */
