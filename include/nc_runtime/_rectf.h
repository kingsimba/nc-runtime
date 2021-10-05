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
};

inline bool operator==(RectF l, RectF r)
{
    return l.left == r.left && l.top == r.top && l.right == r.right && l.bottom == r.bottom;
}

inline bool operator!=(RectF l, RectF r)
{
    return l.left != r.left || l.top != r.top || l.right != r.right || l.bottom != r.bottom;
}
