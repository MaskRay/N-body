#include <stdlib.h>
#include <sys/select.h>
#include "XFormat.hh"
#include "ProcessScheduler.hh"
#include "Config.hh"

XFormat::XFormat(const Task& tk) : Format(tk)
{
  dpy = XOpenDisplay(NULL);
  unsigned long white = WhitePixel(dpy, DefaultScreen(dpy)),
                black = BlackPixel(dpy, DefaultScreen(dpy));
  win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, cfg.width, cfg.height, 0, black, black);
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "N-body Simulator");

  gc = XCreateGC(dpy, win, 0, NULL);
  XSetForeground(dpy, gc, white);

  XSelectInput(dpy, win, ExposureMask | StructureNotifyMask);
  wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);
}

void XFormat::render()
{
  timeval last_update, tv;

  bool first = true;
  int fd = ConnectionNumber(dpy);
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  for(;;) {
    XEvent e;
    tv.tv_sec = 0;
    tv.tv_usec = 1000000 / cfg.fps;
    if (select(fd + 1, &fds, NULL, NULL, &tv) == 0)
      goto EXPOSE;

    XNextEvent(dpy, &e);
    switch (e.type) {
    case Expose:
      if (!e.xexpose.count) {
EXPOSE:
        struct timeval now;
        gettimeofday(&now, NULL);
        if (first)
          first = false;
        else {
          ft seconds = now.tv_sec - last_update.tv_sec + 1e-6 * (now.tv_usec - last_update.tv_usec);
          fsm.advance(seconds);
        }
        last_update = now;

        XWindowAttributes attr;
        XGetWindowAttributes(dpy, win, &attr);
        Task& tk = fsm.tk;
        unsigned long black = BlackPixel(dpy, DefaultScreen(dpy));
        XClearWindow(dpy, win);
        for (int i = 0; i < tk.n; i++) {
          double cx = tk.bodies[i].pos.x / cfg.width * attr.width,
                 cy = tk.bodies[i].pos.y / cfg.height * attr.height,
                 rx = tk.bodies[i].r / cfg.width * attr.width,
                 ry = tk.bodies[i].r / cfg.height * attr.height;
          XFillArc(dpy, win, gc,
              cx - rx, cy - ry, rx * 2, ry * 2,
              0, 64 * 360);
        }
      }
      break;
    case ClientMessage:
      if (((Atom)e.xclient.data.l[0] == wmDeleteMessage))
        return;
    }
  }
}
