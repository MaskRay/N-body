#include <algorithm>
#include "Collide.hh"
#include "IntervalTree.hh"
#include "BSPTree.hh"
#include "SegmentTree.hh"
#include "Config.hh"
using namespace std;

const ft eps = 1e-9;

static bool collide(Body& a, Body& b)
{
  Vector collision = a.pos - b.pos;
  ft dist2 = collision.norm2();
  if (dist2 > (a.r + b.r) * (a.r + b.r) + eps) return false;
  collision = collision.unit();

  ft avc = a.v.dot(collision), bvc = b.v.dot(collision);
  a.v += 2 * b.mass * (bvc - avc) / (a.mass + b.mass) * collision;
  b.v += 2 * a.mass * (avc - bvc) / (a.mass + b.mass) * collision;
  return true;
}

static ft predictive(Body& a, Body& b, ft dt)
{
  Vector pq = b.pos - a.pos,
         dpdq = dt * (b.v - a.v);
  ft A = dpdq.norm2(),
     B = 2 * pq.dot(dpdq),
     C = pq.norm2() - (a.r + b.r) * (a.r + b.r),
     delta = B * B - 4 * A * C;
  if (delta < 0) return 0;
  ft w = (-B - sqrt(delta)) / (2 * A);
  if (C < -eps || 0 <= w && w <= 1) {
    a.pos += w * dt * a.v;
    b.pos += w * dt * b.v;
    collide(a, b);
    return w;
  }
  return 0;
}

bool CollisionHandler::operator()(int opposite) const
{
  return me != opposite && predictive(tk.bodies[me], tk.bodies[opposite], tk.dt);
}

void nBodyCollide(const Task& tk)
{
  switch (cfg.collision_detection) {
  case 1:
    nBodyCollideSweepLineIntervalTree(tk);
    break;
  case 2:
    nBodyCollideBSPTree(tk);
    break;
  case 3:
    nBodyCollideSweepLineSegmentTree(tk);
    break;
  default:
    for (Body* i = tk.bodies; i < tk.bodies + tk.n; i++)
      for (Body* j = i; ++j < tk.bodies + tk.n; )
        predictive(*i, *j, tk.dt);
  }
}
