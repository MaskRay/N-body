#include <algorithm>
#include <limits.h>
#include "SegmentTree.hh"
using namespace std;

#define FORIT(t, i, a) for (t::const_iterator i = (a).begin(); i != (a).end(); ++i)
#define FORVIT(t, i, a) FORIT(vector<t>, i, a)

struct SweepLineEvent : Cube
{
  bool operator<(const SweepLineEvent& r) const {
    return lo[1] < r.lo[1] || lo[1] == r.lo[1] && id < r.id;
  }
};

int SegmentTree::getScale(ft x)
{
  return lower_bound(scales, scales + nscales, x) - scales;
}

SegmentTree::SegmentTree(const Cube* cubes, int n)
  : cubes(cubes)
{
  scales = new ft[n * 2];
  for (int i = 0; i < n; i++) {
    scales[i] = cubes[i].lo[0];
    scales[n + i] = cubes[i].hi[0];
  }
  sort(scales, scales + n * 2);
  nscales = unique(scales, scales + n * 2) - scales;

  root = build(0, nscales - 1);
  for (int i = 0; i < n; i++)
    insert(i);
}

SegmentTree::Node* SegmentTree::build(int l, int r)
{
  Node* rt = new Node;
  if (l == r-1)
    rt->ch[0] = rt->ch[1] = NULL;
  else {
    int m = (l + r) / 2;
    rt->ch[0] = build(l, m);
    rt->ch[1] = build(m, r);
  }
  return rt;
}

void SegmentTree::insert(SegmentTree::Node* rt, int l, int r, int ll, int rr, int id)
{
  if (ll <= l && r <= rr)
    rt->os.insert(id);
  else {
    int m = (l + r) / 2;
    if (ll < m) insert(rt->ch[0], l, m, ll, rr, id);
    if (m < rr) insert(rt->ch[1], m, r, ll, rr, id);
  }
}

void SegmentTree::remove(SegmentTree::Node* rt, int l, int r, int ll, int rr, int id)
{
  if (ll <= l && r <= rr)
    rt->os.erase(id);
  else {
    int m = (l + r) / 2;
    if (ll < m) remove(rt->ch[0], l, m, ll, rr, id);
    if (m < rr) remove(rt->ch[1], m, r, ll, rr, id);
  }
}

bool SegmentTree::overlap(SegmentTree::Node* rt, int l, int r, int ll, int rr, int id, const CollisionHandler& maybeCollide)
{
  FORIT(set<int>, i, rt->os)
    if (maybeCollide(*i))
      return true;
  if (l < r - 1) {
    int m = (l + r) / 2;
    return ll < m && overlap(rt->ch[0], l, m, ll, rr, id, maybeCollide) ||
      m < rr && overlap(rt->ch[1], m, r, ll, rr, id, maybeCollide);
  }
  return false;
}

void nBodyCollideSweepLineSegmentTree(const Task& tk)
{
  vector<SweepLineEvent> es(tk.n * 2);
  Cube* cubes = new Cube[tk.n];
  for (Body* j = tk.bodies; j < tk.bodies + tk.n; j++) {
    int i = j - tk.bodies;
    ft xx = j->pos.x + j->v.x * tk.dt,
       yy = j->pos.y + j->v.y * tk.dt;

    cubes[i].id = i;
    cubes[i].lo[0] = min(j->pos.x, xx) - j->r;
    cubes[i].lo[1] = min(j->pos.y, yy) - j->r;
    cubes[i].hi[0] = max(j->pos.x, xx) + j->r;
    cubes[i].hi[1] = max(j->pos.y, yy) + j->r;

    es[i].id = i;
    es[i].lo[0] = cubes[i].lo[0];
    es[i].hi[0] = cubes[i].hi[0];
    es[i].lo[1] = cubes[i].lo[1];

    es[tk.n + i].id = tk.n + i;
    es[tk.n + i].lo[0] = cubes[i].lo[0];
    es[tk.n + i].hi[0] = cubes[i].hi[0];
    es[tk.n + i].lo[1] = cubes[i].hi[1];
  }

  SegmentTree segment(cubes, tk.n);
  sort(es.begin(), es.end());
  for (vector<SweepLineEvent>::iterator i = es.begin(); i != es.end(); ++i) {
    if (i->id < tk.n) {
      CollisionHandler maybeCollide(tk, i->id);
      segment.overlap(i->id, maybeCollide);
      segment.insert(i->id);
    } else
      segment.remove(i->id - tk.n);
  }
  delete[] cubes;
}
