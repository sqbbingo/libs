#ifndef _twtimer_h_
#define _twtimer_h_

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
	TIMER_ONESHOT,
	TIMER_CONTINUS,
} TWTIMER_TYPE_E;

typedef struct twtimer_s {
	uint64_t        expire; // expire time (ms)
    TWTIMER_TYPE_E  type;
	struct          twtimer_s* next;
	struct          twtimer_s** pprev;
	void (*ontimeout)(void* param);
	void* param;
} twtimer_t;

typedef struct time_wheel_s tw_handle_t;
typedef struct time_wheel_s time_wheel_t;
time_wheel_t *time_wheel_create(uint64_t clock);
int time_wheel_destroy(time_wheel_t *tm);
/// @return sleep time(ms)
int time_wheel_process(time_wheel_t *tm, uint64_t clock);
/// one-shoot timeout timer
/// @return 0-ok, other-error
int time_wheel_start(time_wheel_t *tm, twtimer_t *timer);
/// @return  0-ok, other-timer can't be stop(timer have triggered or will be triggered)
int time_wheel_stop(time_wheel_t *tm, twtimer_t *timer);


/**** Another set of API : When there's only one user ****/
/// @return current system time (ms)
uint64_t twtimer_get_systime(void);
int twtimer_msleep(uint64_t ms);
/// @note timer->expire = ms, not sysclock+ms
tw_handle_t *twtimer_start(twtimer_t *timer);
int twtimer_stop(tw_handle_t *twheel);


#if defined(__cplusplus)
}
#endif
#endif /* !_twtimer_h_ */
