#pragma once

#include "common.hpp"

namespace slim
{

struct point
{
    int x;
    int y;

    point(int x = 0, int y = 0) :
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

    area(int l = 0, int r = 0, int t = 0, int b = 0) :
        left(l),
        right(r),
        top(t),
        bottom(b)
    {
    }

    area(const RECT& rc) :
        left(rc.left),
        right(rc.right),
        top(rc.top),
        bottom(rc.bottom)
    {
    }

    int size() const
    {
        return abs(right - left) * abs(bottom - top);
    }

    point center() const
    {
        return point((left + right) / 2, (top + bottom) / 2);
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

    double closeness(point p) const
    {
        return closeness(p.x, p.y);
    }

    // [(r-l)-abs(d1-d2)]/(r-l)
    double closeness(int x, int y) const
    {
        if (distance(x, y) > 0)
        {
            return 0.0f;
        }

        double w = (double)(right - left);
        if (w <= 0)
        {
            return 0.0f;
        }
        double h = (double)(bottom - top);
        if (h <= 0)
        {
            return 0.0f;
        }

        double x_ctr = (w - abs(left + right - x - x)) / w;
        double y_ctr = (h - abs(top + bottom - y - y)) / h;

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

    string stringify() const
    {
        string str = "";
        str += '<';
        str += to_string(left);
        str += '.';
        str += to_string(right);
        str += '.';
        str += to_string(top);
        str += '.';
        str += to_string(bottom);
        str += '>';
        return str;
    }
};

}