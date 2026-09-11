#pragma once

#include <common/common.h>
#include <window/window.h>

enum Event 
{
    EVENT_WINDOW_CLOSE,
    EVENT_WINDOW_RESIZE,
    EVENT_WINDOW_MOVE,
    EVENT_MOUSE_PRESS,
    EVENT_MOUSE_SCROLL,
    EVENT_MOUSE_RELEASE,
    EVENT_MOUSE_MOVE,
    EVENT_KEY_PRESS,
    EVENT_KEY_RELEASE
};

typedef enum Event Event;
typedef void (*OnEventCallback)(Event, void*);
typedef void (*OnSubscribedEventCallback)(Event, void*, void*);
ypedef uint32 SubId;

struct EventManagerInitInfo 
{
    Window window;
    OnEventCallback on_event_callback;
};

struct EventSubscribeInfo 
{
    uint32_t event;
    OnSubscribedEventCallback on_event_callback;
    void *data;
};

void initialize_event_manager(struct EventManagerInitInfo *info);
SubId subscribe_to_event(struct EventSubscribeInfo *info);
void unsubscribe_to_event(uint32_t subscrition_id);
