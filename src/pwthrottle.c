/*=======================================================================
 *	Throttle data sent to maximum bit rate
 *=======================================================================*/
#include <pwutil.h>

#if defined(CLOCK_MONOTONIC_RAW)
#define MY_CLOCK CLOCK_MONOTONIC_RAW
#elif defined(CLOCK_MONOTONIC)
#define MY_CLOCK CLOCK_MONOTONIC
#else
#define MY_CLOCK CLOCK_REALTIME
#endif

#define TIM_SECS(t) ((t).tv_sec + (t).tv_nsec * 1e-9)
#define TIM_FROM_SECS(t, s) do {(t).tv_sec = (int)(s); (t).tv_nsec = (int)(((s) - (t).tv_sec) * 1000000000);} while (0)
#define TIM_ZERO(t) do {(t).tv_sec = 0; (t).tv_nsec = 0;} while (0)
#define TIM_SUB(a,b) do {if ((a).tv_nsec < (b).tv_nsec) {(a).tv_sec -= 1; (a).tv_nsec += 1000000000;} (a).tv_sec -= (b).tv_sec; (a).tv_nsec -= (b).tv_nsec;} while (0)
#define TIM_ADD(a,b) do {(a).tv_sec += (b).tv_sec; (a).tv_nsec += (b).tv_nsec; if ((a).tv_nsec >= 1000000000) {(a).tv_sec += 1; (a).tv_nsec -= 1000000000;}} while (0)
#define TIM_LT(a,b) ((a).tv_sec < (b).tv_sec || ((a).tv_sec == (b).tv_sec && (a).tv_nsec < (b).tv_nsec))


struct _PwThrottle {
  /* config */
  double buffer_size;		/* Capacity of output buffer in bytes */
  double rate;			/* Rate at which buffer drains */
  /* state */
  struct timespec empty_time;	/* When buffer will be empty */
};

PwThrottle *
pwthrottle_create(double buffer_size, double rate)
{
  PwThrottle *self = g_new0(PwThrottle, 1);
  self->buffer_size = buffer_size;
  self->rate = rate;
  TIM_ZERO(self->empty_time);
  return self;
}

/*-----------------------------------------------------------------------
 *	Calculate if we need to wait before sending.
 *	If OK to send, return 0 and adjust for amount presumed to be sent.
 *	If not, return 1 and set *wait to time to hold off for.
 *-----------------------------------------------------------------------*/
int
pwthrottle_check(PwThrottle *self, size_t nbytes, struct timespec *wait)
{
  struct timespec now, till_empty;
  double available;

  available = self->buffer_size;
  clock_gettime(MY_CLOCK, &now);
  if (TIM_LT(now, self->empty_time)) {
    /* Not empty yet - see how long until it is */
    till_empty = self->empty_time;
    TIM_SUB(till_empty, now);
    available -= TIM_SECS(till_empty) * self->rate;
  } else {
    TIM_ZERO(till_empty);
  }
  if (nbytes <= available) {
    /* Data will fit in bandwidth.  Calculate new empty time. */
    TIM_FROM_SECS(self->empty_time, nbytes / self->rate);
    TIM_ADD(self->empty_time, till_empty);
    TIM_ADD(self->empty_time, now);
    return 0;
  } else {
    /* Note how long to wait until clear to send */
    double secs = (nbytes - available) / self->rate;
    TIM_FROM_SECS(*wait, secs);
    return 1;
  }
}
