#include "event.h"
#include <common/vector.h>
#include <GLFW/glfw3.h>

const uint32_t INITIAL_EVENT_SUBSCRIBERS_COUNT = 5;

struct EventManagerState
{
    EventSubscribeInfo *event_subscribers[EVENT_TYPE_TOTAL_EVENTS];
};
typedef struct EventManagerState EventManagerState;
static EventManagerState g_eventManagerState;

void em_initialize_event_manager()
{
    EventSubscribeInfo **event_subscribers = g_eventManagerState.event_subscribers;
    for (uint64_t i = 0; i < EVENT_TYPE_TOTAL_EVENTS; ++i)
    {
        event_subscribers[i] = (EventSubscribeInfo *)vector_create(sizeof(EventSubscribeInfo), INITIAL_EVENT_SUBSCRIBERS_COUNT);
    }
}

void em_destroy_event_manager()
{
    EventSubscribeInfo **event_subscribers = g_eventManagerState.event_subscribers;
    for (uint64_t i = 0; i < EVENT_TYPE_TOTAL_EVENTS; ++i)
    {
        vector_free(event_subscribers[i]);
    }
}

SubId em_subscribe_to_event(const EventSubscribeInfo *info)
{
    EventType type = info->event_type; 
}

void em_unsubscribe_to_event(SubId subscrition_id); // Add vector_remove_at function

void em_queue_event(const Event *event)
{
}

void em_update()
{
}
