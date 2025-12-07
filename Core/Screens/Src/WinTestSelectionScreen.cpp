/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../APPL/Inc/NonVolatileMemory.h"
#include "../APPL/Inc/Testrun.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

#define MAX_NUM_OF_ITEMS_INTP_WINDOW (20)

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bAddTest = NexButton(en_WinID_TestSelectionScreen , 1 , "bAddTest");
static NexButton bLeftArrow = NexButton(en_WinID_TestSelectionScreen , 3 , "bLeftArrow");
static NexButton bRightArrow = NexButton(en_WinID_TestSelectionScreen , 2 , "bRightArrow");
static NexButton bBackButton = NexButton(en_WinID_TestSelectionScreen , 4 , "b3");
static NexButton bBackButtonSetup = NexButton(en_WinID_TestSelectionScreen , 55 , "b7");

static NexText tHeaderTestSelect = NexText(en_WinID_TestSelectionScreen , 46 , "t21");

static NexButton bTestPosT0 = NexButton(en_WinID_TestSelectionScreen , 5 , "bTestPosT0");
static NexButton bTestPosT1 = NexButton(en_WinID_TestSelectionScreen , 7 , "bTestPosT1");
static NexButton bTestPosT2 = NexButton(en_WinID_TestSelectionScreen , 9 , "bTestPosT2");
static NexButton bTestPosT3 = NexButton(en_WinID_TestSelectionScreen , 11 , "bTestPosT3");
static NexButton bTestPosT4 = NexButton(en_WinID_TestSelectionScreen , 13 , "bTestPosT4");
static NexButton bTestPosT5 = NexButton(en_WinID_TestSelectionScreen , 15 , "bTestPosT5");
static NexButton bTestPosT6 = NexButton(en_WinID_TestSelectionScreen , 17 , "bTestPosT6");
static NexButton bTestPosT7 = NexButton(en_WinID_TestSelectionScreen , 19 , "bTestPosT7");
static NexButton bTestPosT8 = NexButton(en_WinID_TestSelectionScreen , 21 , "bTestPosT8");
static NexButton bTestPosT9 = NexButton(en_WinID_TestSelectionScreen , 23 , "bTestPosT9");
static NexButton bTestPosT10 = NexButton(en_WinID_TestSelectionScreen , 25 , "bTestPosT10");
static NexButton bTestPosT11 = NexButton(en_WinID_TestSelectionScreen , 27 , "bTestPosT11");
static NexButton bTestPosT12 = NexButton(en_WinID_TestSelectionScreen , 29 , "bTestPosT12");
static NexButton bTestPosT13 = NexButton(en_WinID_TestSelectionScreen , 31 , "bTestPosT13");
static NexButton bTestPosT14 = NexButton(en_WinID_TestSelectionScreen , 33 , "bTestPosT14");
static NexButton bTestPosT15 = NexButton(en_WinID_TestSelectionScreen , 35 , "bTestPosT15");
static NexButton bTestPosT16 = NexButton(en_WinID_TestSelectionScreen , 37 , "bTestPosT16");
static NexButton bTestPosT17 = NexButton(en_WinID_TestSelectionScreen , 39 , "bTestPosT17");
static NexButton bTestPosT18 = NexButton(en_WinID_TestSelectionScreen , 41 , "bTestPosT18");
static NexButton bTestPosT19 = NexButton(en_WinID_TestSelectionScreen , 43 , "bTestPosT19");

static NexButton bAddTestManually = NexButton(en_WinID_TestSelectionScreen , 48 , "b4");
static NexButton bAddTestBarcode = NexButton(en_WinID_TestSelectionScreen , 49 , "b5");
static NexButton bPopUpBackButton = NexButton(en_WinID_TestSelectionScreen , 50 , "b6");

static NexButton bBarcodePopBackButton = NexButton(en_WinID_TestSelectionScreen , 53 , "b8");

static NexText tPageAvailtext = NexText(en_WinID_TestSelectionScreen , 45 , "t20");
static NexPicture pAddTestPopup = NexPicture(en_WinID_TestSelectionScreen , 47 , "p20");
static NexPicture pAddBarcodeTestPopup = NexPicture(en_WinID_TestSelectionScreen , 51 , "p21");
static NexPicture pAddBarcodeTestPopupImage = NexPicture(en_WinID_TestSelectionScreen , 52 , "p22");

static NexText tBarcodePopupTxt1 = NexText(en_WinID_TestSelectionScreen , 53 , "t24");

