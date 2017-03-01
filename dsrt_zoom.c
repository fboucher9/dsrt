/* See LICENSE for license details. */

/*

Module: dsrt_zoom.c

Description:

    Zoom a rectangle within jpeg image.

*/

/* OS headers */
#include "dsrt_os.h"

/* Configuration */
#include "dsrt_cfg.h"

#if defined(DSRT_FEATURE_ZOOM)

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_zoom.h"

/* Options */
#include "dsrt_opts.h"

/* View */
#include "dsrt_view.h"

static
void
dsrt_zoom_reset(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    p_zoom->i_left = 0;

    p_zoom->i_top = 0;

    p_zoom->u_width = 0;

    p_zoom->u_height = 0;

} /* dsrt_zoom_reset() */

void
dsrt_zoom_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    if (p_opts->b_zoom)
    {
        p_zoom->i_left = p_opts->i_zoom_left;

        p_zoom->i_top = p_opts->i_zoom_top;

        p_zoom->u_width = p_opts->i_zoom_width;

        p_zoom->u_height = p_opts->i_zoom_height;
    }
    else
    {
        dsrt_zoom_reset(p_ctxt);
    }

} /* dsrt_zoom_init() */

void
dsrt_zoom_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    dsrt_zoom_reset(p_ctxt);

} /* dsrt_zoom_cleanup() */

void
dsrt_zoom_setup(
    struct dsrt_ctxt const * const p_ctxt,
    unsigned int const u_width,
    unsigned int const u_height)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    int i_zoom_width;

    int i_zoom_height;

    int i_width;

    int i_height;

    if ((p_zoom->u_width > u_width) || !p_zoom->u_width)
    {
        p_zoom->i_left = 0;

        p_zoom->u_width = u_width;
    }

    if ((p_zoom->u_height > u_height) || !p_zoom->u_height)
    {
        p_zoom->i_top = 0;

        p_zoom->u_height = u_height;
    }

    i_width = (int)(u_width);

    i_height = (int)(u_height);

    i_zoom_width = (int)(p_zoom->u_width);

    i_zoom_height = (int)(p_zoom->u_height);

    if (p_zoom->i_left < -(i_zoom_width / 4))
    {
        p_zoom->i_left = -(i_zoom_width / 4);
    }

    if (p_zoom->i_top < -(i_zoom_height / 4))
    {
        p_zoom->i_top = -(i_zoom_height / 4);
    }

    if (p_zoom->i_left + i_zoom_width > i_width + (i_zoom_width / 4))
    {
        p_zoom->i_left = (i_width + i_zoom_width/4) - i_zoom_width;
    }

    if (p_zoom->i_top + i_zoom_height > i_height + (i_zoom_height / 4))
    {
        p_zoom->i_top = (i_height + i_zoom_height/4) - i_zoom_height;
    }

} /* dsrt_zoom_setup() */

void
dsrt_zoom_event(
    struct dsrt_ctxt const * const p_ctxt,
    char const c_event,
    int const i_mouse_x,
    int const i_mouse_y)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    int x;

    int y;

    int w;

    int h;

    int dx;

    int dy;

    (void)(p_opts);

    w = (int)(p_zoom->u_width);

    h = (int)(p_zoom->u_height);

#if defined(DSRT_FEATURE_MIRROR)
    if (p_opts->b_mirror_x)
    {
        dx = -1;

        x = p_zoom->i_left + ((((int)(p_ctxt->p_view->width) - 1 - i_mouse_x) * w) / (int)(p_ctxt->p_view->width));
    }
    else
#endif /* #if defined(DSRT_FEATURE_MIRROR) */
    {
        dx = 1;

        x = p_zoom->i_left + ((i_mouse_x * w) / (int)(p_ctxt->p_view->width));
    }

#if defined(DSRT_FEATURE_MIRROR)
    if (p_opts->b_mirror_y)
    {
        dy = -1;

        y = p_zoom->i_top + ((((int)(p_ctxt->p_view->height) - 1 - i_mouse_y) * h) / (int)(p_ctxt->p_view->height) );
    }
    else
#endif /* #if defined(DSRT_FEATURE_MIRROR) */
    {
        dy = 1;

        y = p_zoom->i_top + ((i_mouse_y * h) / (int)(p_ctxt->p_view->height) );
    }

    if (('<' == c_event) ||
       ('=' == c_event) ||
       ('>' == c_event))
    {
        if ('<' == c_event)
        {
            w *= 2;

            h *= 2;
        }
        else if ('>' == c_event)
        {
            w /= 2;

            h /= 2;
        }

        if (w < 1)
        {
            w = 1;
        }

        if (h < 1)
        {
            h = 1;
        }

        x = x - w/2;

        y = y - h/2;

        p_zoom->i_left = x;

        p_zoom->u_width = (unsigned int)(w);

        p_zoom->i_top = y;

        p_zoom->u_height = (unsigned int)(h);
    }
    else if (('w' == c_event)
        || ('a' == c_event)
        || ('s' == c_event)
        || ('d' == c_event))
    {
        h /= 2;

        w /= 2;

        if (h < 1)
        {
            h = 1;
        }

        if (w < 1)
        {
            w = 1;
        }

        w *= dx;

        h *= dy;

        if ('w' == c_event)
        {
            p_zoom->i_top -= h;
        }
        else if ('s' == c_event)
        {
            p_zoom->i_top += h;
        }
        else if ('a' == c_event)
        {
            p_zoom->i_left -= w;
        }
        else if ('d' == c_event)
        {
            p_zoom->i_left += w;
        }
    }
    else if (('n' == c_event) || ('p' == c_event))
    {
        dsrt_zoom_reset(p_ctxt);
    }
} /* dsrt_zoom_event() */

#endif /* #if defined(DSRT_FEATURE_ZOOM) */

/* end-of-file: dsrt_zoom.c */
