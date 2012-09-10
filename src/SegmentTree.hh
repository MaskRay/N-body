#ifndef SEGMENTTREE_HH
#define SEGMENTTREE_HH

#include <set>
#include <utility>
#include <vector>
#include "Collide.hh"
using std::vector;
using std::set;

typedef vector<int> vi;

class SegmentTree
{
public:
  SegmentTree(const Cube*, int);
  ~SegmentTree() { delete root; delete[] scales; }
  struct Node {
    Node* ch[2];
    set<int> os;
    Node() {};
    ~Node() { delete ch[0]; delete ch[1]; }
  };
  void insert(int id) { insert(root, 0, nscales - 1, getScale(cubes[id].lo[0]), getScale(cubes[id].hi[0]), id); }
  void remove(int id) { remove(root, 0, nscales - 1, getScale(cubes[id].lo[0]), getScale(cubes[id].hi[0]), id); }
  bool overlap(int id, const CollisionHandler& maybeCollide) {
    return overlap(root, 0, nscales - 1, getScale(cubes[id].lo[0]), getScale(cubes[id].hi[0]), id, maybeCollide);
  }
protected:
  Node* build(int, int);
  bool overlap(Node*, int, int, int, int, int, const CollisionHandler&);
  void insert(Node*, int l, int r, int ll, int rr, int id);
  void remove(Node*, int l, int r, int ll, int rr, int id);
  void destruct(Node*);
  int getScale(ft x);
  const Cube* cubes;
  ft* scales;
  int nscales;
  Node* root;
};

void nBodyCollideSweepLineSegmentTree(const Task& tk);

#endif /* end of include guard: SEGMENTTREE_HH */
