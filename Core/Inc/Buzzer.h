/*
 * Buzzer.h
 *
 *  Created on: May 18, 2022
 *      Author: Alvin
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

void BeepBuzzer(void);
void IRQ_BuzzerTimerExpiry(void);
void LongBeepBuzzer(void);
void InstrumentBusyBuzz(void);
void TestTimerCompleteBuzzer(void);
void MemoryCorruptionBuzzer(void);
#endif /* INC_BUZZER_H_ */
