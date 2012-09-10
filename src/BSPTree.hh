#ifndef BSPTREE_HH
#define BSPTREE_HH

#include <vector>
#include "Collide.hh"
using std::vector;

typedef vector<int> vi;

struct AAPlane
{
  int cid;
  ft coord;
  void set(int cid_, ft coord_) {
    cid = cid_;
    coord = coord_;
  }
};

class BSPTree
{
public:
  BSPTree(const Cube* coords, const vi& ids, Cube c) : coords(coords) { root = build(ids, c); }
  ~BSPTree() { delete root; }
  struct Node {
    Node* ch[2];
    Cube c;
    AAPlane plane;
    std::vector<int> os;
    Node() { ch[0] = ch[1] = NULL; }
    ~Node() { delete ch[0]; delete ch[1]; }
    bool isLeaf() { return ch[0] == NULL && ch[1] == NULL; }
  };
  bool overlap(int id, const CollisionHandler& maybeCollide) {
    return overlap(root, id, maybeCollide);
  }
protected:
  Node* build(const vi&, Cube);
  bool overlap(Node* rt, int, const CollisionHandler&);
  Node* root;
  const Cube* coords;
};

void nBodyCollideBSPTree(const Task&);

#endif /* end of include guard: BSPTREE_HH */
