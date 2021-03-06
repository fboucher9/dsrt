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

        unsigned int i_parent_width;

        unsigned int i_parent_height;

#if defined(DSRT_FEATURE_EMBED)
        if (p_opts->b_embed)
        {
            XWindowAttributes wa;

            i_parent = p_opts->i_embed;

            memset(&wa, 0, sizeof(wa));

            XGetWindowAttributes(p_display->dis, i_parent, &wa);

            i_parent_width = (unsigned int)(wa.width);

            i_parent_height = (unsigned int)(wa.height);

#if defined(DSRT_FEATURE_GEOM)
            if (p_opts->b_geometry)
            {
                p_view->width = p_opts->i_width;

                p_view->height = p_opts->i_height;
            }
            else
#endif /* #if defined(DSRT_FEATURE_GEOM) */
            {
                p_view->width = i_parent_width;

                p_view->height = i_parent_height;
            }

            XSelectInput(
                p_display->dis,
                i_parent,
                StructureNotifyMask);
        }
        else
#endif /* #if defined(DSRT_FEATURE_EMBED) */
        {
            i_parent = p_display->root;

            i_parent_width = p_display->width;

            i_parent_height = p_display->height;

#if defined(DSRT_FEATURE_GEOM)
            if (p_opts->b_geometry)
            {
                p_view->width = p_opts->i_width;

                p_view->height = p_opts->i_height;
            }
            else
#endif /* #if defined(DSRT_FEATURE_GEOM) */
            {
                p_view->width = 1024u;

                p_view->height = 768u;
            }
        }

        p_view->h = XCreateSimpleWindow(
            p_display->dis,
            i_parent,
            ((int)(i_parent_width) - (int)(p_view->width)) / 2,
            ((int)(i_parent_height) - (int)(p_view->height)) / 2,
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
            ButtonPressMask | KeyPressMask | StructureNotifyMask);
    }
    else
#endif /* #if defined(DSRT_FEATURE_PREVIEW) */
    {
        p_view->h = p_display->root;

#if defined(DSRT_FEATURE_GEOM)
        if (p_opts->b_geometry)
        {
            p_view->width = p_opts->i_width;

            p_view->height = p_opts->i_height;
        }
        else
#endif /* #if defined(DSRT_FEATURE_GEOM) */
        {
            p_view->width = p_display->width;

            p_view->height = p_display->height;
        }
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
    struct dsrt_ctxt const * const p_ctxt,
    int * const p_mouse_x,
    int * const p_mouse_y)
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
                        c_event = 'n';

                        b_continue = 0;
                    }
                    else if (XK_BackSpace == ks)
                    {
                        c_event = 'p';

                        b_continue = 0;
                    }
#if defined(DSRT_FEATURE_ZOOM)
                    else if (XK_Return == ks)
                    {
                        c_event = '=';

                        b_continue = 0;
                    }
                    else if (XK_Prior == ks)
                    {
                        c_event = '<';

                        b_continue = 0;
                    }
                    else if (XK_Next == ks)
                    {
                        c_event = '>';

                        b_continue = 0;
                    }
                    else if (XK_Up == ks)
                    {
                        c_event = 'w';

                        b_continue = 0;
                    }
                    else if (XK_Left == ks)
                    {
                        c_event = 'a';

                        b_continue = 0;
                    }
                    else if (XK_Down == ks)
                    {
                        c_event = 's';

                        b_continue = 0;
                    }
                    else if (XK_Right == ks)
                    {
                        c_event = 'd';

                        b_continue = 0;
                    }
#endif /* #if defined(DSRT_FEATURE_ZOOM) */

                    *p_mouse_x = o_event.xkey.x;

                    *p_mouse_y = o_event.xkey.y;
                }
#if defined(DSRT_FEATURE_ZOOM)
                else if (ButtonPress == o_event.type)
                {
                    if (Button1 == o_event.xbutton.button)
                    {
                        c_event = '>';
                    }
                    else if (Button3 == o_event.xbutton.button)
                    {
                        c_event = '=';
                    }
                    else
                    {
                        c_event = '<';
                    }

                    *p_mouse_x = o_event.xbutton.x;

                    *p_mouse_y = o_event.xbutton.y;

                    b_continue = 0;
                }
#endif /* #if defined(DSRT_FEATURE_ZOOM) */
                else if (ConfigureNotify == o_event.type)
                {
                    if (o_event.xconfigure.window == p_opts->i_embed)
                    {
                        XWindowChanges wc;

                        memset(&wc, 0, sizeof(wc));

#if defined(DSRT_FEATURE_GEOM)
                        if (p_opts->b_geometry)
                        {
                            wc.x = (o_event.xconfigure.width - (int)(p_view->width)) / 2;

                            wc.y = (o_event.xconfigure.height - (int)(p_view->height)) / 2;

                            XConfigureWindow(p_display->dis, p_view->h, CWX|CWY, &wc);
                        }
                        else
#endif /* #if defined(DSRT_FEATURE_GEOM) */
                        {
                            wc.width = o_event.xconfigure.width;

                            wc.height = o_event.xconfigure.height;

                            XConfigureWindow(p_display->dis, p_view->h, CWWidth|CWHeight, &wc);
                        }
                    }
                    else if (o_event.xconfigure.window == p_view->h)
                    {
                        if (o_event.xconfigure.width != (int)(p_view->width))
                        {
                            p_view->width = (unsigned int)(o_event.xconfigure.width);

                            i_timeout = 10; /* 160 msec */
                        }

                        if (o_event.xconfigure.height != (int)(p_view->height))
                        {
                            p_view->height = (unsigned int)(o_event.xconfigure.height);

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
        /* Enable interactive mode when there a list of files */
        if (p_opts->n_files > 1)
        {
            char b_continue;

            b_continue = 1;

            while (b_continue)
            {
                /* Grab keys from standard input */
                char buf[80];
                if (buf == fgets(buf, sizeof(buf), stdin))
                {
                    if (buf[0] == 'n')
                    {
                        c_event = 'n';

                        b_continue = 0;
                    }
                    else if (buf[0] == 'p')
                    {
                        c_event = 'p';

                        b_continue = 0;
                    }
                    else if (buf[0] == 'q')
                    {
                        c_event = 'q';

                        b_continue = 0;
                    }
                }
                else
                {
                    c_event = 'q';

                    b_continue = 0;
                }
            }
        }
    }

    return c_event;

} /* dsrt_view_event() */

/* end-of-file: dsrt_view.c */
