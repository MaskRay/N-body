#include <algorithm>
#include <limits.h>
#include "IntervalTree.hh"
using namespace std;

#define FORIT(t, i, a) for (t::const_iterator i = (a).begin(); i != (a).end(); ++i)
#define FORVIT(t, i, a) FORIT(vector<t>, i, a)

struct SweepLineEvent : Cube
{
  bool operator<(const SweepLineEvent& r) const {
    return lo[1] < r.lo[1] || lo[1] == r.lo[1] && id < r.id;
  }
};

IntervalTree::Node* IntervalTree::build(const vi& ids)
{
  if (ids.empty())
    return NULL;
  vector<ft> xs;
  vi ls, rs;
  FORVIT(int, i, ids) {
    xs.push_back(coords[*i].lo[0]);
    xs.push_back(coords[*i].hi[0]);
  }
  nth_element(xs.begin(), xs.begin() + xs.size() / 2, xs.end());
  ft pivot = xs[xs.size() / 2];

  IntervalTree::Node* rt = new Node(pivot);
  FORVIT(int, i, ids)
    if (coords[*i].hi[0] < pivot)
      ls.push_back(*i);
    else if (pivot < coords[*i].lo[0])
      rs.push_back(*i);

  rt->ch[0] = build(ls);
  rt->ch[1] = build(rs);
  return rt;
}

void IntervalTree::insert(IntervalTree::Node* rt, int id)
{
  if (coords[id].hi[0] < rt->key)
    insert(rt->ch[0], id);
  else if (rt->key < coords[id].lo[0])
    insert(rt->ch[1], id);
  else {
    rt->spanl.insert(make_pair(coords[id].lo[0], id));
    rt->spanr.insert(make_pair(coords[id].hi[0], id));
  }
}

void IntervalTree::remove(IntervalTree::Node* rt, int id)
{
  if (coords[id].hi[0] < rt->key)
    remove(rt->ch[0], id);
  else if (rt->key < coords[id].lo[0])
    remove(rt->ch[1], id);
  else {
    rt->spanl.erase(make_pair(coords[id].lo[0], id));
    rt->spanr.erase(make_pair(coords[id].hi[0], id));
  }
}

bool IntervalTree::overlap(IntervalTree::Node* rt, int id, const CollisionHandler& maybeCollide)
{
  if (rt == NULL) return false;
  const Cube& c = coords[id];
  if (c.hi[0] < rt->key) {
    set<pair<ft, int> >::iterator it = rt->spanl.upper_bound(make_pair(c.hi[0], INT_MAX));
    for (set<pair<ft, int> >::iterator i = rt->spanl.begin(); i != it; ++i)
      if (maybeCollide(i->second))
        return true;
    return overlap(rt->ch[0], id, maybeCollide);
  } else if (rt->key < c.lo[0]) {
    set<pair<ft, int> >::iterator it = rt->spanr.lower_bound(make_pair(c.lo[0], 0));
    for (set<pair<ft, int> >::iterator i = it; i != rt->spanr.end(); ++i)
      if (maybeCollide(i->second))
        return true;
    return overlap(rt->ch[1], id, maybeCollide);
  } else {
    for (set<pair<ft, int> >::iterator i = rt->spanl.begin(); i != rt->spanl.end(); ++i)
      if (maybeCollide(i->second))
        return true;
    return overlap(rt->ch[0], id, maybeCollide) ||
      overlap(rt->ch[1], id, maybeCollide);
  }
}

static ft getX(const Vector& a)
{ return a.x; }

static ft getY(const Vector& a)
{ return a.y; }

void nBodyCollideSweepLineIntervalTree(const Task& tk)
{
  vector<SweepLineEvent> es(tk.n * 2);
  vi ids;
  Cube* coords = new Cube[tk.n];
  for (int i = 0; i < tk.n; i++)
    ids.push_back(i);
  ft (*getX)(const Vector&) = ::getX,
     (*getY)(const Vector&) = ::getY;
  for (int off = 0; off < 1; off++) {
    for (Body* j = tk.bodies; j < tk.bodies + tk.n; j++) {
      int i = j - tk.bodies;
      ft xx = getX(j->pos) + getX(j->v) * tk.dt,
         yy = getY(j->pos) + getY(j->v) * tk.dt;

      coords[i].id = i;
      coords[i].lo[0] = min(getX(j->pos), xx) - j->r;
      coords[i].lo[1] = min(getY(j->pos), yy) - j->r;
      coords[i].hi[0] = max(getX(j->pos), xx) + j->r;
      coords[i].hi[1] = max(getY(j->pos), yy) + j->r;

      es[i].id = i;
      es[i].lo[0] = coords[i].lo[0];
      es[i].hi[0] = coords[i].hi[0];
      es[i].lo[1] = coords[i].lo[1];

      es[tk.n + i].id = tk.n + i;
      es[tk.n + i].lo[0] = coords[i].lo[0];
      es[tk.n + i].hi[0] = coords[i].hi[0];
      es[tk.n + i].lo[1] = coords[i].hi[1];
    }

    IntervalTree interval(coords, ids);
    sort(es.begin(), es.end());
    for (vector<SweepLineEvent>::iterator i = es.begin(); i != es.end(); ++i) {
      if (i->id < tk.n) {
        CollisionHandler maybeCollide(tk, i->id);
        interval.overlap(i->id, maybeCollide);
        interval.insert(i->id);
      } else
        interval.remove(i->id - tk.n);
    }

    swap(getX, getY);
  }
  delete[] coords;
}
