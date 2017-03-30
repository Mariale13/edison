#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

static int signal_recv_count;

void sigalrm_handler(int signum)
{
  printf("SIGALRM received, count :%d\n", (int)signal_recv_count);
  signal_recv_count++;
}

int main()
{
  struct itimerval timer={0};
  char a[200];
  /* Initial timeout value */
  timer.it_value.tv_sec = 1;

  /* We want a repetitive timer */
  timer.it_interval.tv_sec = 1;

  /* Register Signal handler
   * And register for periodic timer with Kernel*/
  signal(SIGALRM, &sigalrm_handler);
  setitimer(ITIMER_REAL, &timer, NULL);

  read(2, &a, 199);
}
