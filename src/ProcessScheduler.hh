#ifndef PROCESSSCHEDULE_HH
#define PROCESSSCHEDULE_HH

#include <algorithm>
#include <vector>
#include "Core.hh"
#include "ThreadScheduler.hh"

enum Event
{
  INITIAL_TASK, ADVANCE, FINISH, END
};

void processSchedule();

class Master
{
public:
  Master(const Task& tk);
  ~Master();
  void advance(ft dt);
  Task tk;
};

#endif /* end of include guard: PROCESSSCHEDULE_HH */
