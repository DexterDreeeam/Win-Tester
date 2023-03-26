#pragma once

#include "common.hpp"

namespace slim
{

struct point
{
    int x;
    int y;

    point(int x, int y) :
        x(x),
        y(y)
    {
    }

    point(POINT pt) :
        x(pt.x),
        y(pt.y)
    {
    }

    POINT ToPOINT() const
    {
        POINT pt;
        pt.x = x;
        pt.y = y;
        return pt;
    }
};

struct area
{
    int left;
    int right;

    int top;
    int bottom;

    area() :
        left(),
        right(),
        top(),
        bottom()
    {
    }

    area(const RECT& rc) :
        left(rc.left),
        right(rc.right),
        top(rc.top),
        bottom(rc.bottom)
    {
    }

    bool Inside(point p) const
    {
        return Inside(p.x, p.y);
    }

    bool Inside(int x, int y) const
    {
        return x >= left && x <= right && y >= top && y <= bottom;
    }

    int distance(point p) const
    {
        return distance(p.x, p.y);
    }

    int distance(int x, int y) const
    {
        int x_dis = 0;
        int y_dis = 0;

        if (x < left)
        {
            x_dis = left - x;
        }
        else if (x > right)
        {
            x_dis = x - right;
        }

        if (y < top)
        {
            y_dis = top - y;
        }
        else if (y > bottom)
        {
            y_dis = y - bottom;
        }

        return x_dis + y_dis;
    }

    float center(point p) const
    {
        return center(p.x, p.y);
    }

    // [(r-l)-abs(d1-d2)]/(r-l)
    float center(int x, int y) const
    {
        if (distance(x, y) > 0)
        {
            return 0.0f;
        }

        float w = (float)(right - left);
        if (w <= 0)
        {
            return 0.0f;
        }
        float h = (float)(bottom - top);
        if (h <= 0)
        {
            return 0.0f;
        }

        float x_ctr = (w - abs(left + right - x - x)) / w;
        float y_ctr = (h - abs(top + bottom - y - y)) / h;

        return x_ctr * y_ctr;
    }

    RECT ToRECT() const
    {
        RECT rc;
        rc.left = this->left;
        rc.top = this->top;
        rc.right = this->right;
        rc.bottom = this->bottom;
        return rc;
    }
};

}