static NexText arrTextTP[MAX_NUM_OF_ITEMS_INTP_WINDOW] = {NexText(en_WinID_TestSelectionScreen , 5 , "t0"),
																	NexText(en_WinID_TestSelectionScreen , 7 , "t1"),
																	NexText(en_WinID_TestSelectionScreen , 9 , "t2"),
																	NexText(en_WinID_TestSelectionScreen , 11 , "t3"),
																	NexText(en_WinID_TestSelectionScreen , 13 , "t4"),
																	NexText(en_WinID_TestSelectionScreen , 15 , "t5"),
																	NexText(en_WinID_TestSelectionScreen , 17 , "t6"),
																	NexText(en_WinID_TestSelectionScreen , 19 , "t7"),
																	NexText(en_WinID_TestSelectionScreen , 21 , "t8"),
																	NexText(en_WinID_TestSelectionScreen , 23 , "t9"),
																	NexText(en_WinID_TestSelectionScreen , 25 , "t10"),
																	NexText(en_WinID_TestSelectionScreen , 27 , "t11"),
																	NexText(en_WinID_TestSelectionScreen , 29 , "t12"),
																	NexText(en_WinID_TestSelectionScreen , 31 , "t13"),
																	NexText(en_WinID_TestSelectionScreen , 33 , "t14"),
																	NexText(en_WinID_TestSelectionScreen , 35 , "t15"),
																	NexText(en_WinID_TestSelectionScreen , 37 , "t16"),
																	NexText(en_WinID_TestSelectionScreen , 39 , "t17"),
																	NexText(en_WinID_TestSelectionScreen , 41 , "t18"),
																	NexText(en_WinID_TestSelectionScreen , 43 , "t19")};

static NexPicture arrPictTP[MAX_NUM_OF_ITEMS_INTP_WINDOW] = {NexPicture(en_WinID_TestSelectionScreen , 6 , "p0"),
																	NexPicture(en_WinID_TestSelectionScreen , 8 , "p1"),
																	NexPicture(en_WinID_TestSelectionScreen , 10 , "p2"),
																	NexPicture(en_WinID_TestSelectionScreen , 12 , "p3"),
																	NexPicture(en_WinID_TestSelectionScreen , 14 , "p4"),
																	NexPicture(en_WinID_TestSelectionScreen , 16 , "p5"),
																	NexPicture(en_WinID_TestSelectionScreen , 18 , "p6"),
																	NexPicture(en_WinID_TestSelectionScreen , 20 , "p7"),
																	NexPicture(en_WinID_TestSelectionScreen , 22 , "p8"),
																	NexPicture(en_WinID_TestSelectionScreen , 24 , "p9"),
																	NexPicture(en_WinID_TestSelectionScreen , 26 , "p10"),
																	NexPicture(en_WinID_TestSelectionScreen , 28 , "p11"),
																	NexPicture(en_WinID_TestSelectionScreen , 30 , "p12"),
																	NexPicture(en_WinID_TestSelectionScreen , 32 , "p13"),
																	NexPicture(en_WinID_TestSelectionScreen , 34 , "p14"),
																	NexPicture(en_WinID_TestSelectionScreen , 36 , "p15"),
																	NexPicture(en_WinID_TestSelectionScreen , 38 , "p16"),
																	NexPicture(en_WinID_TestSelectionScreen , 40 , "p17"),
																	NexPicture(en_WinID_TestSelectionScreen , 42 , "p18"),
																	NexPicture(en_WinID_TestSelectionScreen , 44 , "p19")};


/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bAddTest,
										&bBarcodePopBackButton,
										&bLeftArrow,
										&bRightArrow,
										&bBackButton,
										&bBackButtonSetup,
										&bTestPosT0,
										&bTestPosT1,
										&bTestPosT2,
										&bTestPosT3,
										&bTestPosT4,
										&bTestPosT5,
										&bTestPosT6,
										&bTestPosT7,
										&bTestPosT8,
										&bTestPosT9,
										&bTestPosT10,
										&bTestPosT11,
										&bTestPosT12,
										&bTestPosT13,
										&bTestPosT14,
										&bTestPosT15,
										&bTestPosT16,
										&bTestPosT17,
										&bTestPosT18,
										&bTestPosT19,
										&bAddTestManually,
										&bAddTestBarcode,
										&bPopUpBackButton,
										NULL};


static void HandlerPopButtonAddTest(void *ptr);
static void HandlerPopButtonLeftArrow(void *ptr);
static void HandlerPopButtonRightArrow(void *ptr);
static void HandlerWindowButtonback(void *ptr);
static void HandlerWindowButtonbackSetup(void *ptr);
static void HandlerBarcoidePopButtonback(void *ptr);

