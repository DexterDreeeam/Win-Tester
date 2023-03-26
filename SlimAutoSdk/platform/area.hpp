#pragma once

#include "common.hpp"

namespace slim
{

struct area
{
    int left;
    int right;

    int top;
    int bottom;

    area(const RECT& rc) :
        left(rc.left),
        right(rc.right),
        top(rc.top),
        bottom(rc.bottom)
    {
    }

    bool Inside(int x, int y) const
    {
        return x >= left && x <= right && y >= top && y <= bottom;
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

    // [(r-l)-abs(d1-d2)]/(r-l)
    float center(int x, int y) const
    {
        if (distance(x, y) > 0)
        {
            return 0.0f;
        }

        float w = right - left;
        if (w <= 0)
        {
            return 0.0f;
        }
        float h = bottom - top;
        if (h <= 0)
        {
            return 0.0f;
        }

        float x_ctr = (w - abs(left + right - x - x)) / w;
        float y_ctr = (h - abs(top + bottom - y - y)) / h;

        return x_ctr * y_ctr;
    }

    RECT ToRect() const
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