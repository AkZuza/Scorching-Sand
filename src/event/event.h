#pragma once

#include <common/common.h>
#include <window/window.h>

enum EventType : uint64_t
{
    EVENT_TYPE_WINDOW_CLOSE,
    EVENT_TYPE_WINDOW_RESIZE,
    EVENT_TYPE_WINDOW_MOVE,
    EVENT_TYPE_MOUSE_PRESS,
    EVENT_TYPE_MOUSE_SCROLL,
    EVENT_TYPE_MOUSE_RELEASE,
    EVENT_TYPE_MOUSE_MOVE,
    EVENT_TYPE_KEY_PRESS,
    EVENT_TYPE_KEY_RELEASE,
    EVENT_TYPE_TOTAL_EVENTS
};

typedef enum EventType EventType;
typedef void (*OnEventCallback)(EventType, void*);
typedef void (*OnSubscribedEventCallback)(EventType, void*, void*);
typedef uint32_t SubId;

struct EventManagerInitInfo 
{
    Window window;
    OnEventCallback on_event_callback;
};

struct EventSubscribeInfo 
{
    EventType event_type;
    OnSubscribedEventCallback on_event_callback;
    void *data;
};

void em_initialize_event_manager(struct EventManagerInitInfo *info);
SubId em_subscribe_to_event(struct EventSubscribeInfo *info);
void em_unsubscribe_to_event(uint32_t subscrition_id);
void em_fire_event(EventType, void *data);
void em_poll_events();