static void ShowHideAddTestPopup(bool flag);
static void ShowHideBarcodeAddTestPopup(bool flag);

static void HandlerButtonTestPosT0(void *ptr);
static void HandlerButtonTestPosT1(void *ptr);
static void HandlerButtonTestPosT2(void *ptr);
static void HandlerButtonTestPosT3(void *ptr);
static void HandlerButtonTestPosT4(void *ptr);
static void HandlerButtonTestPosT5(void *ptr);
static void HandlerButtonTestPosT6(void *ptr);
static void HandlerButtonTestPosT7(void *ptr);
static void HandlerButtonTestPosT8(void *ptr);
static void HandlerButtonTestPosT9(void *ptr);
static void HandlerButtonTestPosT10(void *ptr);
static void HandlerButtonTestPosT11(void *ptr);
static void HandlerButtonTestPosT12(void *ptr);
static void HandlerButtonTestPosT13(void *ptr);
static void HandlerButtonTestPosT14(void *ptr);
static void HandlerButtonTestPosT15(void *ptr);
static void HandlerButtonTestPosT16(void *ptr);
static void HandlerButtonTestPosT17(void *ptr);
static void HandlerButtonTestPosT18(void *ptr);
static void HandlerButtonTestPosT19(void *ptr);

static void HandlerPopAddTestManually(void *ptr);
static void HandlerPopAddTestBarcode(void *ptr);
static void HandlerPopButtonback(void *ptr);

int8_t g_CurrentPage = 0;
uint16_t g_NumOfTPAvailable = 0;
bool Temp_warning = false;

enWindowStatus ShowTestSelectionScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;

	bAddTest.attachPush(HandlerPopButtonAddTest, &bAddTest);
	bLeftArrow.attachPush(HandlerPopButtonLeftArrow, &bLeftArrow);
	bRightArrow.attachPush(HandlerPopButtonRightArrow, &bRightArrow);
	bBackButton.attachPush(HandlerWindowButtonback, &bBackButton);
	bBackButtonSetup.attachPush(HandlerWindowButtonbackSetup, &bBackButtonSetup);

	bTestPosT0.attachPush(HandlerButtonTestPosT0, &bTestPosT0);
	bTestPosT1.attachPush(HandlerButtonTestPosT1, &bTestPosT1);
	bTestPosT2.attachPush(HandlerButtonTestPosT2, &bTestPosT2);
	bTestPosT3.attachPush(HandlerButtonTestPosT3, &bTestPosT3);
	bTestPosT4.attachPush(HandlerButtonTestPosT4, &bTestPosT4);
	bTestPosT5.attachPush(HandlerButtonTestPosT5, &bTestPosT5);
	bTestPosT6.attachPush(HandlerButtonTestPosT6, &bTestPosT6);
	bTestPosT7.attachPush(HandlerButtonTestPosT7, &bTestPosT7);
	bTestPosT8.attachPush(HandlerButtonTestPosT8, &bTestPosT8);
	bTestPosT9.attachPush(HandlerButtonTestPosT9, &bTestPosT9);
	bTestPosT10.attachPush(HandlerButtonTestPosT10, &bTestPosT10);
	bTestPosT11.attachPush(HandlerButtonTestPosT11, &bTestPosT11);
	bTestPosT12.attachPush(HandlerButtonTestPosT12, &bTestPosT12);
	bTestPosT13.attachPush(HandlerButtonTestPosT13, &bTestPosT13);
	bTestPosT14.attachPush(HandlerButtonTestPosT14, &bTestPosT14);
	bTestPosT15.attachPush(HandlerButtonTestPosT15, &bTestPosT15);
	bTestPosT16.attachPush(HandlerButtonTestPosT16, &bTestPosT16);
	bTestPosT17.attachPush(HandlerButtonTestPosT17, &bTestPosT17);
	bTestPosT18.attachPush(HandlerButtonTestPosT18, &bTestPosT18);
	bTestPosT19.attachPush(HandlerButtonTestPosT19, &bTestPosT19);

	bAddTestManually.attachPush(HandlerPopAddTestManually, &bAddTestManually);
	bAddTestBarcode.attachPush(HandlerPopAddTestBarcode, &bAddTestBarcode);
	bPopUpBackButton.attachPush(HandlerPopButtonback, &bPopUpBackButton);
	bBarcodePopBackButton.attachPush(HandlerBarcoidePopButtonback, &bBarcodePopBackButton);

	Temp_warning = true;

	ptr_obJCurrPage->show();

	for(uint8_t nI = 0 ; nI < MAX_NUM_OF_ITEMS_INTP_WINDOW ; ++nI)
	{
		arrTextTP[nI].setVisible(false);
		arrPictTP[nI].setVisible(false);
	}

	if(en_WinId_TestParamsGeneral == stcScreenNavigation.NextWindowId)
	{
		bAddTest.setVisible(true);
		tHeaderTestSelect.setText("Test Setup");
		bBackButton.setVisible(false);
		bBackButtonSetup.setVisible(true);
	}
	else
	{
		bAddTest.setVisible(false);
		if(en_WinID_QcTestScreen == stcScreenNavigation.NextWindowId)
		{
			tHeaderTestSelect.setText("Select Test");
			bBackButton.setVisible(false);
			bBackButtonSetup.setVisible(true);
		}
		else
		{
			tHeaderTestSelect.setText("Run Test");
			bBackButton.setVisible(true);
			bBackButtonSetup.setVisible(false);
		}
	}
	/* * disable all popups */
	//ShowHideAddTestPopup(false);/*Hide popup for add test initially*/
	//ShowHideBarcodeAddTestPopup(false);
	/* * */

	/*at last read all test parameters*/
