#include <algorithm>
#include <cmath>
#include <stdio.h>
#include "QuadTree.hh"
#include "Config.hh"
using namespace std;

const ft eps = 1e-9, THETA = 0.5;

static int getQuadrant(ft x, ft y, ft& xmin, ft& ymin, ft& xmax, ft& ymax)
{
  int quadrant = 0;
  ft xmid = (xmin + xmax) / 2,
     ymid = (ymin + ymax) / 2;
  if (x < xmid) xmax = xmid;
  else xmin = xmid, quadrant += 2;
  if (y < ymid) ymax = ymid;
  else ymin = ymid, quadrant++;
  return quadrant;
}

void QuadTree::insert(QuadTree::Node*& rt, ft xmin, ft ymin, ft xmax, ft ymax, ft x, ft y, ft r, ft m)
{
  if (rt == NULL) {
    rt = new Node;
    rt->size = 1;
    rt->xm = x * m;
    rt->ym = y * m;
    rt->r = r;
    rt->mass = m;
  } else if (rt->size == 1) {
    Node* p = new Node;
    p->size = 2;
    p->xm = rt->xm + x * m;
    p->ym = rt->ym + y * m;
    p->r = 0;
    p->mass = m + rt->mass;

    ft xmin0 = xmin, ymin0 = ymin, xmax0 = xmax, ymax0 = ymax;
    int q = getQuadrant(rt->xm / rt->mass, rt->ym / rt->mass, xmin, ymin, xmax, ymax);
    insert(p->ch[q], xmin, ymin, xmax, ymax, rt->xm / rt->mass, rt->ym / rt->mass, rt->r, rt->mass);
    delete rt;
    rt = p;

    q = getQuadrant(x, y, xmin0, ymin0, xmax0, ymax0);
    insert(p->ch[q], xmin0, ymin0, xmax0, ymax0, x, y, r, m);
  } else {
    int q = getQuadrant(x, y, xmin, ymin, xmax, ymax);
    rt->size++;
    rt->xm += x * m;
    rt->ym += y * m;
    rt->mass += m;
    insert(rt->ch[q], xmin, ymin, xmax, ymax, x, y, r, m);
  }
}

void QuadTree::traverse(QuadTree::Node* rt, int d, Callable<std::pair<Node*, std::pair<int, bool> >, void>& f)
{
  if (rt == NULL) return;
  f(make_pair(rt, make_pair(d, true)));
  for (int i = 0; i < 4; i++)
    traverse(rt->ch[i], d + 1, f);
  f(make_pair(rt, make_pair(d, false)));
}

class CbShow : public Callable<pair<QuadTree::Node*, pair<int, bool> >, void>
{
public:
  virtual void operator()(const pair<QuadTree::Node*, pair<int, bool> >& r) {
    QuadTree::Node* rt = r.first;
    int d = r.second.first;
    bool preorder = r.second.second;
    if (preorder)
      printf("%*s(%lg,%lg) mass:%lg %d\n", d, "", rt->xm, rt->ym, rt->mass, rt->size);
  }
};

void QuadTree::show()
{
  CbShow r;
  traverse(root, 0, r);
}

void QuadTree::barnesHut(const Node* rt, const Task& tk, ft xmin, ft ymin, ft xmax, ft ymax, Body* me)
{
  if (rt == NULL) return;
  ft xmid = (xmin + xmax) / 2,
     ymid = (ymin + ymax) / 2,
     s = max(xmax - xmin, ymax - ymin),
     dx = rt->xm / rt->mass - me->pos.x,
     dy = rt->ym / rt->mass - me->pos.y,
     d = dx * dx + dy * dy;
  if (rt->size == 1 || s * s / d < THETA * THETA) {
    if (d > eps) {
      ft t = cfg.gravitational * tk.dt * rt->mass * pow(max(d, (me->r + rt->r) * (me->r + rt->r)), -1.5);
      me->v.x += dx * t;
      me->v.y += dy * t;
    }
  } else {
    barnesHut(rt->ch[0], tk, xmin, ymin, xmid, ymid, me);
    barnesHut(rt->ch[1], tk, xmin, ymid, xmid, ymax, me);
    barnesHut(rt->ch[2], tk, xmid, ymin, xmax, ymid, me);
    barnesHut(rt->ch[3], tk, xmid, ymid, xmax, ymax, me);
  }
}
