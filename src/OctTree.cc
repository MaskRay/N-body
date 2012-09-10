#include <algorithm>
#include <cmath>
#include <stdio.h>
#include "OctTree.hh"
#include "Config.hh"
using namespace std;

const ft eps = 1e-9, THETA = 0.5;

static int getOctant(ft x, ft y, ft z, ft& xmin, ft& ymin, ft& zmin, ft& xmax, ft& ymax, ft& zmax)
{
  int octant = 0;
  ft xmid = (xmin + xmax) / 2,
     ymid = (ymin + ymax) / 2,
     zmid = (zmin + zmax) / 2;
  if (x < xmid) xmax = xmid;
  else xmin = xmid, octant += 4;
  if (y < ymid) ymax = ymid;
  else ymin = ymid, octant += 2;
  if (z < zmid) zmax = zmid;
  else zmin = zmid, octant += 1;
  return octant;
}

void OctTree::insert(OctTree::Node*& rt, ft xmin, ft ymin, ft zmin, ft xmax, ft ymax, ft zmax, ft x, ft y, ft z, ft r, ft m)
{
  if (rt == NULL) {
    rt = new Node;
    rt->size = 1;
    rt->xm = x * m;
    rt->ym = y * m;
    rt->zm = z * m;
    rt->r = r;
    rt->mass = m;
  } else if (rt->size == 1) {
    Node* p = new Node;
    p->size = 2;
    p->xm = rt->xm + x * m;
    p->ym = rt->ym + y * m;
    p->zm = rt->zm + z * m;
    p->r = 0;
    p->mass = m + rt->mass;

    ft xmin0 = xmin, ymin0 = ymin, zmin0 = zmin, xmax0 = xmax, ymax0 = ymax, zmax0 = zmax;
    int q = getOctant(rt->xm / rt->mass, rt->ym / rt->mass, rt->zm / rt->mass, xmin, ymin, zmin, xmax, ymax, zmax);
    insert(p->ch[q], xmin, ymin, zmin, xmax, ymax, zmax, rt->xm / rt->mass, rt->ym / rt->mass, rt->zm / rt->mass, rt->r, rt->mass);
    delete rt;
    rt = p;

    q = getOctant(x, y, z, xmin0, ymin0, zmin0, xmax0, ymax0, zmax0);
    insert(p->ch[q], xmin0, ymin0, zmin0, xmax0, ymax0, zmax0, x, y, z, r, m);
  } else {
    int q = getOctant(x, y, z, xmin, ymin, zmin, xmax, ymax, zmax);
    rt->size++;
    rt->xm += x * m;
    rt->ym += y * m;
    rt->zm += z * m;
    rt->mass += m;
    insert(rt->ch[q], xmin, ymin, zmin, xmax, ymax, zmax, x, y, z, r, m);
  }
}

void OctTree::traverse(OctTree::Node* rt, int d, Callable<std::pair<Node*, std::pair<int, bool> >, void>& f)
{
  if (rt == NULL) return;
  f(make_pair(rt, make_pair(d, true)));
  for (int i = 0; i < 8; i++)
    traverse(rt->ch[i], d + 1, f);
  f(make_pair(rt, make_pair(d, false)));
}

class CbShow : public Callable<pair<OctTree::Node*, pair<int, bool> >, void>
{
public:
  virtual void operator()(const pair<OctTree::Node*, pair<int, bool> >& r) {
    OctTree::Node* rt = r.first;
    int d = r.second.first;
    bool preorder = r.second.second;
    if (preorder)
      printf("%*s(%lf,%lf) mass:%lf %lf\n", d, "", rt->xm, rt->ym, rt->zm, rt->mass, rt->size);
  }
};

void OctTree::show()
{
  CbShow r;
  traverse(root, 0, r);
}

void OctTree::barnesHut(const Node* rt, const Task& tk, ft xmin, ft ymin, ft zmin, ft xmax, ft ymax, ft zmax, Body* me)
{
  if (rt == NULL) return;
  ft xmid = (xmin + xmax) / 2,
     ymid = (ymin + ymax) / 2,
     zmid = (zmin + zmax) / 2,
     cx = rt->xm / rt->mass,
     cy = rt->ym / rt->mass,
     cz = rt->zm / rt->mass,
     s = max(max(xmax - xmin, ymax - ymin), zmax - zmin),
     dx = rt->xm / rt->mass - me->pos.x,
     dy = rt->ym / rt->mass - me->pos.y,
     dz = rt->zm / rt->mass - me->pos.z,
     d = dx * dx + dy * dy + dz * dz;
  if (rt->size == 1 || s * s / d < THETA * THETA) {
    if (d > eps) {
      ft t = cfg.gravitational * tk.dt * rt->mass * pow(max(d, (me->r + rt->r) * (me->r + rt->r)), -1.5);
      me->v.x += dx * t;
      me->v.y += dy * t;
      me->v.z += dz * t;
    }
  } else {
    barnesHut(rt->ch[0], tk, xmin, ymin, zmin, xmid, ymid, zmid, me);
    barnesHut(rt->ch[1], tk, xmin, ymin, zmid, xmid, ymid, zmax, me);
    barnesHut(rt->ch[2], tk, xmin, ymid, zmin, xmid, ymax, zmid, me);
    barnesHut(rt->ch[3], tk, xmin, ymid, zmid, xmid, ymax, zmax, me);
    barnesHut(rt->ch[4], tk, xmid, ymin, zmin, xmax, ymid, zmid, me);
    barnesHut(rt->ch[5], tk, xmid, ymin, zmid, xmax, ymid, zmax, me);
    barnesHut(rt->ch[6], tk, xmid, ymid, zmin, xmax, ymax, zmid, me);
    barnesHut(rt->ch[7], tk, xmid, ymid, zmid, xmax, ymax, zmax, me);
  }
}
