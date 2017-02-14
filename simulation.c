#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "linked_list.h"
#include "rngs.h"
#include "rvms.h"
#include "rvgs.h"

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

#define DEBUG 1

struct Event {
  double time;
  int type;
  struct Event * next;
  struct Event * prev;
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
double t_current = 0.0;
double t_begin = 0.0;
double t_end = 0.0;
int N;
int S;
long initial_seed;


double get_t(){
  return t_current;
}

/*
    STATISTICAL FUNCTIONS
*/
double generate_exp(double lambda, int stream)
{
  SelectStream(stream);
  return Exponential(1/lambda); //rngs use mean instead of parameter
}

double generate_next_time(double lambda, int stream){
  return generate_exp(lambda, stream) + get_t();
}


struct Event * generate_arrive_event(double lambda, int EVENT){
  struct Event * event = calloc(sizeof(struct Event), 1);
  if(event == NULL){
    fprintf(stderr, "Error in calloc!\n");
    return NULL;
  }

  event->next = NULL;
  event->prev = NULL;
  event->time = generate_next_time(lambda, EVENT);
  event->type = EVENT;
  event->arrival_time = get_t();
  event->route = -1;

  return event;
}

void destroy_event(struct Event * event){
  free(event);
}


/*
    INITIALIZATION
*/

int initialize_parameters(int argc, char ** argv)
{
  if(argc != 5)
  {
    fprintf(stderr, "Usage: %s <N> <S> <end_time> <initial_seed>\n", argv[0]);
    return EXIT_FAILURE;
  }
  else
  {
    errno = 0;
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

    initial_seed = strtol(argv[4] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - initial seed\n");
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
}

int get_event_type(struct Event t_event){
  return t_event.type;
}

int get_system_state(){

}

void initialize_state()
{
  state.cloudlet_1 = 0;
  state.cloudlet_2 = 0;
  state.cloud_1 = 0;
  state.cloud_2 = 0;
  state.setup_2 = 0;
}

void initialize_generators(long seed){
  PlantSeeds(seed);
}


void initialize_events()
{
  struct Event * first_1 = generate_arrive_event(lambda_1, EVENT_ARRIVE1);
  push_event(first_1);

  struct Event * first_2 = generate_arrive_event(lambda_2, EVENT_ARRIVE2);
  push_event(first_2);

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
  initialize_generators(initial_seed);
  initialize_state();
  initialize_events();

  printf("Started simulation with N=%d, S=%d, t_end=%f, seed=%ld\n", N,S,t_end,initial_seed);


  while(1)
  {


  }
  return EXIT_SUCCESS;


}
