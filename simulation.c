#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "linked_list.h"
#include "rngs.h"
#include "rvms.h"
#include "rvgs.h"

#define EVENT_ARRIVE1  0
#define EVENT_ARRIVE2  1
#define EVENT_COMPLETED_1_IN_1  3
#define EVENT_COMPLETED_1_IN_2  4
#define EVENT_COMPLETED_2_IN_1  5
#define EVENT_COMPLETED_2_IN_2  6
#define EVENT_COMPLETED_2_IN_SETUP  2

#define FREE 0
#define BUSY_CLASS_2 1
#define BUSY 2

#define PATH_1_1 3
#define PATH_1_2 4
#define PATH_2_1 5
#define PATH_2_2 6
#define PATH_2_S_2 7
#define PATH_NOT_TO_UPDATE -2

#define lambda_1 3.25
#define lambda_2 6.25
#define mu_cloudlet_1 0.45
#define mu_cloudlet_2 0.30
#define mu_cloud_1 0.25
#define mu_cloud_2 0.22
#define mu_setup_2 1.25

int DEBUG =1;

struct Event {
  double time;
  int type;
  struct Event * next;
  struct Event * prev;
  double arrival_time;
  int path;
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

int batch_active = 0;

struct Batch_Stat {
  long counter_per_path[5];
  // 2 events arrive are always on system
  long counter_generated ;
  long counter_exited;
  long double mean_time_per_path[5];
  long double mean_time_wasted_in_cloudlet;
};

struct Batch_Stat *stats;

// Parameters
double t_end = 0.0;
int batch_number_total= 0;
int N;
int S;
long initial_seed;

long double tot_mean_counter_exited(){
  long double mean = 0.0;
  int n = sizeof(stats)/sizeof(stats[0]);
  for(int i=0; i<n; i++){
    if (isnan(stats[i].counter_exited) != 1){
      mean += stats[i].counter_exited;
    }
  }
  return mean/n;
}

long double tot_mean_counter_generated(){
  long double mean = 0.0;
  int n = sizeof(stats)/sizeof(stats[0]);
  for(int i=0; i<n; i++){
    if (isnan(stats[i].counter_generated) != 1){
      mean += stats[i].counter_generated;
    }
  }
  return mean/n;
}

long double tot_mean_counter_per_path(int s_path){
  long double mean = 0.0;
  int n = sizeof(stats)/sizeof(stats[0]);
  for(int i=0; i<n; i++){
    if (isnan(stats[i].counter_per_path[s_path])!=1){
      mean += stats[i].counter_per_path[s_path];
    }
  }
  return mean/n;
}

long double tot_mean_time_per_path(int s_path){
  long double mean = 0.0;
  int n = sizeof(stats)/sizeof(stats[0]);
  for(int i=0; i<n; i++){
    if (isnan(stats[i].mean_time_per_path[s_path])!=1){
      mean += stats[i].mean_time_per_path[s_path];
    }
  }
  return mean/n;
}

long double tot_mean_time_wasted_in_cloudlet(){
  long double mean = 0.0;
  int n = sizeof(stats)/sizeof(stats[0]);
  for(int i=0; i<n; i++){
    if (isnan(stats[i].mean_time_wasted_in_cloudlet)!=1){
      mean += stats[i].mean_time_wasted_in_cloudlet;
    }
  }
  return mean/n;
}

long * counter_per_path_now(){
  return stats[batch_active].counter_per_path;
}
long counter_generated_now(){
  return stats[batch_active].counter_generated;
}
long counter_exited_now(){
  return stats[batch_active].counter_exited;
}
long double * mean_time_per_path_now(){
  return stats[batch_active].mean_time_per_path;
}
long double mean_time_wasted_in_cloudlet_now(){
  return stats[batch_active].mean_time_wasted_in_cloudlet;
}

void counter_per_path_increment(int path){
  stats[batch_active].counter_per_path[path]++;
}
void counter_generated_increment(){
  stats[batch_active].counter_generated ++;
}
void counter_exited_increment(){
  stats[batch_active].counter_exited++;
}
void mean_time_per_path_add(int path, long double time){
  stats[batch_active].mean_time_per_path[path]+=time;
}
void mean_time_wasted_in_cloudlet_add(long double time){
  stats[batch_active].mean_time_wasted_in_cloudlet+=time;
}

void set_mean_time_per_path(){
  int n = sizeof(mean_time_per_path_now())/sizeof(mean_time_per_path_now()[0]);
  for (int i=0;i<n;i++){
    mean_time_per_path_now()[i] = mean_time_per_path_now()[i]/counter_per_path_now()[i];
  }
}

void set_mean_wasted_time_for_cloudlet(){
  stats[batch_active].mean_time_wasted_in_cloudlet = mean_time_wasted_in_cloudlet_now()/counter_per_path_now()[4];
}


double get_t(){
  return t_current;
}

void set_t(double time_now){
  t_current = time_now;
}

void print_actual_state()
{
  fprintf(stdout, "state at %f: cloudlet (%d,%d), cloud (%d,%d), setup %d. Path completed: cloudlet(%ld,%ld),cloud(%ld,%ld),withSetup(%ld). Entered %ld, Served %ld\n",
    get_t(),state->cloudlet_1,state->cloudlet_2,state->cloud_1,state->cloud_2,state->setup_2, counter_per_path_now()[0],counter_per_path_now()[2],counter_per_path_now()[1],counter_per_path_now()[3],counter_per_path_now()[4],counter_generated_now(),counter_exited_now());
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

struct Event * generate_completion_event(struct Event * event, double mu, int next_event_type, int path){

  event->time = generate_next_time(mu, next_event_type);
  event->type = next_event_type;
  if (path != PATH_NOT_TO_UPDATE){
    event->path = path;
  }
  if(DEBUG){printf("Event Generation: Completion of packet type %d path %d arrived at %f, next time %f\n", next_event_type,event->path, event->arrival_time, event->time);}

  return event;
}

struct Event * generate_arrive_event(double lambda, int EVENT){
  struct Event * event = (struct Event *)calloc(sizeof(struct Event), 1);
  if(event == NULL){
    fprintf(stderr, "Error in calloc!\n");
    return NULL;
  }
  // a job is entered in system, update counter;
  counter_generated_increment();

  event->next = NULL;
  event->prev = NULL;
  event->time = generate_next_time(lambda, EVENT);
  event->type = EVENT;
  event->arrival_time = event->time; //Arrival time must be when the packet arrives, not when event is generated!!!
  event->path = -1; //Invalid path

  if(DEBUG){printf("Event Generation: Arrived %d at %f, next time %f\n", EVENT, event->arrival_time, event->time);}

  return event;
}

void exit_event(struct Event * event){
  counter_exited_increment();
  counter_per_path_increment(event->path-3);
  mean_time_per_path_add(event->path-3, get_t()-event->arrival_time);

  if(DEBUG){printf("Event Destroyed: Exited packet with path %d at %f after %f\n", event->path, get_t(),get_t()-event->arrival_time);}
  free(event);
}

//Transition Matrix

int arrive_1_free(struct Event *ev){
  if(DEBUG){printf("\nArrived 1 in free\n");}
  if(DEBUG)print_actual_state();
  state->cloudlet_1++;

  //Complete event 1_1
  generate_completion_event(ev, mu_cloudlet_1, EVENT_COMPLETED_1_IN_1, PATH_1_1);
  push_event(ev);

  //Generate next arrival 1
  struct Event * arrival = generate_arrive_event(lambda_1, EVENT_ARRIVE1);
  push_event(arrival);

  if(DEBUG)print_actual_state();
  return 0;
}

int arrive_2_free(struct Event *ev){
  if(DEBUG){printf("\nArrived 2 free\n");}
  if(DEBUG)print_actual_state();
  state->cloudlet_2++;


  //Complete event 2_1
  generate_completion_event(ev, mu_cloudlet_2, EVENT_COMPLETED_2_IN_1, PATH_2_1);
  push_event(ev);

  //Generate next arrival 2
  struct Event * arrival = generate_arrive_event(lambda_2, EVENT_ARRIVE2);
  push_event(arrival);

  if(DEBUG)print_actual_state();

  return 0;
}

void manage_setup_arrival(struct Event *ev){
  if(DEBUG){printf("\nSetup 2\n");}
  if(DEBUG)print_actual_state();
  state->setup_2--;
  state->cloud_2++;

  //Complete event 2_2
  generate_completion_event(ev, mu_cloud_2, EVENT_COMPLETED_2_IN_2, PATH_2_S_2);
  push_event(ev);

  if(DEBUG)print_actual_state();
}

int setup_free(struct Event *ev){
  manage_setup_arrival(ev);
  return 0;
}

int arrive_1_busy(struct Event *ev){
  if(DEBUG){printf("\nArrived 1 in Busy\n");}
  if(DEBUG)print_actual_state();
  state->cloud_1++;

  //Complete event 1_2
  generate_completion_event(ev, mu_cloud_1, EVENT_COMPLETED_1_IN_2, PATH_1_2);
  push_event(ev);

  //Generate next arrival 1
  struct Event * arrival = generate_arrive_event(lambda_1, EVENT_ARRIVE1);
  push_event(arrival);
  if(DEBUG)print_actual_state();

  return 0;
}

int arrive_2_busy(struct Event *ev){
  if(DEBUG){printf("\nArrived 2 in Busy\n");}
  if(DEBUG)print_actual_state();
  state->cloud_2++;

  //Complete event 2_2
  generate_completion_event(ev, mu_cloud_2, EVENT_COMPLETED_2_IN_2, PATH_2_2);
  push_event(ev);

  //Generate next arrival 2
  struct Event * arrival = generate_arrive_event(lambda_2, EVENT_ARRIVE2);
  push_event(arrival);
  if(DEBUG)print_actual_state();

  return 0;
}

int setup_busy(struct Event *ev){
  manage_setup_arrival(ev);
  return 0;
}

int arrive_1_busy_2(struct Event *ev){
  if(DEBUG){printf("\nArrived 1 in Busy 2\n");}
  if(DEBUG)print_actual_state();
  state->cloudlet_1++;
  state->cloudlet_2--;
  state->setup_2++;

  //gen compl_1_1 and compl_setup and arrivo_1

  //Complete event 1_1
  generate_completion_event(ev, mu_cloudlet_1, EVENT_COMPLETED_1_IN_1, PATH_1_1);
  push_event(ev);

  //Complete event 2_setup, seek last event in list with type EVENT_COMPLETED_2_IN_1
  struct Event *to_remove = remove_last_event_of_type(EVENT_COMPLETED_2_IN_1);
  generate_completion_event(to_remove, mu_setup_2, EVENT_COMPLETED_2_IN_SETUP, PATH_2_S_2);
  mean_time_wasted_in_cloudlet_add(get_t()-to_remove->arrival_time);
  push_event(to_remove);
  //Generate next arrival 1
  struct Event * arrival = generate_arrive_event(lambda_1, EVENT_ARRIVE1);
  push_event(arrival);
  if(DEBUG)print_actual_state();

  return 0;
}

int arrive_2_busy_2(struct Event *ev){
  if(DEBUG){printf("\nArrived 2 in Busy 2\n");}
  if(DEBUG)print_actual_state();
  state->cloud_2++;

  //Complete event 2_2
  generate_completion_event(ev, mu_cloud_2, EVENT_COMPLETED_2_IN_2, PATH_2_2);
  push_event(ev);

  //Generate next arrival 2
  struct Event * arrival = generate_arrive_event(lambda_2, EVENT_ARRIVE2);
  push_event(arrival);
  if(DEBUG)print_actual_state();

  return 0;
}

int setup_busy_2(struct Event *ev){
  manage_setup_arrival(ev);
  return 0;
}

int compl_1_1(struct Event *ev){
  if(DEBUG){fprintf(stderr,"\nCompletion 1 in Cloudlet\n");}
  if(DEBUG)print_actual_state();
  state->cloudlet_1--;

  exit_event(ev);

  if(DEBUG)print_actual_state();

  return 0;
}

int compl_1_2(struct Event *ev){
  if(DEBUG){fprintf(stdout,"\nCompletion 1 in Cloud\n");}
  if(DEBUG)print_actual_state();
  state->cloud_1--;

  exit_event(ev);

  if(DEBUG)print_actual_state();

  return 0;
}

int compl_2_1(struct Event *ev){
  if(DEBUG){fprintf(stdout,"\nCompletion 2 in Cloudlet\n");}
  if(DEBUG)print_actual_state();
  state->cloudlet_2--;
  exit_event(ev);
  if(DEBUG)print_actual_state();

  return 0;
}

int compl_2_2(struct Event *ev){
  if(DEBUG){fprintf(stdout, "\nCompletion 2 in Cloud\n");}
  if(DEBUG)print_actual_state();
  state->cloud_2--;
  exit_event(ev);
  if(DEBUG)print_actual_state();
  return 0;
}

int (*transition_matrix[3][3])(struct Event *) = {{arrive_1_free, arrive_1_busy_2, arrive_1_busy},
                                    {arrive_2_free, arrive_2_busy_2, arrive_2_busy_2},
                                    {setup_free, setup_busy_2, setup_busy}};



/*
    INITIALIZATION
*/

int initialize_parameters(int argc, char ** argv)
{
  if(argc != 7)
  {
    fprintf(stderr, "Usage: %s <N> <S> <end_time> <initial_seed> <# of batch intervals> <debug 0 or 1>\n", argv[0]);
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

    batch_number_total = strtol(argv[5] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - batch number total\n");
      return EXIT_FAILURE;
    }

    DEBUG = strtol(argv[6] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - debug must be 0 or 1\n");
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

void decrease_job(struct Event *ev){
  switch (get_event_type(ev)) {
    case EVENT_COMPLETED_1_IN_1:
       compl_1_1(ev);
       break;
    case EVENT_COMPLETED_1_IN_2:
       compl_1_2(ev);
       break;
    case EVENT_COMPLETED_2_IN_1:
       compl_2_1(ev);
       break;
    case EVENT_COMPLETED_2_IN_2:
       compl_2_2(ev);
       break;
   }
}

void gen_next_ev(struct Event *ev, int s_state){
  if (transition_matrix[get_event_type(ev)][s_state](ev)){
    fprintf(stderr, "Error generating next event\n");
    exit(EXIT_FAILURE);
  };
}

void process_event(struct Event *ev){
  int s_state = get_system_state(get_event_type(ev));
  int ev_type = get_event_type(ev);

  //set t_current
  set_t(ev->time);
  int is_arrival = ev_type == EVENT_ARRIVE1 || ev_type == EVENT_ARRIVE2 || ev_type == EVENT_COMPLETED_2_IN_SETUP;
  if (is_arrival){
    gen_next_ev(ev, s_state);
  }
  else{
    decrease_job(ev);
  }
}

void initialize_state()
{
  state = (struct State *) calloc(sizeof(struct Event), 1);

  state->cloudlet_1 = 0;
  state->cloudlet_2 = 0;
  state->cloud_1 = 0;
  state->cloud_2 = 0;
  state->setup_2 = 0;
  t_current = t_begin;

}
void initialize_batch_stats(){
  stats = (struct Batch_Stat *)calloc(sizeof(struct Batch_Stat), batch_number_total);
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

int main(int argc, char ** argv)
{

  if(initialize_parameters(argc, argv) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  initialize_batch_stats();
  initialize_generators(initial_seed);
  initialize_state();
  initialize_events();

  printf("Started simulation with N=%d, S=%d, batch_time_total=%f, batch#=%d, seed=%ld\n", N,S,t_end,batch_number_total,initial_seed);

  for (batch_active = 0; batch_active < batch_number_total; batch_active++){
    while(get_t() <= t_end*(batch_active+1) )
    {
      process_event(pop_event());
    }
    printf("\n\nBATCH %d finished\n", batch_active);
    print_actual_state();


    mean_time_per_path_now()[0] = mean_time_per_path_now()[0]/counter_per_path_now()[0];



    printf("Mean should be:\n%f - %f - %f - %f - ?\n",1/mu_cloudlet_1, 1/mu_cloud_1, 1/mu_cloudlet_2, 1/mu_cloud_2);
    printf("Mean for path: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%Lf - %Lf - %Lf - %Lf - %Lf. Mean of wasted time %Lf \n", mean_time_per_path_now()[0],mean_time_per_path_now()[1]/counter_per_path_now()[1],mean_time_per_path_now()[2]/counter_per_path_now()[2],mean_time_per_path_now()[3]/counter_per_path_now()[3],mean_time_per_path_now()[4]/counter_per_path_now()[4], mean_time_wasted_in_cloudlet_now()/counter_per_path_now()[4]);
    printf("P calculated: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%f - %f - %f - %f - %f\n", counter_per_path_now()[0]*1.0/counter_exited_now(),counter_per_path_now()[1]*1.0/counter_exited_now(),counter_per_path_now()[2]*1.0/counter_exited_now(),counter_per_path_now()[3]*1.0/counter_exited_now(),counter_per_path_now()[4]*1.0/counter_exited_now());
  }

  set_mean_time_per_path();
  set_mean_wasted_time_for_cloudlet();
  printf("End Simulation\n ");
  printf("Mean should be:\n%f - %f - %f - %f - ?\n",1/mu_cloudlet_1, 1/mu_cloud_1, 1/mu_cloudlet_2, 1/mu_cloud_2);
  printf("Mean for path: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%Lf - %Lf - %Lf - %Lf - %Lf. Mean of wasted time %Lf \n", tot_mean_time_per_path(0),tot_mean_time_per_path(1),tot_mean_time_per_path(2),tot_mean_time_per_path(3),tot_mean_time_per_path(4), tot_mean_time_wasted_in_cloudlet());
  printf("P calculated: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%Lf - %Lf - %Lf - %Lf - %Lf\n", tot_mean_counter_per_path(0)*1.0/tot_mean_counter_exited(),tot_mean_counter_per_path(1)*1.0/tot_mean_counter_exited(),tot_mean_counter_per_path(2)*1.0/tot_mean_counter_exited(),tot_mean_counter_per_path(3)*1.0/tot_mean_counter_exited(),tot_mean_counter_per_path(4)*1.0/tot_mean_counter_exited());

  return EXIT_SUCCESS;
}
