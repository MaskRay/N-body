#include <algorithm>
#include <stdlib.h>
#include <float.h>
#include "BSPTree.hh"
#include "Config.hh"
using namespace std;

#define FORIT(t, i, a) for (t::const_iterator i = (a).begin(); i != (a).end(); ++i)
#define FORVIT(t, i, a) FORIT(vector<t>, i, a)

const int NODE_SIZE_THREASHOLD = 9;
const ft EPS = 1e-8;

struct Event
{
  bool type;
  int cid, id;
  ft pos;
  bool operator<(const Event& r) const {
    return pos < r.pos;
  }
};

BSPTree::Node* BSPTree::build(const vi& ids, Cube c)
{
  if (ids.empty())
    return NULL;
  Node* rt = new Node;
  rt->c = c;
  if (ids.size() <= NODE_SIZE_THREASHOLD) {
    rt->os = ids;
    return rt;
  }

  ft pivotpos;
  int pivotcid = -1, pivotd = ids.size();
  vector<Event> es;
  for (int cid = 0; cid < cfg.dimension; cid++) {
    es.clear();
    FORVIT(int, i, ids) {
      Event e;
      e.type = true;
      e.id = *i;
      e.pos = coords[*i].lo[cid];
      es.push_back(e);

      e.type = false;
      e.pos = coords[*i].hi[cid];
      es.push_back(e);
    }
    sort(es.begin(), es.end());

    int nl = 0, nm = 0, nr = ids.size();
    for (vector<Event>::iterator j, i = es.begin(); i != es.end(); i = j) {
      for (j = i; j != es.end() && j->pos == i->pos; ++j)
        if (j->type)
          nm++, nr--;
        else
          nm--, nl++;
      int d = abs(nl - nr) + nm;
      if (d < pivotd) {
        pivotd = d;
        pivotcid = cid;
        pivotpos = i->pos;
      }
    }
  }

  if (double(pivotd) < max(12.0, ids.size() * 0.14) && pivotd != ids.size()) {
    rt->plane.set(pivotcid, pivotpos);
    vi ls, rs;
    FORVIT(int, i, ids)
      if (coords[*i].hi[pivotcid] <= pivotpos)
        ls.push_back(*i);
      else if (pivotpos <= coords[*i].lo[pivotcid])
        rs.push_back(*i);
      else
        ls.push_back(*i), rs.push_back(*i);

    ft t = c.hi[pivotcid];
    c.hi[pivotcid] = pivotpos;
    rt->ch[0] = build(ls, c);
    c.hi[pivotcid] = t;

    t = c.lo[pivotcid];
    c.lo[pivotcid] = pivotpos;
    rt->ch[1] = build(rs, c);
    c.lo[pivotcid] = t;
  } else
    rt->os = ids;
  return rt;
}

bool BSPTree::overlap(Node* rt, int id, const CollisionHandler& maybeCollide)
{
  if (rt == NULL)
    return false;
  const Cube& c = coords[id];
  if (rt->isLeaf()) {
    FORVIT(int, i, rt->os)
      if (*i != id) {
        int cid;
        for (cid = 0; cid < cfg.dimension; cid++)
          if (! (coords[*i].lo[cid] <= c.hi[cid] && c.lo[cid] <= coords[*i].hi[cid]))
            break;
        if (cid == cfg.dimension && maybeCollide(*i))
          return true;
      }
    return false;
  }
  if (c.lo[rt->plane.cid] < rt->plane.coord && overlap(rt->ch[0], id, maybeCollide))
    return true;
  if (rt->plane.coord < c.hi[rt->plane.cid] && overlap(rt->ch[1], id, maybeCollide))
    return true;
  return false;
}

void extend(Cube& a, const Cube& b)
{
  for (int i = 0; i < cfg.dimension; i++) {
    if (b.lo[i] < a.lo[i])
      a.lo[i] = b.lo[i];
    if (b.hi[i] > a.hi[i])
      a.hi[i] = b.hi[i];
  }
}

void nBodyCollideBSPTree(const Task& tk)
{
  vi ids;
  for (int i = 0; i < tk.n; i++)
    ids.push_back(i);
  Cube space, *coords = new Cube[tk.n];
  space.lo[0] = DBL_MAX; space.hi[0] = DBL_MIN;
  space.lo[1] = DBL_MAX; space.hi[1] = DBL_MIN;
  space.lo[2] = DBL_MAX; space.hi[2] = DBL_MIN;
  for (Body* j = tk.bodies; j < tk.bodies + tk.n; j++) {
    int i = j - tk.bodies;
    ft x = j->pos.x, xx = x + j->v.x * tk.dt,
       y = j->pos.y, yy = j->pos.y + j->v.y * tk.dt,
       z = j->pos.z, zz = j->pos.z + j->v.z * tk.dt;

    coords[i].id = i;
    coords[i].lo[0] = min(x, xx) - j->r;
    coords[i].lo[1] = min(y, yy) - j->r;
    coords[i].lo[2] = min(z, zz) - j->r;
    coords[i].hi[0] = max(x, xx) + j->r;
    coords[i].hi[1] = max(y, yy) + j->r;
    coords[i].hi[2] = max(z, zz) + j->r;

    extend(space, coords[i]);
  }

  BSPTree bsp(coords, ids, space);
  for (int i = 0; i < tk.n; i++) {
    CollisionHandler maybeCollide(tk, i);
    bsp.overlap(i, maybeCollide);
  }
  delete[] coords;
}
