/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"

std::string g_NumericKeyBoardBuffer;
char arr_Buffer_old[64] = {0};
extern bool DateEntry;

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bKeyBack = NexButton(en_WinID_NumericKeypad , 16 , "b12");
static NexButton bKey0 = NexButton(en_WinID_NumericKeypad , 18 , "b11");
static NexButton bKey1 = NexButton(en_WinID_NumericKeypad , 5 , "b0");
static NexButton bKey2 = NexButton(en_WinID_NumericKeypad , 6 , "b1");
static NexButton bKey3 = NexButton(en_WinID_NumericKeypad , 7 , "b2");
static NexButton bKey4 = NexButton(en_WinID_NumericKeypad , 8 , "b3");
static NexButton bKey5 = NexButton(en_WinID_NumericKeypad , 9 , "b4");
static NexButton bKey6 = NexButton(en_WinID_NumericKeypad , 10 , "b5");
static NexButton bKey7 = NexButton(en_WinID_NumericKeypad , 11 , "b6");
static NexButton bKey8 = NexButton(en_WinID_NumericKeypad , 12 , "b7");
static NexButton bKey9 = NexButton(en_WinID_NumericKeypad , 13 , "b8");
static NexButton bKeyDot = NexButton(en_WinID_NumericKeypad , 14 , "b9");
static NexButton bKeyDash = NexButton(en_WinID_NumericKeypad , 19 , "b13");
static NexButton bKeySave = NexButton(en_WinID_NumericKeypad , 17 , "b41");
static NexButton bKeyBackSpace = NexButton(en_WinID_NumericKeypad , 15 , "b10");

static NexText tKeyboardBufferText = NexText(en_WinID_NumericKeypad , 4 , "t1");
static NexText tKeyboardNameBuffer = NexText(en_WinID_NumericKeypad , 3 , "t0");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bKeyBack,
									 &bKeyBackSpace,
									 &bKeyDot,
									 &bKeySave,
									 &bKeyDash,
									 &bKey0,
									 &bKey1,
									 &bKey2,
									 &bKey3,
									 &bKey4,
									 &bKey5,
									 &bKey6,
									 &bKey7,
									 &bKey8,
									 &bKey9,
							  	  	 NULL};

static void HandlerButtonBack(void *ptr);
static void HandlerButtonBackSpace(void *ptr);
static void HandlerButtonK0(void *ptr);
static void HandlerButtonK1(void *ptr);
static void HandlerButtonK2(void *ptr);
static void HandlerButtonK3(void *ptr);
static void HandlerButtonK4(void *ptr);
static void HandlerButtonK5(void *ptr);
static void HandlerButtonK6(void *ptr);
static void HandlerButtonK7(void *ptr);
static void HandlerButtonK8(void *ptr);
static void HandlerButtonK9(void *ptr);
static void HandlerButtonKDot(void *ptr);
static void HandlerButtonKDash(void *ptr);
static void HandlerButtonSave(void *ptr);
void InsertCharAt(std::string& str, char ch, size_t pos);
void AutoUpdateDateTimeBuffer();

enWindowStatus ShowWNumericKeypadScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bKeyBack.attachPush(HandlerButtonBack, &bKeyBack);
	bKeyBackSpace.attachPush(HandlerButtonBackSpace, &bKeyBackSpace);
	bKey0.attachPush(HandlerButtonK0, &bKey0);
	bKey1.attachPush(HandlerButtonK1, &bKey1);
	bKey2.attachPush(HandlerButtonK2, &bKey2);
	bKey3.attachPush(HandlerButtonK3, &bKey3);
	bKey4.attachPush(HandlerButtonK4, &bKey4);
	bKey5.attachPush(HandlerButtonK5, &bKey5);
	bKey6.attachPush(HandlerButtonK6, &bKey6);
	bKey7.attachPush(HandlerButtonK7, &bKey7);
	bKey8.attachPush(HandlerButtonK8, &bKey8);
	bKey9.attachPush(HandlerButtonK9, &bKey9);
	bKeyDot.attachPush(HandlerButtonKDot, &bKeyDot);
	bKeySave.attachPush(HandlerButtonSave, &bKeySave);
	bKeyDash.attachPush(HandlerButtonKDash, &bKeyDash);

	char arr_Buffer[64] = {0};

