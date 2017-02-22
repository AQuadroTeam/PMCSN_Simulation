
struct Event * push_event(struct Event *event);
struct Event * remove_event(struct Event *event);
struct Event * pop_event();
struct Event * remove_event_of_type(int governor, int type);
void print_events();
void print_events_inverse();