//	TODO show waiting popup
	g_NumOfTPAvailable = GetTestParamsTotalItemSaved();/*This function will also read test parameters from flash memory*/
	//g_NumOfTPAvailable = 200;// to be removed -> just for testing
	//Hide waiting popup
	g_CurrentPage = 0;
	UpdateTestSelectionGridDetails();/*show database grid from staring loc*/
	return WinStatus;
}
bool UpdateTestSelectionGridDetails(void)
{
	uint16_t TotalPage = (uint16_t)abs(g_NumOfTPAvailable / MAX_NUM_OF_ITEMS_INTP_WINDOW);

	if (g_NumOfTPAvailable % MAX_NUM_OF_ITEMS_INTP_WINDOW != 0)
	{
	    TotalPage += 1; // Increment total page by one if there are remaining items
	}
	if(TotalPage == 0)
	{
		TotalPage = TotalPage + 1;
	}
	if(g_CurrentPage >= TotalPage)
	{
		g_CurrentPage = 0;
//		g_CurrentPage--;
//		return false;
	}
	else if(g_CurrentPage < 0)
	{
		g_CurrentPage = TotalPage - 1;
//		g_CurrentPage++;
//		return false;
	}

	std::string PageTxt = std::string("Page : ") + std::to_string(g_CurrentPage + 1)
											+ std::string("/") +  std::to_string(TotalPage);
	tPageAvailtext.setText(PageTxt.c_str());
	uint16_t DataStartingLoc = (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	for(uint8_t nI = 0 ; nI < MAX_NUM_OF_ITEMS_INTP_WINDOW ; ++nI)
	{
		if(DataStartingLoc < g_NumOfTPAvailable)/*Print data only if valid data is available*/
		{
			arrTextTP[nI].setVisible(true);
			arrPictTP[nI].setVisible(true);
			arrTextTP[nI].setText(GetInstance_TestParams()[DataStartingLoc].arrTestName);
			switch(GetInstance_TestParams()[DataStartingLoc].TestProcedureType)
			{
				case en_Endpoint:
					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_ENDPOINT);
					break;
				case en_FixedTime:
					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_FIXEDPOINT);
					break;
				case en_Kinetics:
					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_KINETICS);
					break;
				case en_Turbidimetry:
					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_TURBIDIMETRY);
					break;
//				case en_Differential:
//					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_DIFFRENTIAL);
//					break;
				case en_CoagulationTest:
					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_COAGULATION);
					break;
				case en_Absorbance:
					arrPictTP[nI].Set_background_image_pic(IMAGE_ID_ENDPOINT);
					break;
				default:
					arrPictTP[nI].Set_background_image_pic(DELAULT_NO_TEST_IMAGE_ID);
					break;
			}
			DataStartingLoc++;
		}
		else/*if valid data is finished print dummy*/
		{
			arrTextTP[nI].setVisible(false);
			arrPictTP[nI].setVisible(false);
		}
	}
	return true;
}

void HandlerTestSelectionScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}
void ShowHideAddTestPopup(bool flag)
{
	pAddTestPopup.setVisible(flag);
	bAddTestManually.setVisible(flag);
	bAddTestBarcode.setVisible(flag);
	bPopUpBackButton.setVisible(flag);
}
void ShowHideBarcodeAddTestPopup(bool flag)
{
	if(flag)
	{
		bBackButtonSetup.setVisible(false);
	}
	else
	{
		bBackButtonSetup.setVisible(true);
	}
	pAddBarcodeTestPopup.setVisible(flag);
	pAddBarcodeTestPopupImage.setVisible(flag);
	tBarcodePopupTxt1.setVisible(flag);
	bBarcodePopBackButton.setVisible(flag);
}

static void HandlerBarcoidePopButtonback(void *ptr)
{
	ShowHideBarcodeAddTestPopup(false);
	BeepBuzzer();
}
void HandlerPopButtonAddTest(void *ptr)/*Add test main button*/
{
	BeepBuzzer();
	uint8_t SlotAvail = GetSlotForSavingTestParam();
	if(0 <= SlotAvail && MAX_NUM_OF_TEST_PARAMS_SAVED > SlotAvail)/*Check for valid slot available*/
	{
		stcTestData.TestId = SlotAvail;
//		InitTestParameterBuffer(SlotAvail);


		/*Disabled on 29-11-24, its not having barcode scan option*/
//		ShowHideAddTestPopup(true);
//		ShowHideBarcodeAddTestPopup(false);

		/*Added on 29-11-24, its not having barcode scan option. So it willgo to manual entry mode directly*/
		/*Image in nextion display also changed. Old Pic 213 Pic2 199  New Pic 580 Pic2 579*/
		InitTestParameterBuffer(stcTestData.TestId);
		ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	}
	else
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.NextWindowId;
		enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
		if(enkeyOk == ShowMainPopUp("Test Setup","Maximum Tests Added", true))
		{
			/*Show pop up for memory full*/
			ChangeWindowPage(en_WinID_TestSelectionScreen , (enWindowID)NextWindow);
			stcScreenNavigation.PrevWindowId = PrevWindow;
		}
	}
}

void HandlerPopButtonLeftArrow(void *ptr)
{
	g_CurrentPage--;
	if(UpdateTestSelectionGridDetails())
	{
		BeepBuzzer();
	}
}

void HandlerPopButtonRightArrow(void *ptr)
{
	g_CurrentPage++;
	if(UpdateTestSelectionGridDetails())
	{
		BeepBuzzer();
	}
}

void HandlerWindowButtonback(void *ptr)
{
//	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	ChangeWindowPage(stcScreenNavigation.PrevWindowId , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerWindowButtonbackSetup(void *ptr)
{
	ChangeWindowPage(stcScreenNavigation.PrevWindowId , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerPopAddTestManually(void *ptr)
{
	InitTestParameterBuffer(stcTestData.TestId);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}

void HandlerPopAddTestBarcode(void *ptr)
{
	ShowHideBarcodeAddTestPopup(true);
	ShowHideAddTestPopup(false);
	BeepBuzzer();
}

void HandlerPopButtonback(void *ptr)
{
	ShowHideAddTestPopup(false);
	BeepBuzzer();
}

static void HandlerButtonTestPosT0(void *ptr)
{
	stcTestData.TestId = 0 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT1(void *ptr)
{
	stcTestData.TestId = 1 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT2(void *ptr)
{
	stcTestData.TestId = 2 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT3(void *ptr)
{
	stcTestData.TestId = 3 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT4(void *ptr)
{
	stcTestData.TestId = 4 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT5(void *ptr)
{
	stcTestData.TestId = 5 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT6(void *ptr)
{
	stcTestData.TestId = 6 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT7(void *ptr)
{
	stcTestData.TestId = 7 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT8(void *ptr)
{
	stcTestData.TestId = 8 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT9(void *ptr)
{
	stcTestData.TestId = 9 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT10(void *ptr)
{
	stcTestData.TestId = 10 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT11(void *ptr)
{
	stcTestData.TestId = 11 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT12(void *ptr)
{
	stcTestData.TestId = 12 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT13(void *ptr)
{
	stcTestData.TestId = 13 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT14(void *ptr)
{
	stcTestData.TestId = 14 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT15(void *ptr)
{
	stcTestData.TestId = 15 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT16(void *ptr)
{
	stcTestData.TestId = 16 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT17(void *ptr)
{
	stcTestData.TestId = 17 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT18(void *ptr)
{
	stcTestData.TestId = 18 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerButtonTestPosT19(void *ptr)
{
	stcTestData.TestId = 19 + (g_CurrentPage * MAX_NUM_OF_ITEMS_INTP_WINDOW);
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
