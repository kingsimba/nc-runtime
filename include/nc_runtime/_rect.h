#pragma once

struct Rect
{
    int left;
    int top;
    int right;
    int bottom;
    forceinline int width() const { return this->right - this->left; }
    forceinline int height() const { return this->bottom - this->top; }

    Rect() = default;

    Rect(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    void set(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    void expand(int radius)
    {
        this->left -= radius, this->top -= radius, this->right += radius, this->bottom += radius;
    }

    void intersectWith(Rect r)
    {
        this->left = nc_max(r.left, this->left);
        this->top = nc_max(r.top, this->top);
        this->right = nc_min(r.right, this->right);
        this->bottom = nc_min(r.bottom, this->bottom);
    }

    bool isIntersectWith(Rect r)
    {
        return !(this->right <= r.left || this->bottom <= r.top || this->left >= r.right || this->top >= r.bottom);
    }

    bool containsPoint(int x, int y)
    {
        return x >= this->left && x < this->right && y >= this->top && y < this->bottom;
    }

    void setAsNegativeMinimum()
    {
        this->left = this->top = INT_MAX;
        this->right = this->bottom = INT_MIN;
    }

    void combinePoint(int x, int y)
    {
        this->left = nc_min(x, this->left);
        this->top = nc_min(y, this->top);
        this->right = nc_max(x, this->right);
        this->bottom = nc_max(y, this->bottom);
    }

    void combineRect(Rect rct)
    {
        this->left = nc_min(rct.left, this->left);
        this->top = nc_min(rct.top, this->top);
        this->right = nc_max(rct.right, this->right);
        this->bottom = nc_max(rct.bottom, this->bottom);
    }
};

inline bool operator==(Rect l, Rect r)
{
    return l.left == r.left && l.top == r.top && l.right == r.right && l.bottom == r.bottom;
}

inline bool operator!=(Rect l, Rect r)
{
    return l.left != r.left || l.top != r.top || l.right != r.right || l.bottom != r.bottom;
}
