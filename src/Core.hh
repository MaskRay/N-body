#ifndef CORE_HH
#define CORE_HH

#include "config.h"

#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define TRACE(fmt, ...) fprintf(stderr, "%s %d: "fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#if defined(USE_FLOAT)
typedef float ft;
#else
typedef double ft;
#endif

class XFormat;

template<typename T, typename R>
class Callable
{
public:
  Callable() {}
  virtual ~Callable() {}
  virtual R operator()(const T&) = 0;
};

struct Vector
{
  ft x, y, z;
  Vector() {}
  Vector(ft x, ft y, ft z) : x(x), y(y), z(z) {}
  ft dot(const Vector& r) const {
    return x * r.x + y * r.y + z * r.z;
  }
  ft dist(const Vector& r) const {
    return (*this - r).norm();
  }
  Vector unit() const {
    ft n = 1.0 / norm();
    return Vector(x * n, y * n, z * n);
  }
  Vector operator*(ft r) const { return Vector(x * r, y * r, z * r); }
  Vector operator+(const Vector& r) const { return Vector(x + r.x, y + r.y, z + r.z); }
  Vector operator-(const Vector& r) const { return Vector(x - r.x, y - r.y, z - r.z); }
  Vector operator+=(const Vector& r) {
    x += r.x;
    y += r.y;
    z += r.z;
    return *this;
  }
  ft norm() const { return sqrt(x * x + y * y + z * z); }
  ft norm2() const { return x * x + y * y + z * z; }
};

Vector operator*(ft l, const Vector& r);

struct Body
{
  ft r, mass;
  Vector pos, v;
};

struct Task
{
  ft dt;
  int n, bgn, end;
  Body* bodies;
};

struct Cube
{
  int id;
  ft lo[3], hi[3];
};

void nBodyMove(const Task&);
void nBodyAccelerate(const Task&);
void nBodyCollide(const Task&);
Body* makeBodies();

#endif /* end of include guard: CORE_HH */
