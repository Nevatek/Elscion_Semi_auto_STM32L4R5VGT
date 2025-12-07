/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"

static std::string g_KeyBoardBuffer;
bool g_CapsLock = false;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bKeyBack = NexButton(en_WinID_AlphaKeypad , 3 , "b12");
static NexButton bKeyBackSpace = NexButton(en_WinID_AlphaKeypad , 26 , "b22");
static NexButton bKeyEnter = NexButton(en_WinID_AlphaKeypad , 43 , "b40");
static NexButton bKeyCapsLock = NexButton(en_WinID_AlphaKeypad , 44 , "b38");
static NexButton bKeyCapsLock1 = NexButton(en_WinID_AlphaKeypad , 48 , "b43");
static NexButton bKeySpaceBar = NexButton(en_WinID_AlphaKeypad , 45 , "b39");
static NexButton bKeyDash = NexButton(en_WinID_AlphaKeypad , 46 , "b41");
static NexButton bKeyDot = NexButton(en_WinID_AlphaKeypad , 47 , "b42");
static NexButton bKeyslash = NexButton(en_WinID_AlphaKeypad , 49 , "b44");

static NexButton bKey1 = NexButton(en_WinID_AlphaKeypad , 6 , "b0");
static NexButton bKey2 = NexButton(en_WinID_AlphaKeypad , 7 , "b1");
static NexButton bKey3 = NexButton(en_WinID_AlphaKeypad , 8 , "b2");
static NexButton bKey4 = NexButton(en_WinID_AlphaKeypad , 9 , "b3");
static NexButton bKey5 = NexButton(en_WinID_AlphaKeypad , 10 , "b4");
static NexButton bKey6 = NexButton(en_WinID_AlphaKeypad , 11 , "b5");
static NexButton bKey7 = NexButton(en_WinID_AlphaKeypad , 12 , "b6");
static NexButton bKey8 = NexButton(en_WinID_AlphaKeypad , 13 , "b7");
static NexButton bKey9 = NexButton(en_WinID_AlphaKeypad , 14 , "b8");
static NexButton bKey0 = NexButton(en_WinID_AlphaKeypad , 15 , "b9");

static NexButton bKeyQ = NexButton(en_WinID_AlphaKeypad , 16 , "b10");
static NexButton bKeyW = NexButton(en_WinID_AlphaKeypad , 17 , "b11");
static NexButton bKeyE = NexButton(en_WinID_AlphaKeypad , 18 , "b13");
static NexButton bKeyR = NexButton(en_WinID_AlphaKeypad , 19 , "b14");
static NexButton bKeyT = NexButton(en_WinID_AlphaKeypad , 20, "b15");
static NexButton bKeyY = NexButton(en_WinID_AlphaKeypad , 21 , "b16");
static NexButton bKeyU = NexButton(en_WinID_AlphaKeypad , 22 , "b17");
static NexButton bKeyI = NexButton(en_WinID_AlphaKeypad , 23 , "b18");
static NexButton bKeyO = NexButton(en_WinID_AlphaKeypad , 24 , "b19");
static NexButton bKeyP = NexButton(en_WinID_AlphaKeypad , 25 , "b20");

static NexButton bKeyA = NexButton(en_WinID_AlphaKeypad , 35 , "b32");
static NexButton bKeyS = NexButton(en_WinID_AlphaKeypad , 34 , "b31");
static NexButton bKeyD = NexButton(en_WinID_AlphaKeypad , 33 , "b30");
static NexButton bKeyF = NexButton(en_WinID_AlphaKeypad , 32 , "b29");
static NexButton bKeyG = NexButton(en_WinID_AlphaKeypad , 31 , "b28");
static NexButton bKeyH = NexButton(en_WinID_AlphaKeypad , 30 , "b27");
static NexButton bKeyJ = NexButton(en_WinID_AlphaKeypad , 29 , "b26");
static NexButton bKeyK = NexButton(en_WinID_AlphaKeypad , 28 , "b25");
static NexButton bKeyL = NexButton(en_WinID_AlphaKeypad , 27 , "b24");

