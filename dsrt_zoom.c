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

/* Context */
#include "dsrt_ctxt.h"

/* Module */
#include "dsrt_zoom.h"

/* Options */
#include "dsrt_opts.h"

char
dsrt_zoom_init(
    struct dsrt_ctxt const * const p_ctxt)
{
    char b_result;

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
        p_zoom->x1 = 0;

        p_zoom->y1 = 0;

        p_zoom->x2 = -1;

        p_zoom->y2 = -1;
    }

    b_result = 1;

    return b_result;

} /* dsrt_zoom_init() */

void
dsrt_zoom_cleanup(
    struct dsrt_ctxt const * const p_ctxt)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    p_zoom->x1 = 0;

    p_zoom->y1 = 0;

    p_zoom->x2 = -1;

    p_zoom->y2 = -1;

} /* dsrt_zoom_cleanup() */

void
dsrt_zoom_setup(
    struct dsrt_ctxt const * const p_ctxt,
    signed long int const width,
    signed long int const height)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

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

    if (p_zoom->x1 < 0)
    {
        p_zoom->x2 = p_zoom->x2 - p_zoom->x1;

        p_zoom->x1 = 0;
    }

    if (p_zoom->y1 < 0)
    {
        p_zoom->y2 = p_zoom->y2 - p_zoom->y1;

        p_zoom->y1 = 0;
    }

    if (p_zoom->x2 > width)
    {
        p_zoom->x1 = p_zoom->x1 + width - p_zoom->x2;

        p_zoom->x2 = width;
    }

    if (p_zoom->y2 > height)
    {
        p_zoom->y1 = p_zoom->y1 + height - p_zoom->y2;

        p_zoom->y2 = height;
    }

} /* dsrt_zoom_setup() */

void
dsrt_zoom_event(
    struct dsrt_ctxt const * const p_ctxt,
    char const c_event)
{
    struct dsrt_zoom * const p_zoom = p_ctxt->p_zoom;

    if ('0' == c_event)
    {
        int x;

        int y;

        int w;

        int h;

        x = ((p_zoom->x1 + p_zoom->x2 - 1) / 2);

        y = ((p_zoom->y1 + p_zoom->y2 - 1) / 2);

        w = (p_zoom->x2 - p_zoom->x1) * 2;

        h = (p_zoom->y2 - p_zoom->y1) * 2;

        x = x - w/2;

        y = y - h/2;

        p_zoom->x1 = x;

        p_zoom->x2 = x + w;

        p_zoom->y1 = y;

        p_zoom->y2 = y + h;
    }
    else if (('1' <= c_event) && ('9' >= c_event))
    {
        int x;

        int y;

        int w;

        int h;

        w = (p_zoom->x2 - p_zoom->x1) / 2;

        h = (p_zoom->y2 - p_zoom->y1) / 2;

        if (w < 1)
        {
            w = 1;
        }

        if (h < 1)
        {
            h = 1;
        }

        if (('1' == c_event)
            || ('4' == c_event)
            || ('7' == c_event))
        {
            x = p_zoom->x1 + ((p_zoom->x2 - p_zoom->x1) * 1) / 6;
        }
        else if (('2' == c_event)
            || ('5' == c_event)
            || ('8' == c_event))
        {
            x = p_zoom->x1 + ((p_zoom->x2 - p_zoom->x1) * 3) / 6;
        }
        else
        {
            x = p_zoom->x1 + ((p_zoom->x2 - p_zoom->x1) * 5) / 6;
        }

        if (('1' == c_event)
            || ('2' == c_event)
            || ('3' == c_event))
        {
            y = p_zoom->y1 + ((p_zoom->y2 - p_zoom->y1) * 1) / 6;
        }
        else if (('4' == c_event)
            || ('5' == c_event)
            || ('6' == c_event))
        {
            y = p_zoom->y1 + ((p_zoom->y2 - p_zoom->y1) * 3) / 6;
        }
        else
        {
            y = p_zoom->y1 + ((p_zoom->y2 - p_zoom->y1) * 5) / 6;
        }

        x = x - w/2;

        y = y - h/2;

        p_zoom->x1 = x;

        p_zoom->x2 = x + w;

        p_zoom->y1 = y;

        p_zoom->y2 = y + h;
    }
    else if ('w' == c_event)
    {
        int h;

        h = (p_zoom->y2 - p_zoom->y1) / 2;

        if (h < 1)
        {
            h = 1;
        }

        p_zoom->y1 -= h;

        p_zoom->y2 -= h;
    }
    else if ('s' == c_event)
    {
        int h;

        h = (p_zoom->y2 - p_zoom->y1) / 2;

        if (h < 1)
        {
            h = 1;
        }

        p_zoom->y1 += h;

        p_zoom->y2 += h;
    }
    else if ('a' == c_event)
    {
        int w;

        w = (p_zoom->x2 - p_zoom->x1) / 2;

        if (w < 1)
        {
            w = 1;
        }

        p_zoom->x1 -= w;

        p_zoom->x2 -= w;
    }
    else if ('d' == c_event)
    {
        int w;

        w = (p_zoom->x2 - p_zoom->x1) / 2;

        if (w < 1)
        {
            w = 1;
        }

        p_zoom->x1 += w;

        p_zoom->x2 += w;
    }
} /* dsrt_zoom_event() */

/* end-of-file: dsrt_zoom.c */
