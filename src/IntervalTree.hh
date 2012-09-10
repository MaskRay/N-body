#ifndef INTERVALTREE_HH
#define INTERVALTREE_HH

#include <set>
#include <utility>
#include <vector>
#include "Collide.hh"
using std::vector;
using std::set;
using std::pair;

typedef vector<int> vi;

class IntervalTree
{
public:
  IntervalTree(const Cube* coords, const vi& ids) : coords(coords)
  { root = build(ids); }
  ~IntervalTree() { delete root; }
  struct Node {
    Node* ch[2];
    ft key;
    set<pair<ft, int> > spanl;
    set<pair<ft, int> > spanr;
    Node(ft key) : key(key) {};
    ~Node() { delete ch[0]; delete ch[1]; }
  };
  bool overlap(int id, const CollisionHandler& maybeCollide) {
    return overlap(root, id, maybeCollide);
  }
  void insert(int id) { insert(root, id); }
  void remove(int id) { remove(root, id); }
protected:
  Node* build(const vi&);
  bool overlap(Node*, int, const CollisionHandler&);
  void insert(Node*, int);
  void remove(Node*, int);
  const Cube* coords;
  Node* root;
};

void nBodyCollideSweepLineIntervalTree(const Task& tk);

#endif /* end of include guard: INTERVALTREE_HH */
