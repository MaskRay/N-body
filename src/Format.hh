#ifndef FORMAT_HH
#define FORMAT_HH

#include "config.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "Core.hh"
#include "ProcessScheduler.hh"

class Format
{
public:
  Format(const Task& tk) : fsm(tk) {}
  virtual ~Format() {}
  virtual void render() = 0;
protected:
  Master fsm;
};

class TextFormat : public Format
{
public:
  TextFormat(const Task& tk) : Format(tk) {}
  virtual void render();
};

class TimeFormat : public Format
{
public:
  TimeFormat(const Task& tk) : Format(tk) {}
  virtual void render();
};

#endif /* end of include guard: FORMAT_HH */
