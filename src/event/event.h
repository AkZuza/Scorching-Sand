#pragma once

#include <common/common.h>
#include <window/window.h>
#include <event/window_event.h>

enum EventType : uint32_t
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
typedef struct 
{
    uint32_t event_type;
    uint32_t idx;
} SubId;

struct Event
{
   EventType type;
   union
   {
       WindowResizeData window_size;
       WindowMoveData window_pos;
   } data;
};
typedef struct Event Event;

typedef void (*OnEventCallback)(const Event *event, void *user_data);

struct EventSubscribeInfo 
{
    EventType event_type;
    OnEventCallback on_event_callback;
    void *user_data;
};
typedef struct EventSubscribeInfo EventSubscribeInfo;

void em_initialize_event_manager();
void em_destroy_event_manager();
SubId em_subscribe_to_event(const EventSubscribeInfo *info);
void em_unsubscribe_to_event(SubId subscrition_id);
void em_queue_event(const Event *event);
void em_update();
