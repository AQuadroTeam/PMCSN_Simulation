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

void initialize_state()
{
  state.cloudlet_1 = 0;
  state.cloudlet_2 = 0;
  state.cloud_1 = 0;
  state.cloud_2 = 0;
  state.setup_2 = 0;
}

double generate_exp(mean)
{
  return 0.0; //TODO: add custom exp
}

void initialize_events()
{
  push_event(EVENT_ARRIVE1, generate_exp(lambda_1));
  push_event(EVENT_ARRIVE2, generate_exp(lambda_2));
}

void process_arrive_1_all(struct Event * event)
{
  if(state.cloudlet_1 == N )
}

void process_event(struct Event * first)
{
  switch (first->type):
  case ARRIVE_1:
    process_arrive_1(first)

}

int main(int argc, char ** argv)
{
  initialize_state();
  initialize_events();
  while()
  {
    process_event(events);

  }
  return EXIT_SUCCESS;


}