//	if(false == stcScreenNavigation.NumberFloatEnable)
//	{
//		int32_t nVal = 0;
////		uint32_t nVal = *stcScreenNavigation.ptrInterscreenDataBuffer;
//		memcpy(&nVal , stcScreenNavigation.ptrInterscreenDataBuffer , stcScreenNavigation.SizeOfBuffer);
////		snprintf(arr_Buffer , 61 , "%d" , (int)nVal);
//	    if (nVal != 0) {
//	    	snprintf(arr_Buffer , 61 , "%d" , (int)nVal);
//	    } else {
//	        arr_Buffer[0] = '\0'; // empty string for blank cell
//	    }
//	}
//	else
//	{
//		float fVal = 0;//= *stcScreenNavigation.ptrInterscreenDataFloatBuffer;
//		memcpy(&fVal , stcScreenNavigation.ptrInterscreenDataFloatBuffer , stcScreenNavigation.SizeOfBuffer);
//	    if (fVal != 0) {
//	        snprintf(arr_Buffer, sizeof(arr_Buffer), "%.3f", fVal);
//	    } else {
//	        arr_Buffer[0] = '\0'; // empty string for blank cell
//	    }
//
//	}


	if(false == stcScreenNavigation.NumberFloatEnable)
	{
		int32_t nVal = 0;
		memcpy(&nVal , stcScreenNavigation.ptrInterscreenDataBuffer , stcScreenNavigation.SizeOfBuffer);
	    snprintf(arr_Buffer_old , 61 , "%d" , (int)nVal);
	    arr_Buffer[0] = '\0'; // empty string for blank cell
	}
	else
	{
		float fVal = 0;//= *stcScreenNavigation.ptrInterscreenDataFloatBuffer;
		memcpy(&fVal , stcScreenNavigation.ptrInterscreenDataFloatBuffer , stcScreenNavigation.SizeOfBuffer);
	    snprintf(arr_Buffer_old, sizeof(arr_Buffer), "%.3f", fVal);
	    arr_Buffer[0] = '\0'; // empty string for blank cell
	}
	g_NumericKeyBoardBuffer = std::string(arr_Buffer);
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	tKeyboardNameBuffer.setText(stcScreenNavigation.strKeyboardname.c_str());
	return WinStatus;
}

void HandlerNumericKeypadScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void AutoUpdateDateTimeBuffer()
{
    size_t length = g_NumericKeyBoardBuffer.length();
    if(DateEntry)
    {
        if (length == 2 || length == 5 )
        {
        	g_NumericKeyBoardBuffer.push_back('/');
            tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
        }
    }
}
static void HandlerButtonKDash(void *ptr)
{
//	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length() ||
//			(false == stcScreenNavigation.NumberFloatEnable) || (true == DateEntry))
	if(0 != g_NumericKeyBoardBuffer.length() || stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length() || (true == DateEntry))
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('-');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	BeepBuzzer();
}
static void HandlerButtonKDot(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length() ||
			(false == stcScreenNavigation.NumberFloatEnable) || (true == DateEntry))
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('.');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	BeepBuzzer();
}
static void HandlerButtonSave(void *ptr)
{
	float DataFloat = (float)0;
	uint32_t DataInt = 0U;
	if(stcScreenNavigation.NumberFloatEnable)
	{
		DataFloat = (float)atof(g_NumericKeyBoardBuffer.c_str());
		memcpy(stcScreenNavigation.ptrInterscreenDataFloatBuffer , &DataFloat , sizeof(float));
	}
	else if(sizeof(uint8_t) == stcScreenNavigation.SizeOfBuffer)
	{
		DataInt = (uint32_t)atoi(g_NumericKeyBoardBuffer.c_str());
		memcpy(stcScreenNavigation.ptrInterscreenDataBuffer , &DataInt , sizeof(uint8_t));
	}
	else if(sizeof(int16_t) == stcScreenNavigation.SizeOfBuffer)
	{
		DataInt = (uint32_t)atoi(g_NumericKeyBoardBuffer.c_str());
		memcpy(stcScreenNavigation.ptrInterscreenDataBuffer , &DataInt , sizeof(uint16_t));
	}
	else if(sizeof(int32_t) == stcScreenNavigation.SizeOfBuffer)
	{
		DataInt = (uint32_t)atoi(g_NumericKeyBoardBuffer.c_str());
		memcpy(stcScreenNavigation.ptrInterscreenDataBuffer , &DataInt , sizeof(uint32_t));
	}
	else if(DateEntry == true)
	{
		strncpy((char*)stcScreenNavigation.ptr_KeyBoardBuffer,
				g_NumericKeyBoardBuffer.c_str(),stcScreenNavigation.MaxKeyboardCharLength);
	}
    DateEntry = false;
	ChangeWindowPage(stcScreenNavigation.PrevWindowId , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerButtonBack(void *ptr)
{
	g_NumericKeyBoardBuffer = std::string(arr_Buffer_old);
    DateEntry = false;
	ChangeWindowPage(stcScreenNavigation.PrevWindowId , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerButtonBackSpace(void *ptr)
{
	 size_t length = g_NumericKeyBoardBuffer.length();
	if(0 >= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	if(DateEntry == true &&(length == 3 || length == 6))
	{
		g_NumericKeyBoardBuffer.pop_back();
	}
	g_NumericKeyBoardBuffer.pop_back();
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	BeepBuzzer();
}

void HandlerButtonK0(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('0');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK1(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('1');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK2(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('2');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK3(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('3');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK4(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('4');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK5(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('5');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK6(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('6');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK7(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('7');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK8(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('8');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}

void HandlerButtonK9(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_NumericKeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_NumericKeyBoardBuffer.push_back('9');
	tKeyboardBufferText.setText(g_NumericKeyBoardBuffer.c_str());
	AutoUpdateDateTimeBuffer();
	BeepBuzzer();
}
void InsertCharAt(std::string& str, char ch, size_t pos)
{
    if (pos <= str.length())
    {
        str.insert(pos, 1, ch);
    }
}
