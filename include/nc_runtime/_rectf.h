#pragma once

struct RectF
{
    float left;
    float top;
    float right;
    float bottom;

    RectF() = default;

    RectF(float l, float t, float r, float b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    void set(float l, float t, float r, float b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    void expand(float radius)
    {
        this->left -= radius, this->top -= radius, this->right += radius, this->bottom += radius;
    }

    void intersectWith(RectF r)
    {
        this->left = nc_max(r.left, this->left);
        this->top = nc_max(r.top, this->top);
        this->right = nc_min(r.right, this->right);
        this->bottom = nc_min(r.bottom, this->bottom);
    }

    bool isIntersectWith(RectF r)
    {
        return !(this->right <= r.left || this->bottom <= r.top || this->left >= r.right || this->top >= r.bottom);
    }

    bool containsPoint(float x, float y)
    {
        return x >= this->left && x < this->right && y >= this->top && y < this->bottom;
    }

    void setAsNegativeMinimum()
    {
        this->left = this->top = FLT_MAX;
        this->right = this->bottom = -FLT_MAX;
    }

    void combinePoint(float x, float y)
    {
        this->left = nc_min(x, this->left);
        this->top = nc_min(y, this->top);
        this->right = nc_max(x, this->right);
        this->bottom = nc_max(y, this->bottom);
    }

    void combineRect(RectF rct)
    {
        this->left = nc_min(rct.left, this->left);
        this->top = nc_min(rct.top, this->top);
        this->right = nc_max(rct.right, this->right);
        this->bottom = nc_max(rct.bottom, this->bottom);
    }
};

inline bool operator==(RectF l, RectF r)
{
    return l.left == r.left && l.top == r.top && l.right == r.right && l.bottom == r.bottom;
}

inline bool operator!=(RectF l, RectF r)
{
    return l.left != r.left || l.top != r.top || l.right != r.right || l.bottom != r.bottom;
}
