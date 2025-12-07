#include "stdlib.h"
#include "main.h"
#include "system.hpp"
#include "cmsis_os.h"
#include "Appl_Timer.hpp"

void InitilizeTimer(stcTimer *pTimer)
{
  pTimer->u32PrevMills = 0u;
  pTimer->u32CurrMills = 0u;
  pTimer->u32TargetMills = 0u;
  pTimer->bEnable = false;
}
void StartTimer(stcTimer *pTimer , uint32_t DelayTimeMs)
{
  if(false == Timer_IsRunning(pTimer))
  {
    InitilizeTimer(pTimer);
    pTimer->u32TargetMills = DelayTimeMs;
    pTimer->u32PrevMills = HAL_GetTick();
    pTimer->bEnable = true;
  }
}

bool Timer_IsTimeout(stcTimer *pTimer , TimerMode m_TimerMode)
{
  bool bStatus = false;
  if(true == Timer_IsRunning(pTimer))
  {
    pTimer->u32CurrMills = HAL_GetTick();
    int32_t elapsedTime = (int32_t)(pTimer->u32CurrMills - pTimer->u32PrevMills);
    if(abs(elapsedTime) >= pTimer->u32TargetMills)
    {
      bStatus = true;
      if(enTimerAutoRestart == m_TimerMode)
      {
        uint32_t u32TargetMillis = pTimer->u32TargetMills;
        StartTimer(pTimer , u32TargetMillis);
      }
      else/*Normal mode of timer without auto restart*/
      {
        StopTimer(pTimer);
      }
    }
  }
  return bStatus;
}

void StopTimer(stcTimer *pTimer)
{
  InitilizeTimer(pTimer);
}

bool Timer_IsRunning(stcTimer *pTimer)
{
  return pTimer->bEnable;
}
