/* See LICENSE for license details. */

/*

Module: dsrt_view.c

Description:

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_view.h"

/* Options */
#include "dsrt_opts.h"

/* Display */
#include "dsrt_display.h"

/*

Function: dsrt_view_init()

Description:

*/
char
dsrt_view_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

    struct dsrt_view * const p_view = p_ctxt->p_view;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

#if defined(DSRT_FEATURE_PREVIEW)
    if (p_opts->b_preview)
    {
        Window i_parent;

#if defined(DSRT_FEATURE_EMBED)
        if (p_opts->b_embed)
        {
            XWindowAttributes wa;

            i_parent = p_opts->i_embed;

            memset(&wa, 0, sizeof(wa));

            XGetWindowAttributes(p_display->dis, i_parent, &wa);

            p_view->width = wa.width;

            p_view->height = wa.height;

            XSelectInput(
                p_display->dis,
                i_parent,
                StructureNotifyMask);
        }
        else
#endif /* #if defined(DSRT_FEATURE_EMBED) */
        {
            i_parent = p_display->root;

            p_view->width = 320;

            p_view->height = 200;
        }

        p_view->h = XCreateSimpleWindow(
            p_display->dis,
            i_parent,
            0,
            0,
            p_view->width,
            p_view->height,
            0,
            0,
            0);

        XMapRaised(
            p_display->dis,
            p_view->h);

        XSelectInput(
            p_display->dis,
            p_view->h,
            KeyPressMask | StructureNotifyMask);
    }
    else
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */
    {
        p_view->h = p_display->root;

        p_view->width = DisplayWidth(p_display->dis, p_display->screen);

        p_view->height = DisplayHeight(p_display->dis, p_display->screen);
    }

    b_result = 1;

    return b_result;

} /* dsrt_view_init() */

/*

Function: dsrt_view_cleanup()

Description:

*/
void
dsrt_view_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    (void)(p_ctxt);
} /* dsrt_view_cleanup() */

/*

Function: dsrt_view_event()

Description:

*/
char
dsrt_view_event(
    struct dsrt_ctxt const * const p_ctxt)
{
    char c_event;

    struct dsrt_view * const p_view = p_ctxt->p_view;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    struct dsrt_display const * const p_display = p_ctxt->p_display;

    c_event = 'q';

#if defined(DSRT_FEATURE_PREVIEW)
    if (p_opts->b_preview)
    {
        char b_continue;

        int i_timeout;

        b_continue = 1;

        i_timeout = 0;

        while (b_continue)
        {
            XEvent o_event;

            if (XPending(p_display->dis))
            {
                XNextEvent(
                    p_display->dis,
                    &(
                        o_event));

                if (KeyPress == o_event.type)
                {
                    KeySym const ks = XLookupKeysym(&(o_event.xkey), 0);

                    if (XK_q == ks)
                    {
                        c_event = 'q';

                        b_continue = 0;
                    }
                    else if (XK_space == ks)
                    {
                        c_event = ' ';

                        b_continue = 0;
                    }
                }
                else if (ConfigureNotify == o_event.type)
                {
                    if (o_event.xconfigure.window == p_opts->i_embed)
                    {
                        XWindowChanges wc;

                        memset(&wc, 0, sizeof(wc));

                        wc.width = o_event.xconfigure.width;

                        wc.height = o_event.xconfigure.height;

                        XConfigureWindow(p_display->dis, p_view->h, CWWidth|CWHeight, &wc);
                    }
                    else if (o_event.xconfigure.window == p_view->h)
                    {
                        if (o_event.xconfigure.width != p_view->width)
                        {
                            p_view->width = o_event.xconfigure.width;

                            i_timeout = 10; /* 160 msec */
                        }

                        if (o_event.xconfigure.height != p_view->height)
                        {
                            p_view->height = o_event.xconfigure.height;

                            i_timeout = 10; /* 160 msec */
                        }
                    }
                }
            }
            else
            {
                usleep(16ul * 1000ul);

                if (i_timeout)
                {
                    i_timeout --;

                    if (!i_timeout)
                    {
                        c_event = ' ';

                        b_continue = 0;
                    }
                }
            }
        }
    }
    else
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */
    {
        /* Grab keys from standard input */
    }

    return c_event;

} /* dsrt_view_event() */

/* end-of-file: dsrt_view.c */
