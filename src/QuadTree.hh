#ifndef QUADTREE_HH
#define QUADTREE_HH

#include <utility>
#include <stddef.h>
#include <string.h>

#include "Core.hh"

class QuadTree
{
public:
  QuadTree(ft xmin, ft ymin, ft xmax, ft ymax) : root(NULL), xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax) {}
  ~QuadTree() { delete root; }
  struct Node {
    int size;
    ft mass, r;
    ft xm, ym;
    Node* ch[4];
    Node() { memset(ch, 0, sizeof ch); }
    ~Node() { for (int i = 0; i < 4; i++) delete ch[i]; }
  };
  void show();
  void insert(ft x, ft y, ft r, ft m) { insert(root, xmin, ymin, xmax, ymax, x, y, r, m); }
  void barnesHut(const Task& tk, Body* me) { barnesHut(root, tk, xmin, ymin, xmax, ymax, me); }
protected:
  Node *root;
  void insert(Node*&, ft xmin, ft ymin, ft xmax, ft ymax, ft x, ft y, ft r, ft m);
  void traverse(Node* rt, int d, Callable<std::pair<Node*, std::pair<int, bool> >, void>& f);
  ft xmin, ymin, xmax, ymax;
  void barnesHut(const Node* rt, const Task& tk, ft xmin, ft ymin, ft xmax, ft ymax, Body* me);
};

#endif /* end of include guard: QUADTREE_HH */
