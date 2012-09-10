#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include "Collide.hh"
#include "BarnesHut.hh"
#include "Config.hh"
using namespace std;

const ft eps = 1e-9;

Vector operator*(ft l, const Vector& r)
{
  return Vector(r.x * l, r.y * l, r.z * l);
}

ft getRand(ft upper)
{
  return rand() * upper / (RAND_MAX + 1.0);
}

Body* makeBodies()
{
  Body* bodies = new Body[cfg.n];
  srand(815);
  for (int i = 0; i < cfg.n; i++) {
    bodies[i].pos.x = getRand(cfg.width);
    bodies[i].pos.y = getRand(cfg.height);
    bodies[i].pos.z = cfg.dimension == 2 ? cfg.length / 2 : getRand(cfg.length);
    bodies[i].v.x = getRand(cfg.width) / 100;
    bodies[i].v.y = getRand(cfg.height) / 100;
    bodies[i].v.z = getRand(cfg.length) / 100;
    bodies[i].r = getRand(60) + 20;
    bodies[i].mass = 8.15 * bodies[i].r * bodies[i].r * bodies[i].r;
  }
  return bodies;
}

void nBodyAccelerate(const Task& tk)
{
  if (cfg.barnesHut)
    barnesHutAccelerate(tk);
  else
#pragma omp parallel for schedule(static) num_threads(cfg.nthreads)
    for (int ii = tk.bgn; ii < tk.end; ii++) {
      Body* i = tk.bodies + ii;
      for (Body* j = tk.bodies; j < tk.bodies + tk.n; j++)
        if (i != j) {
          ft dx = j->pos.x - i->pos.x,
             dy = j->pos.y - i->pos.y;
          ft d = max(dx * dx + dy * dy, (i->r + j->r) * (i->r + j->r));
          ft t = cfg.gravitational * tk.dt * j->mass * pow(d, -1.5);
          i->v.x += dx * t;
          i->v.y += dy * t;
        }
    }
}

void nBodyMove(const Task& tk)
{
#pragma omp parallel for schedule(static) num_threads(cfg.nthreads)
  for (int ii = 0; ii < tk.n; ii++) {
    Body* i = tk.bodies + ii;
    ft tt = tk.dt;
    while (tt > eps) {
      Vector ww = i->pos + i->v * tt;
      ft t;
      int flag = 0;
      if (ww.x <= i->r) {
        t = (i->pos.x - i->r) / - i->v.x;
        flag |= 4;
      } else if (ww.x >= cfg.width - i->r) {
        t = (cfg.width - i->r - i->pos.x) / i->v.x;
        flag |= 4;
      } else if (ww.y <= i->r) {
        t = (i->pos.y - i->r) / - i->v.y;
        flag |= 2;
      } else if (ww.y >= cfg.height - i->r) {
        t = (cfg.height - i->r - i->pos.y) / i->v.y;
        flag |= 2;
      } else if (ww.z <= i->r) {
        t = (i->pos.z - i->r) / - i->v.z;
        flag |= 1;
      } else if (ww.z >= cfg.length - i->r) {
        t = (cfg.length - i->r - i->pos.z) / i->v.z;
        flag |= 1;
      } else {
        i->pos = ww;
        break;
      }
      if (isnormal(t)) {
        i->pos += i->v * t;
        tt -= t;
      } else {
        i->pos = ww;
        break;
      }
      switch (flag) {
      case 4: i->v.x *= -1; break;
      case 2: i->v.y *= -1; break;
      case 1: i->v.z *= -1; break;
      }
    }
  }
}
