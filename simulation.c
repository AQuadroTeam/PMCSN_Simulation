#include <stdio.h>
#include <stdlib.h>
EVENT_ARRIVE1 = 1
EVENT_ARRIVE2 = 2
EVENT_COMPLETED_1_IN_1 = 3
EVENT_COMPLETED_1_IN_2 = 4
EVENT_COMPLETED_2_IN_1 = 5
EVENT_COMPLETED_2_IN_2 = 6
EVENT_COMPLETED_2_IN_SETUP = 7

struct Event {
  double time;
  int type;
}

struct State {
  int cloudlet_1; //n1
  int cloudlet_2; //n2
  int cloud_1;
  int cloud_2;
  int setup_2;
}

struct State state;
struct Event * events = NULL;
double t_current;
double t_begin;
double t_end;

int main(int argc, char ** argv)
{
  return EXIT_SUCCESS;


}
