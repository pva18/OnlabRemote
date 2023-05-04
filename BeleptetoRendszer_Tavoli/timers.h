#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

typedef void timer_event_handler_t(uint32_t millis_real_period);

typedef struct _timer_event
{
    unsigned long millis_start;
    unsigned long millis_period;
    timer_event_handler_t *handler;
} timer_event_t;

int TIMERS_GetMaxEventNumber(void);

bool TIMERS_AddEvent(const timer_event_t *event);

void TIMERS_HandleEvents(unsigned long millis_current);

#endif /* TIMERS_H */
