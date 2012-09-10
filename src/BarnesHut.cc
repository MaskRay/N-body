#include "OctTree.hh"
#include "QuadTree.hh"
#include "Config.hh"

static QuadTree* quad;
static OctTree* oct;

void barnesHutNew(const Task& tk)
{
  if (cfg.dimension == 2) {
    quad = new QuadTree(0, 0, cfg.width, cfg.height);
    for (Body* i = tk.bodies; i < tk.bodies + tk.n; i++)
      if (0 <= i->pos.x && i->pos.x <= cfg.width && 0 <= i->pos.y && i->pos.y <= cfg.height)
        quad->insert(i->pos.x, i->pos.y, i->r, i->mass);
  } else {
    oct = new OctTree(0, 0, 0, cfg.width, cfg.height, cfg.length);
    for (Body* i = tk.bodies; i < tk.bodies + tk.n; i++)
      if (0 <= i->pos.x && i->pos.x <= cfg.width && 0 <= i->pos.y && i->pos.y <= cfg.height && 0 <= i->pos.z && i->pos.z <= cfg.length)
        oct->insert(i->pos.x, i->pos.y, i->pos.z, i->r, i->mass);
  }
}

void barnesHutAccelerate(const Task& tk)
{
  if (cfg.dimension == 2)
#pragma omp parallel for schedule(static) num_threads(cfg.nthreads)
    for (int i = tk.bgn; i < tk.end; i++)
      quad->barnesHut(tk, &tk.bodies[i]);
  else
#pragma omp parallel for schedule(static) num_threads(cfg.nthreads)
    for (int i = tk.bgn; i < tk.end; i++)
      oct->barnesHut(tk, &tk.bodies[i]);
}

void barnesHutDestroy()
{
  if (cfg.dimension == 2)
    delete quad;
  else
    delete oct;
}
