#include "event.h"
#include <common/vector.h>
#include <common/queue.h>
#include <GLFW/glfw3.h>

const uint32_t INITIAL_EVENT_SUBSCRIBERS_COUNT = 5;

struct Subscriber
{
    OnEventCallback on_event_callback;
    void *user_data;
};
typedef struct Subscriber Subscriber;

struct EventManagerState
{
    Subscriber *event_subscribers[EVENT_TYPE_TOTAL_EVENTS];
    Queue event_queue;
};
typedef struct EventManagerState EventManagerState;
static EventManagerState g_eventManagerState;

void em_initialize_event_manager()
{
    for (uint64_t i = 0; i < EVENT_TYPE_TOTAL_EVENTS; ++i)
    {
        g_eventManagerState.event_subscribers[i] = (Subscriber *)vector_create(sizeof(Subscriber), INITIAL_EVENT_SUBSCRIBERS_COUNT);
    }
    queue_create(&g_eventManagerState.event_queue, sizeof(Event), 20);
}

void em_destroy_event_manager()
{
    Subscriber **event_subscribers = g_eventManagerState.event_subscribers;
    for (uint64_t i = 0; i < EVENT_TYPE_TOTAL_EVENTS; ++i)
    {
        vector_free(event_subscribers[i]);
    }
    queue_free(&g_eventManagerState.event_queue);
}

SubId em_subscribe_to_event(const EventSubscribeInfo *info)
{
    Subscriber **event_subscribers = g_eventManagerState.event_subscribers;
    EventType type = info->event_type; 

    Subscriber sub = {
        .on_event_callback = info->on_event_callback,
        .user_data = info->user_data
    };
    event_subscribers[type] = vector_append(event_subscribers[type], &sub);
    
    SubId id = {
        .type = type,
        .idx = vector_count(event_subscribers[type])
    };

    return id;
}

void em_unsubscribe_to_event(SubId subscrition_id); // Add vector_remove_at function

void em_queue_event(const Event *event)
{
    Queue *queue = &g_eventManagerState.event_queue;
    queue_enqueue(queue, (void *)event);
}

void em_update()
{
    Queue *queue = &g_eventManagerState.event_queue;
    Subscriber **event_subscribers = g_eventManagerState.event_subscribers;
    while(queue_count(queue) != 0)
    {
        Event *event = (Event *)queue_front(queue);
        queue_dequeue(queue);
        
        Subscriber *subscribers = event_subscribers[event->type];
        uint64_t count = vector_count(subscribers);
        for (uint64_t i = 0; i < count; ++i)
        {
            Subscriber sub = subscribers[i];
            OnEventCallback callback = sub.on_event_callback;
            void *user_data = sub.user_data;

            callback(event, user_data);
        }
    }
}
