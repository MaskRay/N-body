#ifndef XFORMAT_HH
#define XFORMAT_HH

#include <X11/Xlib.h>
#include "Format.hh"
#include "Core.hh"

class XFormat : public Format
{
public:
  static const int MAX_ITER = 1 << 14;
  XFormat(const Task& tk);
  virtual ~XFormat() { XCloseDisplay(dpy); }
  virtual void render();
protected:
  Display* dpy;
  Window win;
  GC gc;
  Atom wmDeleteMessage;
};

#endif /* end of include guard: XFORMAT_HH */
