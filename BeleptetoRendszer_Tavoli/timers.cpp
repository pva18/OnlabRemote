#include "timers.h"

#include "CircularBuffer.hpp"

#define TIMER_MAX_EVENT_NUMBER 16

CircularBuffer<timer_event_t, TIMER_MAX_EVENT_NUMBER> timer_events;

int TIMERS_GetMaxEventNumber(void)
{
    return TIMER_MAX_EVENT_NUMBER;
}

bool TIMERS_AddEvent(const timer_event_t *event)
{
    return timer_events.enqueue(*event);
}

void TIMERS_HandleEvents(unsigned long millis_current)
{
    timer_event_t event;
    while (timer_events.dequeue(&event))
    {
        unsigned long millis_real_period = millis_current - event.millis_start;
        if (millis_real_period >= event.millis_period)
        {
            event.handler(millis_real_period);
        }
        else
        {
            timer_events.enqueue(event);
        }
    }
}
