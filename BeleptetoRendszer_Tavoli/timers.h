#ifndef TIMERS_H
#define TIMERS_H

typedef void timer_event_handler_t(uint32_t millis_real_period);

typedef struct _timer_event
{
    uint32_t millis_start;
    uint32_t millis_period;
    timer_event_handler_t *handler;
} timer_event_t;

int TIMERS_GetMaxEventNumber(void);

bool TIMERS_AddEvent(const timer_event_t *event);

void TIMERS_HandleEvents(void);

#endif /* TIMERS_H */
