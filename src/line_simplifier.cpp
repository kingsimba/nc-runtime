#include "stdafx_nc_runtime.h"
#include "nc_runtime/line_simplifier.h"
#include "nc_runtime/nc_math.h"

struct LineSpan
{
    int begin;
    int end;
    float distance;
    int mid;

    friend bool operator<(const LineSpan& l, const LineSpan& r) { return l.distance < r.distance; }
};

static bool _makeSpan(nc::Vector2* points, int begin, int end, LineSpan* spanOut)
{
    if (end - begin <= 2)
        return false;

    float maxDis = 0.0f;
    int mid = begin;
    for (int i = begin + 1; i + 1 < end; i++)
    {
        float dis = Math_segmentPointDistance(points[i], points[begin], points[end - 1], nullptr);
        if (dis > maxDis)
        {
            maxDis = dis;
            mid = i;
        }
    }

    spanOut->begin = begin;
    spanOut->end = end;
    spanOut->distance = maxDis;
    spanOut->mid = mid;
    return true;
}

int LineSimplifier::douglasPeucker(nc::Vector2* points, int count, float tolerance, int maxResultCount)
{
    std::priority_queue<LineSpan> st;
    LineSpan span;

    if (count <= 2)
        return count;

    m_selection.clear();
    m_selection.resize(count, false);
    m_selection.front() = true;
    m_selection.back() = true;

    if (_makeSpan(points, 0, count, &span))
        st.push(span);

    int resultCount = 2;
    while (!st.empty() && resultCount < maxResultCount && st.top().distance > tolerance)
    {
        auto ip = st.top();
        st.pop();

        m_selection[ip.mid] = true;
        resultCount++;

        if (_makeSpan(points, ip.begin, ip.mid + 1, &span))
            st.push(span);
        if (_makeSpan(points, ip.mid, ip.end, &span))
            st.push(span);
    }

    int rtn = _compactBySelection(points, count);
    return rtn;
}

int LineSimplifier::_compactBySelection(nc::Vector2* points, int count)
{
    int p1 = 0;
    while (p1 != count && m_selection[p1])
    {
        p1++;
    }
    if (p1 == count)
        return p1;

    int p2 = p1 + 1;
    for (;;)
    {
        while (p2 != count && !m_selection[p2])
        {
            p2++;
        }
        if (p2 == count)
            break;

        points[p1] = points[p2];
        m_selection[p1] = m_selection[p2];
        m_selection[p2] = false;
        p1++;
        p2++;
    }

    return p1;
}
