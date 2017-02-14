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

#define FREE 0
#define BUSY_CLASS_2 1
#define BUSY 2

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

struct State *state;
double t_current = 0.0;
double t_begin = 0.0;
double t_end = 0.0;
int N;
int S;

//Transition Matrix

int arrive_1_free(){
  state->cloudlet_1++;
  //gen compl_1_1 and arrivo_1
  return 0;
}

int arrive_2_free(){
  state->cloudlet_2++;
  //gen compl_2_1 and arrivo_2
  return 0;
}

int setup_free(){
  state->setup_2--;
  state->cloud_2++;
  //gen compl_2_2
  return 0;
}

int arrive_1_busy(){
  state->cloud_2++;
  //gen compl_1_2 and arrivo_1
  return 0;
}

int arrive_2_busy(){
  state->cloud_2++;
  //gen compl_2_2 and arrivo_2
  return 0;
}

int setup_busy(){
  state->setup_2--;
  state->cloud_2++;
  //gen compl_2_2
  return 0;
}

int arrive_1_busy_2(){
  state->cloudlet_1++;
  state->cloudlet_2--;
  state->setup_2++;
  //gen compl_1_1 and compl_setup and arrivo_1
  return 0;
}

int arrive_2_busy_2(){
  state->cloud_2++;
  //gen compl_2_2 and arrivo_2
  return 0;
}

int setup_busy_2(){
  state->setup_2--;
  state->cloud_2++;
  //gen compl_2_2
  return 0;
}

int compl_1_1(){
  state->cloudlet_1--;
  return 0;
}

int compl_1_2(){
  state->cloudlet_2--;
  return 0;
}

int compl_2_1(){
  state->cloud_1--;
  return 0;
}

int compl_2_2(){
  state->cloud_2--;
  return 0;
}

int (*transition_matrix[3][3])() = {{arrive_1_free, arrive_1_busy_2, arrive_1_busy},
                                    {arrive_2_free, arrive_2_busy_2, arrive_2_busy_2},
                                    {setup_free, setup_busy_2, setup_busy}};

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

int get_event_type(struct Event *t_event){
  return t_event->type;
}

int get_system_state(int ev_type){

  if (state->cloudlet_1 + state->cloudlet_2 < S){
    return FREE;
  }
  if (state->cloudlet_1 + state->cloudlet_2 >= S && state->cloudlet_1 < N){
    if (state->cloudlet_2 > 0){
      return BUSY_CLASS_2;
    }
    else{
      if (ev_type == EVENT_ARRIVE1){
        return FREE;
      }
      else {
        //Each event consider the system busy, if it is not arrival_1
        return BUSY;
      }
    }
  }
  else if (state->cloudlet_1 >= N) {
    return BUSY;
  }
  else {
    fprintf(stderr, "Not a state in function get_system_state");
    exit(EXIT_FAILURE);
  }

}

void decrease_job(int ev_type){
  switch (ev_type) {
    case EVENT_COMPLETED_1_IN_1:
       compl_1_1();
       break;
    case EVENT_COMPLETED_1_IN_2:
       compl_1_2();
       break;
    case EVENT_COMPLETED_2_IN_1:
       compl_2_1();
       break;
    case EVENT_COMPLETED_2_IN_2:
       compl_2_2();
       break;
   }
}

struct Event gen_next_ev(int ev_type, int s_state){
  if (transition_matrix[ev_type][s_state]()){
    fprintf(stderr, "Error generating next event\n");
    exit(EXIT_FAILURE);
  };
}

void process_event(int ev_type){
  int s_state = get_system_state(ev_type);
  bool is_arrival = 0;
  if (is_arrival){
    struct Event n_ev = gen_next_ev(ev_type, s_state);
  }
  else{
    decrease_job(ev_type);
  }
}

void initialize_state()
{
  state->cloudlet_1 = 0;
  state->cloudlet_2 = 0;
  state->cloud_1 = 0;
  state->cloud_2 = 0;
  state->setup_2 = 0;
}

void initialize_events()
{
  /*EVENT_ARRIVE1, generate_exp(lambda_1)
  push_event();
  push_event(EVENT_ARRIVE2, generate_exp(lambda_2));*/
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


  }
  return EXIT_SUCCESS;


}
