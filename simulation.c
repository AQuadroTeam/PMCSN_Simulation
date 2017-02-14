#include <stdio.h>
#include <stdlib.h>

#define EVENT_ARRIVE1 = 1
#define EVENT_ARRIVE2 = 2
#define EVENT_COMPLETED_1_IN_1 = 3
#define EVENT_COMPLETED_1_IN_2 = 4
#define EVENT_COMPLETED_2_IN_1 = 5
#define EVENT_COMPLETED_2_IN_2 = 6
#define EVENT_COMPLETED_2_IN_SETUP = 7

struct Event {
  double time;
  int type;
};

struct State {
  int cloudlet_1; //n1
  int cloudlet_2; //n2
  int cloud_1;
  int cloud_2;
  int setup_2;
};

struct State state;
struct Event * events = NULL;
double t_current;
double t_begin;
double t_end;

int main(int argc, char ** argv)
{
  return EXIT_SUCCESS;


}