static NexButton bKeyZ = NexButton(en_WinID_AlphaKeypad , 36 , "b21");
static NexButton bKeyX = NexButton(en_WinID_AlphaKeypad , 37 , "b23");
static NexButton bKeyC = NexButton(en_WinID_AlphaKeypad , 38 , "b33");
static NexButton bKeyV = NexButton(en_WinID_AlphaKeypad , 39 , "b34");
static NexButton bKeyB = NexButton(en_WinID_AlphaKeypad , 40 , "b35");
static NexButton bKeyN = NexButton(en_WinID_AlphaKeypad , 41 , "b36");
static NexButton bKeyM = NexButton(en_WinID_AlphaKeypad , 42 , "b37");

static NexText tKeyboardBufferText = NexText(en_WinID_AlphaKeypad, 5 , "t1");
static NexText tKeyboardNameBuffer = NexText(en_WinID_AlphaKeypad , 4 , "t0");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bKeyBack,
									 &bKeyBackSpace,
									 &bKeyEnter,
									 &bKeyCapsLock,
									 &bKeyCapsLock1,
									 &bKeySpaceBar,
									 &bKeyDash,
									 &bKeyDot,
									 &bKeyslash,
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
									 &bKeyQ,
									 &bKeyW,
									 &bKeyE,
									 &bKeyR,
									 &bKeyT,
									 &bKeyY,
									 &bKeyU,
									 &bKeyI,
									 &bKeyO,
									 &bKeyP,
									 &bKeyA,
									 &bKeyS,
									 &bKeyD,
									 &bKeyF,
									 &bKeyG,
									 &bKeyH,
									 &bKeyJ,
									 &bKeyK,
									 &bKeyL,
									 &bKeyZ,
									 &bKeyX,
									 &bKeyC,
									 &bKeyV,
									 &bKeyB,
									 &bKeyN,
									 &bKeyM,
							  	  	 NULL};

static void HandlerButtonBack(void *ptr);
static void HandlerButtonBackSpace(void *ptr);
static void HandlerButtonEnter(void *ptr);
static void HandlerButtonCapsLock(void *ptr);
static void HandlerButtonCapsLock1(void *ptr);
static void HandlerButtonSpaceBar(void *ptr);
//static void HandlerButtonSave(void *ptr);
static void HandlerButtonKDash(void *ptr);
static void HandlerButtonKDot(void *ptr);
static void HandlerButtonKSlash(void *ptr);
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

static void HandlerButtonKQ(void *ptr);
static void HandlerButtonKW(void *ptr);
static void HandlerButtonKE(void *ptr);
static void HandlerButtonKR(void *ptr);
static void HandlerButtonKT(void *ptr);
static void HandlerButtonKY(void *ptr);
static void HandlerButtonKU(void *ptr);
static void HandlerButtonKI(void *ptr);
static void HandlerButtonKO(void *ptr);
static void HandlerButtonKP(void *ptr);
static void HandlerButtonKA(void *ptr);
static void HandlerButtonKS(void *ptr);
static void HandlerButtonKD(void *ptr);
static void HandlerButtonKF(void *ptr);
static void HandlerButtonKG(void *ptr);
static void HandlerButtonKH(void *ptr);
static void HandlerButtonKJ(void *ptr);
static void HandlerButtonKK(void *ptr);
static void HandlerButtonKL(void *ptr);
static void HandlerButtonKZ(void *ptr);
static void HandlerButtonKX(void *ptr);
static void HandlerButtonKC(void *ptr);
static void HandlerButtonKV(void *ptr);
static void HandlerButtonKB(void *ptr);
static void HandlerButtonKN(void *ptr);
static void HandlerButtonKM(void *ptr);

static void RefreshBeyBoard(bool Caps);

enWindowStatus ShowWAlphaNumKeypadScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	RefreshBeyBoard(g_CapsLock = true);

	bKeyBack.attachPush(HandlerButtonBack, &bKeyBack);
	bKeyBackSpace.attachPush(HandlerButtonBackSpace, &bKeyBackSpace);
	bKeyEnter.attachPush(HandlerButtonEnter, &bKeyEnter);
	bKeyCapsLock.attachPush(HandlerButtonCapsLock, &bKeyCapsLock);
	bKeyCapsLock1.attachPush(HandlerButtonCapsLock1, &bKeyCapsLock1);
	bKeySpaceBar.attachPush(HandlerButtonSpaceBar, &bKeySpaceBar);
//	bKeySave.attachPush(HandlerButtonSave, &bKeySave);
	bKeyDash.attachPush(HandlerButtonKDash, &bKeyDash);
	bKeyDot.attachPush(HandlerButtonKDot, &bKeyDot);
	bKeyslash.attachPush(HandlerButtonKSlash, &bKeyslash);

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

	bKeyQ.attachPush(HandlerButtonKQ, &bKeyQ);
	bKeyW.attachPush(HandlerButtonKW, &bKeyW);
	bKeyE.attachPush(HandlerButtonKE, &bKeyE);
	bKeyR.attachPush(HandlerButtonKR, &bKeyR);
	bKeyT.attachPush(HandlerButtonKT, &bKeyT);
	bKeyY.attachPush(HandlerButtonKY, &bKeyY);
	bKeyU.attachPush(HandlerButtonKU, &bKeyU);
	bKeyI.attachPush(HandlerButtonKI, &bKeyI);
	bKeyO.attachPush(HandlerButtonKO, &bKeyO);
	bKeyP.attachPush(HandlerButtonKP, &bKeyP);
	bKeyA.attachPush(HandlerButtonKA, &bKeyA);
	bKeyS.attachPush(HandlerButtonKS, &bKeyS);
	bKeyD.attachPush(HandlerButtonKD, &bKeyD);
	bKeyF.attachPush(HandlerButtonKF, &bKeyF);
	bKeyG.attachPush(HandlerButtonKG, &bKeyG);
	bKeyH.attachPush(HandlerButtonKH, &bKeyH);
	bKeyJ.attachPush(HandlerButtonKJ, &bKeyJ);
	bKeyK.attachPush(HandlerButtonKK, &bKeyK);
	bKeyL.attachPush(HandlerButtonKL, &bKeyL);
	bKeyZ.attachPush(HandlerButtonKZ, &bKeyZ);
	bKeyX.attachPush(HandlerButtonKX, &bKeyX);
	bKeyC.attachPush(HandlerButtonKC, &bKeyC);
	bKeyV.attachPush(HandlerButtonKV, &bKeyV);
	bKeyB.attachPush(HandlerButtonKB, &bKeyB);
	bKeyN.attachPush(HandlerButtonKN, &bKeyN);
	bKeyM.attachPush(HandlerButtonKM, &bKeyM);

	g_KeyBoardBuffer = std::string((char*)stcScreenNavigation.ptr_KeyBoardBuffer);
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	tKeyboardNameBuffer.setText(stcScreenNavigation.strKeyboardname.c_str());
	return WinStatus;
}

void HandlerAlphaNumKeypadScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerButtonBack(void *ptr)
{
	ChangeWindowPage(stcScreenNavigation.PrevWindowId , stcScreenNavigation.NextWindowId);
	BeepBuzzer();
}
void HandlerButtonBackSpace(void *ptr)
{
	if(0 >= g_KeyBoardBuffer.length())
	{

		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.pop_back();
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonEnter(void *ptr)
{
	strncpy((char*)stcScreenNavigation.ptr_KeyBoardBuffer,
			g_KeyBoardBuffer.c_str(),stcScreenNavigation.MaxKeyboardCharLength);
	ChangeWindowPage(stcScreenNavigation.PrevWindowId , stcScreenNavigation.NextWindowId);
	BeepBuzzer();
}
void HandlerButtonCapsLock(void *ptr)
{
	if(g_CapsLock)
	{
		RefreshBeyBoard(g_CapsLock = false);
	}
	else
	{
		RefreshBeyBoard(g_CapsLock = true);
	}
	BeepBuzzer();
}
void HandlerButtonCapsLock1(void *ptr)
{
	if(g_CapsLock)
	{
		RefreshBeyBoard(g_CapsLock = false);
	}
	else
	{
		RefreshBeyBoard(g_CapsLock = true);
	}
	BeepBuzzer();
}
void HandlerButtonSpaceBar(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(' ');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKDash(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('-');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKDot(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('.');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKSlash(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('/');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
//void HandlerButtonSave(void *ptr)
//{
//	strncpy((char*)stcScreenNavigation.ptr_KeyBoardBuffer,
//			g_KeyBoardBuffer.c_str(),stcScreenNavigation.MaxKeyboardCharLength);
//	ChangeWindowPage(stcScreenNavigation.PrevWindowId , (enWindowID)NULL);
//	BeepBuzzer();
//}

void HandlerButtonK0(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('0');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK1(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('1');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK2(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('2');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK3(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('3');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK4(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('4');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK5(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('5');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK6(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('6');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK7(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('7');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK8(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('8');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonK9(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back('9');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}

void HandlerButtonKQ(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'Q':'q');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKW(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'W':'w');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKE(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'E':'e');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKR(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'R':'r');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKT(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'T':'t');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKY(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'Y':'y');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKU(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'U':'u');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKI(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'I':'i');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKO(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'O':'o');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKP(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'P':'p');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKA(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'A':'a');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKS(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'S':'s');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKD(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'D':'d');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKF(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'F':'f');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKG(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'G':'g');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKH(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'H':'h');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKJ(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'J':'j');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKK(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'K':'k');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKL(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'L':'l');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKZ(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'Z':'z');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKX(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'X':'x');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKC(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'C':'c');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKV(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'V':'v');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKB(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'B':'b');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKN(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'N':'n');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}
void HandlerButtonKM(void *ptr)
{
	if(stcScreenNavigation.MaxKeyboardCharLength <= g_KeyBoardBuffer.length())
	{
		InstrumentBusyBuzz();
		return;
	}
	g_KeyBoardBuffer.push_back(g_CapsLock?'M':'m');
	tKeyboardBufferText.setText(g_KeyBoardBuffer.c_str());
	BeepBuzzer();
}

void RefreshBeyBoard(bool Caps)
{
	if(Caps)
	{
		bKeyQ.setText("Q");
		bKeyW.setText("W");
		bKeyE.setText("E");
		bKeyR.setText("R");
		bKeyT.setText("T");
		bKeyY.setText("Y");
		bKeyU.setText("U");
		bKeyI.setText("I");
		bKeyO.setText("O");
		bKeyP.setText("P");
		bKeyA.setText("A");
		bKeyS.setText("S");
		bKeyD.setText("D");
		bKeyF.setText("F");
		bKeyG.setText("G");
		bKeyH.setText("H");
		bKeyJ.setText("J");
		bKeyK.setText("K");
		bKeyL.setText("L");
		bKeyZ.setText("Z");
		bKeyX.setText("X");
		bKeyC.setText("C");
		bKeyV.setText("V");
		bKeyB.setText("B");
		bKeyN.setText("N");
		bKeyM.setText("M");
	}
	else
	{
		bKeyQ.setText("q");
		bKeyW.setText("w");
		bKeyE.setText("e");
		bKeyR.setText("r");
		bKeyT.setText("t");
		bKeyY.setText("y");
		bKeyU.setText("u");
		bKeyI.setText("i");
		bKeyO.setText("o");
		bKeyP.setText("p");
		bKeyA.setText("a");
		bKeyS.setText("s");
		bKeyD.setText("d");
		bKeyF.setText("f");
		bKeyG.setText("g");
		bKeyH.setText("h");
		bKeyJ.setText("j");
		bKeyK.setText("k");
		bKeyL.setText("l");
		bKeyZ.setText("z");
		bKeyX.setText("x");
		bKeyC.setText("c");
		bKeyV.setText("v");
		bKeyB.setText("b");
		bKeyN.setText("n");
		bKeyM.setText("m");
	}
}
