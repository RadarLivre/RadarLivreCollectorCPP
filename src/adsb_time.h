#ifndef ADSB_TIME_H
#define ADSB_TIME_H

#include <time.h>
#include <signal.h>

/*==============================================
This function is responsible of deal with time
operations.
================================================*/
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define TIMEOUT "60000000000" //nanoseconds
//This says that the messagws can't be older than 10 seconds
#define LIMIT_DIFF_TIME 10

#define TIMER_ERROR -1
#define TIMER_OK    0

double getCurrentTime();
long long int getCurrentTimeMilli();
char* getFormatedTime();
int TIMER_setSignalHandler(void (*handler)(int, siginfo_t*, void*), int timer_signal);
timer_t TIMER_create(int clockid, int timer_signal);
int TIMER_setTimeout(char *timeout_nanosec, timer_t timerid);

#endif