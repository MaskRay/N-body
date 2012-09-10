#ifndef OCTTREE_HH
#define OCTTREE_HH

#include <utility>
#include <stddef.h>
#include <string.h>

#include "Core.hh"

class OctTree
{
public:
  OctTree(ft xmin, ft ymin, ft zmin, ft xmax, ft ymax, ft zmax) : root(NULL), xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax) {}
  ~OctTree() { delete root; }
  struct Node {
    int size;
    ft mass, r;
    ft xm, ym, zm;
    Node* ch[8];
    Node() { memset(ch, 0, sizeof ch); }
    ~Node() { for (int i = 0; i < 8; i++) delete ch[i]; }
  };
  void show();
  void insert(ft x, ft y, ft z, ft r, ft m) { insert(root, xmin, ymin, zmin, xmax, ymax, zmax, x, y, z, r, m); }
  void barnesHut(const Task& tk, Body* me) { barnesHut(root, tk, xmin, ymin, zmin, xmax, ymax, zmax, me); }
protected:
  Node *root;
  void insert(Node*&, ft xmin, ft ymin, ft zmin, ft xmax, ft ymax, ft zmax, ft x, ft y, ft z, ft r, ft m);
  void traverse(Node* rt, int d, Callable<std::pair<Node*, std::pair<int, bool> >, void>& f);
  ft xmin, ymin, zmin, xmax, ymax, zmax;
  void barnesHut(const Node* rt, const Task& tk, ft xmin, ft ymin, ft zmin, ft xmax, ft ymax, ft zmax, Body* me);
};

#endif /* end of include guard: OCTTREE_HH */
