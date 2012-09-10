#include <stdio.h>
#include "Format.hh"
#include "Config.hh"

void TimeFormat::render()
{
  Task& tk = fsm.tk;
  struct timeval bgn, end;
  gettimeofday(&bgn, NULL);
  for (int i = 0; i < cfg.iter; i++)
    fsm.advance(1.0 / cfg.fps);
  gettimeofday(&end, NULL);
  if (end.tv_usec < bgn.tv_usec) {
    end.tv_usec += 1000000;
    end.tv_sec--;
  }
  printf("running time: %ld.%06ld seconds\n", end.tv_sec - bgn.tv_sec, end.tv_usec - bgn.tv_usec);
}

void TextFormat::render()
{
  Task& tk = fsm.tk;
  struct timeval bgn, end;
  for (int i = 0; i < cfg.iter; i++) {
    gettimeofday(&bgn, NULL);
    fsm.advance(1.0 / cfg.fps);
    gettimeofday(&end, NULL);
    if (end.tv_usec < bgn.tv_usec) {
      end.tv_usec += 1000000;
      end.tv_sec--;
    }
    ft usec = 1000000.0 / cfg.fps - (end.tv_usec - bgn.tv_usec + (end.tv_sec - bgn.tv_sec) * 1000000.0);
    if (usec > 0)
      usleep(usec);
    printf("%d\n", tk.n);
    for (int j = 0; j < tk.n; j++)
      printf("%lf %lf %lf %lf\n", tk.bodies[j].pos.x, tk.bodies[j].pos.y, tk.bodies[j].pos.z, tk.bodies[j].r);
    fflush(stdout);
  }
  //printf("running time: %ld.%06ld seconds\n", end.tv_sec - bgn.tv_sec, end.tv_usec - bgn.tv_usec);
}
