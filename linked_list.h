
struct Event * push_event(struct Event *event);
struct Event * remove_event(struct Event *event);
struct Event * pop_event();
struct Event * remove_last_event_of_type(int type);
