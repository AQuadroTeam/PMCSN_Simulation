#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "linked_list.h"

#define EVENT_ARRIVE1  1
#define EVENT_ARRIVE2  2
#define EVENT_COMPLETED_1_IN_1  3
#define EVENT_COMPLETED_1_IN_2  4
#define EVENT_COMPLETED_2_IN_1  5
#define EVENT_COMPLETED_2_IN_2  6
#define EVENT_COMPLETED_2_IN_SETUP  7

#define lambda_1 3.25
#define lambda_2 6.25
#define mu_cloudlet_1 0.45
#define mu_cloudlet_2 0.30
#define mu_cloud_1 0.25
#define mu_cloud_2 0.22
#define mu_setup_2 1.25

struct Event {
  double time;
  int type;
  struct Event * next;
  double arrival_time;
  int route;
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
double t_current = 0.0;
double t_begin = 0.0;
double t_end = 0.0;
int N;
int S;

/*
    STATISTICAL FUNCTIONS
*/
double generate_exp(double mean)
{
  return 0.0; //TODO: add custom exp
}




/*
    INITIALIZATION
*/

int initialize_parameters(int argc, char ** argv)
{
  if(argc != 4)
  {
    fprintf(stderr, "Usage: %s <N> <S> <end_time>\n", argv[0]);
    return EXIT_FAILURE;
  }
  else
  {
    N = strtol(argv[1] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - N\n");
      return EXIT_FAILURE;
    }

    S = strtol(argv[2] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - S\n");
      return EXIT_FAILURE;
    }

    t_end = strtod(argv[3] , NULL);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - time\n");
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
}

int get_event_type(struct Event t_event){
  return t_event.type;
}

int get_system_state(){}

void initialize_state()
{
  state.cloudlet_1 = 0;
  state.cloudlet_2 = 0;
  state.cloud_1 = 0;
  state.cloud_2 = 0;
  state.setup_2 = 0;
}

void initialize_events()
{
  /*EVENT_ARRIVE1, generate_exp(lambda_1)
  push_event();
  push_event(EVENT_ARRIVE2, generate_exp(lambda_2));*/
}

/*

*/

void process_event(struct Event * first)
{

}







int main(int argc, char ** argv)
{
  if(initialize_parameters(argc, argv) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  initialize_state();
  initialize_events();
  while(1)
  {
    process_event(events);

  }
  return EXIT_SUCCESS;


}
