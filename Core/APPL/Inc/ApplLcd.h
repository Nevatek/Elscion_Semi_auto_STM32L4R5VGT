/*
 * ApplLcd.h
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */

#ifndef INC_APPLLCD_H_
#define INC_APPLLCD_H_
#if(LCD_TYPE == LCD_RAYSTAR)
typedef enum
{
 en_IconLayer,
 en_BackgroundLayer,
 en_EraseIconLayer,
 en_DisplayLayer
}enDisplayLayer;

typedef enum
{
	en_DispZeroDegree = 0,
	en_Disp180Degree
}enDisplayOrientation;

typedef enum
{
	en_Black = 0x00000000,
	en_Red,
	en_Green,
	en_Blue,
	en_White = 0x00FFFFFF,
}enDispColour;

typedef enum
{
	en_GeometryLine,
	en_GeometrySqaure
}enGeometry;

typedef enum
{
	en_EraseScreenArea,
	en_EraseIconLayerArea
}enEraseScreen;

typedef enum
{
	en_LCD_Responsefailed,
	en_LCDTouchcordinates,
	en_LCDAcknolodgement,
	en_LCDNack
}enLCDResponse;

typedef enum
{
	en_TextBackgndTransperant = 0,
	en_TextBackGndNonTransperant
}enTextBackgndOpctacity;
typedef enum
{
	en_NoTouch,
	en_PenDown,
	en_PenUp
}enTouchStatus;
#endif
#endif /* INC_APPLLCD_H_ */
