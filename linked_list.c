#include <stdlib.h>
#include <stdio.h>

struct Event {
  double time;
  int type;
  struct Event * next;
  double arrival_time;
  int route;
};

struct Event * push_event(struct Event *event){}
struct Event * remove_event(struct Event *event){}
struct Event * pop_event(){}
struct Event * remove_last_event_of_type(int type){}
