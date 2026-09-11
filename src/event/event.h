#pragma once

#include <common/common.h>
#include <window/window.h>

typedef void (*OnEventCallback)(uint32_t, void*);
typedef void (*OnSubscribedEventCallback)(uint32_t, void*, void*);

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
uint32_t subscribe_to_event(struct EventSubscribeInfo *info);
void unsubscribe_to_event(uint32_t subscrition_id);
