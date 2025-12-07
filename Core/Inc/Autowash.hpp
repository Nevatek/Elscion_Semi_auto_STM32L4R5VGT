/*
 * Autowash.hpp
 *
 *  Created on: Jan 8, 2024
 *      Author: Alvin
 */

#ifndef INC_AUTOWASH_HPP_
#define INC_AUTOWASH_HPP_

typedef enum
{
	enAutoWashDoorClosed = 0,
	enAutoWashDoorOpen,
}en_AutoWashDoorStatus;

bool Read_AutowashPin(void);
bool Read_FluidicSensorPin(void);

#endif /* INC_AUTOWASH_HPP_ */
