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

    p_zoom->x1 = 0;

    p_zoom->y1 = 0;

    p_zoom->x2 = -1;

    p_zoom->y2 = -1;

} /* dsrt_zoom_reset() */

void
dsrt_zoom_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    struct dsrt_opts const * const p_opts = p_ctxt->p_opts;

    if (p_opts->b_zoom)
    {
        p_zoom->x1 = p_opts->i_zoom_x1;

        p_zoom->y1 = p_opts->i_zoom_y1;

        p_zoom->x2 = p_opts->i_zoom_x2;

        p_zoom->y2 = p_opts->i_zoom_y2;
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
    signed long int const width,
    signed long int const height)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    int w;

    int h;

    if (((p_zoom->x2 - p_zoom->x1) > width) || ((p_zoom->x2 - p_zoom->x1) < 1))
    {
        p_zoom->x1 = 0;

        p_zoom->x2 = width;
    }

    if (((p_zoom->y2 - p_zoom->y1) > height) || ((p_zoom->y2 - p_zoom->y1) < 1))
    {
        p_zoom->y1 = 0;

        p_zoom->y2 = height;
    }

    w = p_zoom->x2 - p_zoom->x1;

    h = p_zoom->y2 - p_zoom->y1;

    if (p_zoom->x1 < -(w / 4))
    {
        p_zoom->x2 = p_zoom->x2 - (p_zoom->x1 + (w / 4));

        p_zoom->x1 = -(w / 4);
    }

    if (p_zoom->y1 < -(h / 4))
    {
        p_zoom->y2 = p_zoom->y2 - (p_zoom->y1 + (h / 4));

        p_zoom->y1 = -(h / 4);
    }

    if (p_zoom->x2 > width + (w / 4))
    {
        p_zoom->x1 = p_zoom->x1 + (width + w/4) - p_zoom->x2;

        p_zoom->x2 = width + (w / 4);
    }

    if (p_zoom->y2 > height + (h / 4))
    {
        p_zoom->y1 = p_zoom->y1 + (height + h/4) - p_zoom->y2;

        p_zoom->y2 = height + (h / 4);
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

    w = (p_zoom->x2 - p_zoom->x1);

    h = (p_zoom->y2 - p_zoom->y1);

#if defined(DSRT_FEATURE_MIRROR)
    if (p_opts->b_mirror_x)
    {
        dx = -1;

        x = p_zoom->x1 + (((p_ctxt->p_view->width - 1 - i_mouse_x) * (p_zoom->x2 - p_zoom->x1)) / p_ctxt->p_view->width);
    }
    else
#endif /* #if defined(DSRT_FEATURE_MIRROR) */
    {
        dx = 1;

        x = p_zoom->x1 + ((i_mouse_x * (p_zoom->x2 - p_zoom->x1)) / p_ctxt->p_view->width);
    }

#if defined(DSRT_FEATURE_MIRROR)
    if (p_opts->b_mirror_y)
    {
        dy = -1;

        y = p_zoom->y1 + (((p_ctxt->p_view->height - 1 - i_mouse_y) * (p_zoom->y2 - p_zoom->y1)) / p_ctxt->p_view->height );
    }
    else
#endif /* #if defined(DSRT_FEATURE_MIRROR) */
    {
        dy = 1;

        y = p_zoom->y1 + ((i_mouse_y * (p_zoom->y2 - p_zoom->y1)) / p_ctxt->p_view->height );
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

        p_zoom->x1 = x;

        p_zoom->x2 = x + w;

        p_zoom->y1 = y;

        p_zoom->y2 = y + h;
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
            p_zoom->y1 -= h;

            p_zoom->y2 -= h;
        }
        else if ('s' == c_event)
        {
            p_zoom->y1 += h;

            p_zoom->y2 += h;
        }
        else if ('a' == c_event)
        {
            p_zoom->x1 -= w;

            p_zoom->x2 -= w;
        }
        else if ('d' == c_event)
        {
            p_zoom->x1 += w;

            p_zoom->x2 += w;
        }
    }
    else if (('n' == c_event) || ('p' == c_event))
    {
        dsrt_zoom_reset(p_ctxt);
    }
} /* dsrt_zoom_event() */

#endif /* #if defined(DSRT_FEATURE_ZOOM) */

/* end-of-file: dsrt_zoom.c */
