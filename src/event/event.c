#include "event.h"
#include <common/vector.h>
#include <GLFW/glfw3.h>

struct EventManagerState
{
};
typedef struct EventManagerState EventManagerState;

static EventManagerState g_eventManagerState;

void em_initialize_event_manager(struct EventManagerInitInfo *info)
{
    // Register all the necessary callbacks
}

SubId em_subscribe_to_event(struct EventSubscribeInfo *info)
{
}

void em_unsubscribe_to_event(uint32_t subscrition_id)
{
}

void em_fire_event(EventType, void *data)
{
}

void em_poll_events()
{
}
