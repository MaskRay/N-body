#ifndef COLLIDE_HH
#define COLLIDE_HH

#include "Core.hh"

void nBodyCollide(const Task&);

class CollisionHandler
{
public:
  CollisionHandler(const Task& tk, int me) : tk(tk), me(me) {}
  bool operator()(int) const;
protected:
  const Task& tk;
  int me;
};

#endif /* end of include guard: COLLIDE_HH */
