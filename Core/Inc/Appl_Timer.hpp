#ifndef APPL_TIMER_HPP
#define APPL_TIMER_HPP

typedef struct
{
  uint32_t u32PrevMills;
  uint32_t u32CurrMills;
  uint32_t u32TargetMills;
  bool bEnable;
}stcTimer;

typedef enum
{
  enTimerNormalStop = 0,
  enTimerAutoRestart,
}TimerMode;

void InitilizeTimer(stcTimer *pTimer);
void StartTimer(stcTimer *pTimer , uint32_t DelayTimeMs);
bool Timer_IsTimeout(stcTimer *pTimer , TimerMode m_TimerMode);
void StopTimer(stcTimer *pTimer);
bool Timer_IsRunning(stcTimer *pTimer);

#endif
