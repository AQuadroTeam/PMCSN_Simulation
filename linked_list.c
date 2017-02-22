#include <stdlib.h>
#include <stdio.h>
#define GOVERNOR_FIRST  0
#define GOVERNOR_LAST  1
#define GOVERNOR_RND  2

void print_events();

struct Event {
  double time;
  int type;
  struct Event * next;
  struct Event * prev;
  double arrival_time;
  int path;
};

struct Event * events = NULL;
struct Event * last = NULL;

struct Event * push_event(struct Event *event){
  int added = 0;

  if(events != NULL)
  {
    if(event->time < events->time) // check the first element
    {
      event->next = events;
      event->next->prev = event;
      event->prev = NULL;
      events = event;
      added = 1;
    }

    struct Event * prev = events;
    struct Event * actual = events->next;
    while(actual != NULL && added == 0) // check from second to last one
    {
      if(event->time < actual->time)
      {
          prev->next = event;
          event->prev = prev;
          event->next = actual;
          event->next->prev = event;
          added = 1;
      }
      prev = prev->next;
      actual = actual->next;
    }

    if(added == 0)  // append to list after last one
    {
      prev->next = event;
      event->next = NULL;
      event->prev = prev;
      last = event;
      added = 1;
    }
  }

  else
  {
    events = event;
    events->next = NULL;
    events->prev = NULL;
    last = event;
  }
  return event;
}


struct Event * remove_event(struct Event *event){
  if(event == NULL){
    return NULL;
  }
  struct Event * after = event->next;
  struct Event * before = event->prev;

  if(before != NULL){
    before->next = before->next->next;
  }
  else{
    events = after;
  }

  if(after != NULL){
    after->prev = after->prev->prev;
  }
  else{
    last = before;
  }
  event->next = NULL;
  event->prev = NULL;
  return event;
}

struct Event * pop_event()
{
  if(events == NULL){
    return NULL;
  }
  if(events->next == NULL){
    struct Event * event = events;
    events = NULL;
    last = NULL;
    return event;
  }

  struct Event * first = events;
  events = events->next;
  events->prev = NULL;

  first->prev = NULL;
  first->next = NULL;


  return first;
}

struct Event * remove_last_event_of_type(int type)
{
  if(events != NULL){
    struct Event * actual = last;

    while(actual != NULL){
      if(actual->type == type){
        return remove_event(actual);
      }
      actual = actual->prev;
    }
    fprintf(stderr, "Warning, event type %d not found\n", type);
    return NULL;
  }

  else{
    return NULL;
  }
}

struct Event * remove_first_event_of_type(int type)
{
  if(events != NULL){
    struct Event * actual = events;

    while(actual != NULL){
      if(actual->type == type){
        return remove_event(actual);
      }
      actual = actual->next;
    }
    fprintf(stderr, "Warning, event type %d not found\n", type);
    return NULL;
  }

  else{
    return NULL;
  }
}

struct Event * remove_a_event_of_type(int type)
{
  fprintf(stderr, "TO IMPLEMENT\n");
  return NULL;
}

struct Event * remove_event_of_type(int gov, int type)
{
  switch (gov) {
    case GOVERNOR_FIRST:
      return remove_first_event_of_type(type);
      break;
    case GOVERNOR_LAST:
      return remove_last_event_of_type(type);
      break;
    case GOVERNOR_RND:
      return remove_a_event_of_type(type);
      break;
  }
  return NULL;
}



void print_events()
{
  if(events != NULL){
    struct Event * actual = events;
    while(actual != NULL){
      printf(" %f:%d,",actual->time,actual->type);
      actual = actual->next;
    }
    printf("\n");
  }
}

void print_events_inverse(){
  if(events != NULL){
    struct Event * actual = last;

    while(actual != NULL){
      printf(" %f,",actual->time);
      actual = actual->prev;
    }
    printf("\n");
  }
}
