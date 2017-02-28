#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "linked_list.h"
#include "rngs.h"
#include "rvms.h"
#include "rvgs.h"

/*
    MACROS
*/

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

#define GOVERNOR_FIRST = 0
#define GOVERNOR_LAST = 1
#define GOVERNOR_RND = 2

#define lambda_1 3.25
#define lambda_2 6.25
#define mu_cloudlet_1 0.45
#define mu_cloudlet_2 0.30
#define mu_cloud_1 0.25
#define mu_cloud_2 0.22
#define mu_setup_2 1.25

int DEBUG =1;

/*
    Structs
*/

struct Event {
  double time;
  int type;
  struct Event * next;
  struct Event * prev;
  double arrival_time;
  int path;
  double wasted_time;
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

struct Tot_Mean_Time {
  long double first_clet;
  long double second_clet;
  long double first_cloud;
  long double second_cloud;
  long double setup_cloud;
};

struct Tot_Std_Time {
  long double first_clet;
  long double second_clet;
  long double first_cloud;
  long double second_cloud;
  long double setup_cloud;
};

struct Tot_Wasted_Time {
  long double clet;
};

struct Tot_Std_Wasted_Time {
  long double clet;
};

struct Probabilities {
  double first_clet;
  double second_clet;
  double first_cloud;
  double second_cloud;
  double setup_cloud;
};

struct Std_Probabilities {
  double first_clet;
  double second_clet;
  double first_cloud;
  double second_cloud;
  double setup_cloud;
};


struct Batch_Stat *stats;
struct Tot_Mean_Time *end_means;
struct Tot_Wasted_Time *w_times;
struct Probabilities *probs;
struct Tot_Std_Time *end_stds;
struct Tot_Std_Wasted_Time *w_stds;
struct Std_Probabilities *probs_stds;

long double calc_general_path_mean(struct Tot_Mean_Time *end_means, struct Probabilities* probs){
  return end_means->first_clet*probs->first_clet + end_means->second_clet*probs->second_clet +
    end_means->first_cloud*probs->first_cloud + end_means->second_cloud*probs->second_cloud + end_means->setup_cloud*probs->setup_cloud;
}

/*
    Parameters
*/

double t_end = 0.0;
int batch_number_total= 0;
int N;
int S;
long initial_seed;
int PREEMPTION_GOVERNOR = 0;
FILE * export_file;
double alpha = 0.05;

//To save response times for each batch
long double *batch_response_times;

/*
    BATCH FUNCTIONS
*/

void set_end_means(long double f_clet, long double s_clet, long double f_cloud, long double s_cloud, long double setup_cloud){
  end_means->first_clet = f_clet;
  end_means->second_clet = s_clet;
  end_means->first_cloud = f_cloud;
  end_means->second_cloud = s_cloud;
  end_means->setup_cloud = setup_cloud;
}

void set_end_stds(long double f_clet, long double s_clet, long double f_cloud, long double s_cloud, long double setup_cloud){
  end_stds->first_clet = f_clet;
  end_stds->second_clet = s_clet;
  end_stds->first_cloud = f_cloud;
  end_stds->second_cloud = s_cloud;
  end_stds->setup_cloud = setup_cloud;
}

void set_probabilities(long double f_clet, long double s_clet, long double f_cloud, long double s_cloud, long double setup_cloud){
  probs->first_clet = (double) f_clet;
  probs->second_clet = (double) s_clet;
  probs->first_cloud = (double) f_cloud;
  probs->second_cloud = (double) s_cloud;
  probs->setup_cloud = (double) setup_cloud;
}

void set_wasted_stds(long double clet){
  w_stds->clet = clet;
}

void set_wasted_times(long double clet){
  w_times->clet = clet;
}

long double tot_mean_counter_exited(){
  long double mean = 0.0;
  int n = batch_number_total;
  for(int i=0; i<n; i++){
    if (isnan((double)stats[i].counter_exited) != 1){
      mean += stats[i].counter_exited/n;
    }
  }
  return mean;
}

long double tot_mean_counter_generated(){
  long double mean = 0.0;
  int n = batch_number_total;
  for(int i=0; i<n; i++){
    if (isnan((double)stats[i].counter_generated) != 1){
      mean += stats[i].counter_generated/n;
    }
  }
  return mean;
}

long double tot_mean_counter_per_path(int s_path){
  long double mean = 0.0;
  int n = batch_number_total;
  for(int i=0; i<n; i++){
    if (isnan((double)stats[i].counter_per_path[s_path])!=1){
      mean += stats[i].counter_per_path[s_path]/n;
    }
  }
  return mean;
}

long double tot_mean_time_per_path(int s_path){
  long double mean = 0.0;
  int n = batch_number_total;
  for(int i=0; i<n; i++){
    if (isnan((double)stats[i].mean_time_per_path[s_path])!=1){
      mean += stats[i].mean_time_per_path[s_path]/n;
    }
  }
  return mean;
}

long double tot_mean_time_wasted_in_cloudlet(){
  long double mean = 0.0;
  int n = batch_number_total;
  for(int i=0; i<n; i++){
    if (isnan((double)stats[i].mean_time_wasted_in_cloudlet)!=1){
      mean += stats[i].mean_time_wasted_in_cloudlet/n;
    }
  }
  return mean;
}

long double calculate_sd_total_response_time(long double mean)
{
    long double standard_deviation = 0.0;
    int i;

    for(i=0; i<batch_number_total; i++){
      if (isnan(batch_response_times[i])!=1){
        standard_deviation += pow(batch_response_times[i] - mean, 2)/batch_number_total;
      }
    }
    return sqrt(standard_deviation);
}

long double calculate_medium_total_response_time_2()
{
    long double mean = 0.0;
    int i;

    for(i=0; i<batch_number_total; i++){
      if (isnan(batch_response_times[i])!=1){
        mean += batch_response_times[i]/batch_number_total;
      }
    }
    return mean;
}


long double calculate_sd_time_per_path(int s_path, long double mean)
{
    long double standardDeviation = 0.0;
    int i;

    for(i=0; i<batch_number_total; i++){
      if (isnan((double)stats[i].mean_time_per_path[s_path])!=1){
        standardDeviation += pow((double)stats[i].mean_time_per_path[s_path] - mean, 2)/batch_number_total;
      }
    }
    return sqrt(standardDeviation);
}

long double calculate_sd_wasted_time(long double mean)
{
    long double standardDeviation = 0.0;
    int i;

    for(i=0; i<batch_number_total; i++){
      if (isnan((double)stats[i].mean_time_wasted_in_cloudlet)!=1){
        standardDeviation += pow((double)stats[i].mean_time_wasted_in_cloudlet - mean, 2)/batch_number_total;
      }
    }
    return sqrt(standardDeviation);
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
  int n = 5;
  for (int i=0;i<n;i++){
    //fprintf(stderr,"mean_time_per_path_now %d = mean_time_per_path_now %Lf/ counter_per_path_now %ld\n",i,mean_time_per_path_now()[i],counter_per_path_now()[i]);
    mean_time_per_path_now()[i] = mean_time_per_path_now()[i]/counter_per_path_now()[i];
  }
}

void set_mean_total_time_batch(){
  long double tot_counter = 0.0;
  for (int i=0;i<5;i++){
    tot_counter += stats[batch_active].counter_per_path[i];
  }
  //long double tot_counter = stats[batch_active].counter_generated - stats[batch_active].counter_exited
  batch_response_times[batch_active] = batch_response_times[batch_active]/tot_counter;
}

void set_mean_wasted_time_for_cloudlet(){
  //fprintf(stderr,"mean_time_wasted_in_cloudlet = mean_time_wasted_in_cloudlet_now %Lf/ counter_per_path_now %ld\n",mean_time_wasted_in_cloudlet_now(),counter_per_path_now()[4]);
  stats[batch_active].mean_time_wasted_in_cloudlet = (long double)mean_time_wasted_in_cloudlet_now()/counter_per_path_now()[4];
}

/*
    TIME FUNCTIONS
*/

double get_t(){
  return t_current;
}

void set_t(double time_now){
  t_current = time_now;
}

/*
    STATE FUNCTIONS
*/

void print_actual_state()
{
  fprintf(stdout, "state at %f: cloudlet (%d,%d), cloud (%d,%d), setup %d. Path completed: cloudlet(%ld,%ld),cloud(%ld,%ld),withSetup(%ld). Entered %ld, Served %ld\n",
    get_t(),state->cloudlet_1,state->cloudlet_2,state->cloud_1,state->cloud_2,state->setup_2, counter_per_path_now()[0],counter_per_path_now()[2],counter_per_path_now()[1],counter_per_path_now()[3],counter_per_path_now()[4],counter_generated_now(),counter_exited_now());
}

/*
  FILE EXPORT FUNCTIONS
*/
void save_response_time(long double time, int path, double wasted){
  fprintf(export_file, "%d;%d;%Lf;%f\n",batch_active,path, time,wasted);
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


/*
    EVENT FUNCTIONS
*/
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
  save_response_time(get_t()-event->arrival_time, event->path-3, event->wasted_time);
  //batch_active, save mean
  batch_response_times[batch_active] += (get_t()-event->arrival_time);

  if(DEBUG){printf("Event Destroyed: Exited packet with path %d at %f after %f\n", event->path, get_t(),get_t()-event->arrival_time);}
  free(event);
}

/*
    TRANSITIONS FUNCTIONS
*/

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
  struct Event *to_remove = remove_event_of_type(PREEMPTION_GOVERNOR,EVENT_COMPLETED_2_IN_1);
  generate_completion_event(to_remove, mu_setup_2, EVENT_COMPLETED_2_IN_SETUP, PATH_2_S_2);
  to_remove->wasted_time = get_t()-to_remove->arrival_time;
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
  if(argc != 8)
  {
    fprintf(stderr, "Usage: %s <N> <S> <end_time> <initial_seed> <# of batch intervals> <preemption governor 0 first,1 last,2 random> <debug 0 or 1>\n", argv[0]);
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
    batch_response_times = (long double *)calloc(sizeof(long double), batch_number_total);

    PREEMPTION_GOVERNOR = strtol(argv[6] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - preemption governor\n");
      return EXIT_FAILURE;
    }

    DEBUG = strtol(argv[7] , NULL, 10);
    if (errno != 0)
    {
      fprintf(stderr, "Error in conversion - debug must be 0 or 1\n");
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
}

void initialize_export_file(){
  char name[255];
  sprintf(name,"simN%d-S%d-batch_time_total%.1f-batch%d-seed%ld-gv%d.data", N,S,t_end,batch_number_total,initial_seed,PREEMPTION_GOVERNOR);


  export_file = fopen(name, "w");
  if(export_file == NULL){
    fprintf(stderr, "errore nella fopen\n");
    perror("initializing");
    exit(EXIT_FAILURE);
  }

  fprintf(export_file, "batch;path;response_time;wasted_time\n\n");
}

void close_export_file(){
  fprintf(export_file, "\n\nEND OF SIMULATION\n");

  fclose(export_file);
}

/*
    SUPPORT FUNCTIONS
*/

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

/*
    SIMULATION FUNCTIONS
*/

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

/*
    INITIALIZATION FUNCTIONS
*/

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
void initialize_tot_mean_time(){
  end_means = (struct Tot_Mean_Time *)calloc(sizeof(struct Tot_Mean_Time), 1);
}
void initialize_tot_mean_time_stds(){
  end_stds = (struct Tot_Std_Time *)calloc(sizeof(struct Tot_Std_Time), 1);
}
void initialize_tot_wasted_time(){
  w_times = (struct Tot_Wasted_Time*)calloc(sizeof(struct Tot_Wasted_Time), 1);
}
void initialize_wasted_stds(){
  w_stds = (struct Tot_Std_Wasted_Time*)calloc(sizeof(struct Tot_Std_Wasted_Time), 1);
}
void initialize_probabilities(){
  probs = (struct Probabilities *)calloc(sizeof(struct Probabilities), 1);
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
    MAIN
*/

int main(int argc, char ** argv)
{

  if(initialize_parameters(argc, argv) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  double t_star = idfStudent(batch_number_total-1, 1-alpha/2);

  initialize_batch_stats();
  initialize_tot_mean_time();
  initialize_tot_wasted_time();
  initialize_tot_mean_time_stds();
  initialize_wasted_stds();
  initialize_probabilities();
  initialize_generators(initial_seed);
  initialize_state();
  initialize_events();
  initialize_export_file();

  printf("Started simulation with N=%d, S=%d, batch_time_total=%f, batch#=%d, seed=%ld, governor=%d\n", N,S,t_end,batch_number_total,initial_seed,PREEMPTION_GOVERNOR);

  for (batch_active = 0; batch_active < batch_number_total; batch_active++){
    while(get_t() <= t_end*(batch_active+1) )
    {
      process_event(pop_event());
    }
    printf("\n\nBATCH %d finished\n", batch_active);
    print_actual_state();

    set_mean_time_per_path();
    set_mean_wasted_time_for_cloudlet();
    set_mean_total_time_batch();

    printf("Mean should be:\n%f - %f - %f - %f - ?\n",1/mu_cloudlet_1, 1/mu_cloud_1, 1/mu_cloudlet_2, 1/mu_cloud_2);
    printf("Mean for path: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%Lf - %Lf - %Lf - %Lf - %Lf. Mean of wasted time %Lf \n", mean_time_per_path_now()[0],mean_time_per_path_now()[1],mean_time_per_path_now()[2],mean_time_per_path_now()[3],mean_time_per_path_now()[4], mean_time_wasted_in_cloudlet_now());
    printf("P calculated: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%f - %f - %f - %f - %f\n", counter_per_path_now()[0]*1.0/counter_exited_now(),counter_per_path_now()[1]*1.0/counter_exited_now(),counter_per_path_now()[2]*1.0/counter_exited_now(),counter_per_path_now()[3]*1.0/counter_exited_now(),counter_per_path_now()[4]*1.0/counter_exited_now());
  }

  set_end_means(tot_mean_time_per_path(0), tot_mean_time_per_path(1), tot_mean_time_per_path(2), tot_mean_time_per_path(3), tot_mean_time_per_path(4));
  set_wasted_times(tot_mean_time_wasted_in_cloudlet());
  set_probabilities(tot_mean_counter_per_path(0)/tot_mean_counter_exited(), tot_mean_counter_per_path(1)/tot_mean_counter_exited(), tot_mean_counter_per_path(2)/tot_mean_counter_exited(), tot_mean_counter_per_path(3)/tot_mean_counter_exited(), tot_mean_counter_per_path(4)/tot_mean_counter_exited());
  set_end_stds(calculate_sd_time_per_path(0, end_means->first_clet), calculate_sd_time_per_path(1, end_means->second_clet), calculate_sd_time_per_path(2, end_means->first_cloud), calculate_sd_time_per_path(3, end_means->second_cloud), calculate_sd_time_per_path(4, end_means->setup_cloud));
  set_wasted_stds(calculate_sd_wasted_time(w_times->clet));

  //Just to see if they return the same result
  //long double general_path = calc_general_path_mean(end_means,probs);
  long double general_path = calculate_medium_total_response_time_2();

  long double ts[5];
  ts[0] = (t_star*end_stds->first_clet)/sqrt(batch_number_total-1);
  ts[1] = (t_star*end_stds->second_clet)/sqrt(batch_number_total-1);
  ts[2] = (t_star*end_stds->first_cloud)/sqrt(batch_number_total-1);
  ts[3] = (t_star*end_stds->second_cloud)/sqrt(batch_number_total-1);
  ts[4] = (t_star*end_stds->setup_cloud)/sqrt(batch_number_total-1);
  ts[5] = (t_star*w_stds->clet)/sqrt(batch_number_total-1);

  long double general_path_sd = calculate_sd_total_response_time(general_path);
  long double trr = (t_star*general_path_sd)/sqrt(batch_number_total-1);

  printf("End Simulation\n ");
  printf("\nEnded simulation with N=%d, S=%d, batch_time_total=%f, batch#=%d, seed=%ld, governor=%d\n", N,S,t_end,batch_number_total,initial_seed,PREEMPTION_GOVERNOR);
  printf("Total Mean should be:\n%f - %f - %f - %f - ?\n",1/mu_cloudlet_1, 1/mu_cloud_1, 1/mu_cloudlet_2, 1/mu_cloud_2);
  printf("Total Mean for path: 1_1, 1_2, 2_1, 2_2, 2_S_2, wasted, total\n%Lf - %Lf - %Lf - %Lf - %Lf - %Lf - %Lf\n", end_means->first_clet,end_means->second_clet,end_means->first_cloud,end_means->second_cloud,end_means->setup_cloud, w_times->clet, general_path);
  printf("Total P calculated: 1_1, 1_2, 2_1, 2_2, 2_S_2\n%f - %f - %f - %f - %f\n", probs->first_clet, probs->second_clet, probs->first_cloud, probs->second_cloud, probs->setup_cloud);
  printf("Effective throughput \n%Lf\n",tot_mean_counter_exited()/t_end);
  printf("Total Stds for path: 1_1, 1_2, 2_1, 2_2, 2_S_2, wasted, total\n%Lf - %Lf - %Lf - %Lf - %Lf - %Lf - %Lf\n",ts[0],ts[1],ts[2],ts[3],ts[4],ts[5],trr);
  printf("Intervals for path:\n1_1 %Lf +- %Lf\n1_2 %Lf +- %Lf\n2_1 %Lf +- %Lf\n2_2 %Lf +- %Lf\n2_S_2 %Lf +- %Lf\nwasted_time %Lf +- %Lf\n", end_means->first_clet, ts[0],end_means->second_clet, ts[1],end_means->first_cloud, ts[2],end_means->second_cloud,ts[3],end_means->setup_cloud, ts[4], w_times->clet, ts[5]);
  printf("Interval for total response time\n%Lf +- %Lf\n",general_path,trr);
  
  //printf("Intervals for path: 1_1, 1_2, 2_1, 2_2, 2_S_2, wasted, total\n%Lf - %Lf - %Lf - %Lf - %Lf - %Lf\n", end_stds->first_clet,end_stds->second_clet,end_stds->first_cloud,end_stds->second_cloud,end_stds->setup_cloud, w_stds->clet);
  //printf("Total Std for response time\n%Lf\n",trr);

  close_export_file();

  return EXIT_SUCCESS;
}
