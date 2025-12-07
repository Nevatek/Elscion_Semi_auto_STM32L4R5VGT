/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "HandlerPheripherals.hpp"
#include "../../Screens/Inc/Screens.h"
#include "../APPL/Inc/Testrun.h"
#include "../APPL/Inc/ApplUsbDevice.h"
#include "../APPL/Inc/calibration.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include "Appl_Timer.hpp"

#define DEFAULT_TEST_TIMER_CNT_VAL_MS (1000)
#define MAX_TIMER_MINS (60)
#define MIN_TIMER_MINS (0)

#define BUTTON_ABORT (96)
#define BUTTON_ASPIRATE (63)
#define BUTTON_ASPIRATE_DISABLE (76)
#define BUTTON_ABORT_DISABLE (322)
#define PIC_DESELECTED_WATER_BLANK (77)
#define PIC_SELECTED_WATER_BLANK (78)
#define PIC_DESELECTED_REG_BLANK (80)
#define PIC_SELECTED_REG_BLANK (79)
#define PIC_DESELECTED_STD (82)
#define PIC_SELECTED_STD (81)
#define PIC_DESELECTED_QC (84)
#define PIC_SELECTED_QC (83)
#define PIC_DESELECTED_SAMPLE (87)
#define PIC_SELECTED_SAMPLE (85)
#define PIC_DESELECTED_WASH (88)
#define PIC_SELECTED_WASH (86)

#define PIC_DISABLED_QC (228)
#define PIC_DISABLED_REG_BLANK (229)
#define PIC_DISABLED_STD (230)

#define PIC_DESELECTED_STANDARD (95)
#define PIC_SELECTED_STANDARD (94)

#define PIC_GRAPH (65)
#define PIC_TXT_ADC (575)
#define PIC_TXT_ABS (574)

#define MAX_TIMER (4)
#define PIC_TIMER_SELECT (577)
#define PIC_TIMER_DESELECT (578)

typedef struct
{
	int8_t u8Minute;
	int8_t u8Second;
	bool bTimerRunFlag;
	bool bTimerBackgroundComplete;
	bool bTimerWindowshown;
	stcTimer m_Timer;
}TIMER_BLOCK;

static TIMER_BLOCK stcTimerBlock[4];
static uint8_t timerIndex = 0;
uint8_t InvalidQCcount = 0;
uint16_t LastTestId = 0xFF;

/*Main buttons start*/
static NexButton buttonWaterBlank = NexButton(en_WinId_TestRun , 11 , "b6");
static NexButton buttonReagentBlank = NexButton(en_WinId_TestRun , 12 , "b7");
static NexButton buttonStandard = NexButton(en_WinId_TestRun , 13 , "b8");
static NexButton buttonQc = NexButton(en_WinId_TestRun , 14 , "b9");
static NexButton buttonSampleRun = NexButton(en_WinId_TestRun , 15 , "b10");
static NexButton buttonWash = NexButton(en_WinId_TestRun , 16 , "b11");
static NexButton buttonAspirate = NexButton(en_WinId_TestRun , 4 , "b0");
static NexButton buttonTimer = NexButton(en_WinId_TestRun , 10 , "b2");
static NexButton buttonBackButton = NexButton(en_WinId_TestRun , 1 , "b5");
static NexPicture pInitbackgrd = NexPicture(en_WinId_TestRun , 120 , "p11");

static NexPicture pInitPopup = NexPicture(en_WinId_TestRun , 121 , "p12");
static NexText tInitPopupTitle = NexText(en_WinId_TestRun , 122 , "t32");
static NexText tInitPopupMsg = NexText(en_WinId_TestRun , 123 , "t33");

static NexPicture pWash_topline = NexPicture(en_WinId_TestRun , 17 , "p2");
/*Main buttons ends*/

/*Options PopUp Starts*/
static NexButton buttonShowOption = NexButton(en_WinId_TestRun , 99 , "b29");
static NexButton buttonCloseOption = NexButton(en_WinId_TestRun , 104 , "b30");
static NexButton buttonParameter = NexButton(en_WinId_TestRun , 101 , "b1");
static NexButton buttonCalData = NexButton(en_WinId_TestRun , 102 , "b3");
static NexButton buttonQcData = NexButton(en_WinId_TestRun , 103 , "b4");
static NexPicture pOptionsBG = NexPicture(en_WinId_TestRun , 100 , "p17");
/*Options PopUp Ends*/

/*Title Bar txt and Pictures Starts*/
static NexPicture pTestTypePic = NexPicture(en_WinId_TestRun , 2 , "p0");
static NexText textTestName = NexText(en_WinId_TestRun , 3 , "t0");
static NexText textTestWavelength = NexText(en_WinId_TestRun , 114 , "t5");
static NexText textDate = NexText(en_WinId_TestRun , 21 , "t6");
static NexText textTime = NexText(en_WinId_TestRun , 88 , "t75");
static NexText textTemperature = NexText(en_WinId_TestRun , 22 , "t7");
static NexText textUserName = NexText(en_WinId_TestRun , 24 , "t8");
static NexPicture pUserlogin = NexPicture(en_WinId_TestRun , 25 , "p6");
/*Incubator temp and image*/
static NexText textIncuTemp = NexText(en_WinId_TestRun , 87 , "t74");
static NexText textIncuTempC = NexText(en_WinId_TestRun , 90 , "t77");
static NexPicture pIncubator = NexPicture(en_WinId_TestRun , 86 , "p15");
/*Title Bar txt and Pictures Ends*/

static NexText textTestStatusBuffer = NexText(en_WinId_TestRun , 20 , "t4");
static NexText textPatientIDNameBoard = NexText(en_WinId_TestRun , 40 , "t12");
static NexText textPatientIDBox = NexText(en_WinId_TestRun , 39 , "t13");

/*Standard Buttons*/
static NexButton buttonStd[MAX_NUM_OF_STANDARDS] = {NexButton(en_WinId_TestRun , 34 , "b17"),/*0*/
													NexButton(en_WinId_TestRun , 26 , "b12"),/*1*/
													NexButton(en_WinId_TestRun , 27 , "b13"),/*2*/
													NexButton(en_WinId_TestRun , 28 , "b14"),/*3*/
													NexButton(en_WinId_TestRun , 29 , "b15"),/*4*/
													NexButton(en_WinId_TestRun , 30 , "b16")/*5*/};

/*Result Box Starts*/
static NexText textAdcAbsNameBoard = NexText(en_WinId_TestRun , 32 , "t10");
static NexText textConcNameBoard = NexText(en_WinId_TestRun , 33 , "t11");
static NexText textStdFactorNameBoard = NexText(en_WinId_TestRun , 95 , "t81");
static NexText textUnitNameBoard = NexText(en_WinId_TestRun , 92 , "t78");

static NexText textAbsorbance = NexText(en_WinId_TestRun , 7 , "t1");
static NexText textConcentration = NexText(en_WinId_TestRun , 31 , "t9");
static NexText textStdFactor = NexText(en_WinId_TestRun , 94 , "t80");
static NexText textUnit = NexText(en_WinId_TestRun , 93 , "t79");
/*Result Box Ends*/

NexText textMainGraphXMax = NexText(en_WinId_TestRun , 39 , "t67");
NexText textMainGraphYMax = NexText(en_WinId_TestRun , 35 , "t66");
NexText textMainGraphXMin = NexText(en_WinId_TestRun , 38 , "t69");
NexText textMainGraphYMin = NexText(en_WinId_TestRun , 37 , "t68");
static NexPicture pGraphBG = NexPicture(en_WinId_TestRun , 113 , "p7");

/*Test param data objects ID - START*/
static NexText textParamsName = NexText(en_WinId_TestRun , 64 , "t49");
static NexText textParamsUnits = NexText(en_WinId_TestRun , 65 , "t50");
static NexText textParamsReagentBlank = NexText(en_WinId_TestRun , 66 , "t51");
static NexText textParamsSampleBlank = NexText(en_WinId_TestRun , 67 , "t52");
static NexText textParamsMethod = NexText(en_WinId_TestRun , 68 , "t53");
static NexText textParamsPrimaryFilter = NexText(en_WinId_TestRun , 69 , "t54");
static NexText textParamsSecondaryFilter = NexText(en_WinId_TestRun , 70 , "t55");
static NexText textParamsTemperature = NexText(en_WinId_TestRun , 71 , "t56");
static NexText textParamsAx_B = NexText(en_WinId_TestRun , 97 , "t83");
static NexText textParamsAspVol = NexText(en_WinId_TestRun , 79 , "t64");
static NexText textParamsLagTimeS = NexText(en_WinId_TestRun , 78 , "t63");
static NexText textParamsReadTimeS = NexText(en_WinId_TestRun , 77 , "t62");
static NexText textParamsCuvetteType = NexText(en_WinId_TestRun , 76 , "t61");
static NexText textParamsRefRange = NexText(en_WinId_TestRun , 75 , "t60");
static NexText textParamsLinearityRange = NexText(en_WinId_TestRun , 74 , "t59");
static NexText textParamsFactor = NexText(en_WinId_TestRun , 73 , "t58");
static NexText textParamsCalibrartion = NexText(en_WinId_TestRun , 72 , "t57");
static NexText textParamsStandards = NexText(en_WinId_TestRun , 80 , "t65");

static NexPicture pParamsBox = NexPicture(en_WinId_TestRun , 46 , "p10");
static NexButton bParamsCloseButton = NexButton(en_WinId_TestRun , 82 , "b22");
static NexButton bParamsPrintButton = NexButton(en_WinId_TestRun , 81 , "b21");

#define MAX_TEST_PARAMS_DATA_STATIC_TEXT_FEILDS (18)
static NexText textparamStaticTexts[MAX_TEST_PARAMS_DATA_STATIC_TEXT_FEILDS] =
											{NexText(en_WinId_TestRun , 47 , "t15"),/*0*/
											NexText(en_WinId_TestRun , 48 , "t16"),/*1*/
											NexText(en_WinId_TestRun , 49 , "t17"),/*2*/
											NexText(en_WinId_TestRun , 50 , "t18"),/*3*/
											NexText(en_WinId_TestRun , 51 , "t19"),/*4*/
											NexText(en_WinId_TestRun , 52 , "t20"),/*5*/
											NexText(en_WinId_TestRun , 53 , "t21"),/*6*/
											NexText(en_WinId_TestRun , 54 , "t22"),/*7*/
											NexText(en_WinId_TestRun , 55 , "t23"),/*8*/
											NexText(en_WinId_TestRun , 56 , "t24"),/*9*/
											NexText(en_WinId_TestRun , 57 , "t25"),/*10*/
											NexText(en_WinId_TestRun , 58 , "t26"),/*11*/
											NexText(en_WinId_TestRun , 59 , "t27"),/*12*/
											NexText(en_WinId_TestRun , 60 , "t28"),/*13*/
											NexText(en_WinId_TestRun , 61 , "t29"),/*14*/
											NexText(en_WinId_TestRun , 62 , "t30"),/*15*/
											NexText(en_WinId_TestRun , 63 , "t31"),/*16*/
											NexText(en_WinId_TestRun , 96 , "t82"),/*17*/
											};
/*Test param data objects ID - END*/

/*Below is IDs for test abort POP UP */
static NexButton bTestAbortPopupYes = NexButton(en_WinId_TestRun , 43 , "b20");
static NexButton bTestAbortPopupNo = NexButton(en_WinId_TestRun , 42 , "b18");
static NexButton bTestAbortPopupOK = NexButton(en_WinId_TestRun , 45 , "b23");

static NexText tTestAbortPopuptitle = NexText(en_WinId_TestRun , 44 , "t14"); /*popup title*/
static NexText tTestAbortPopupMsg1 = NexText(en_WinId_TestRun , 83 , "t70"); /*popup msg1*/
static NexText tTestAbortPopupMsg2 = NexText(en_WinId_TestRun , 84 , "t71"); /*popup msg2*/
static NexText tTestAbortPopupMsg3 = NexText(en_WinId_TestRun , 85 , "t72"); /*popup msg3*/

static NexPicture pTestAbortpopup = NexPicture(en_WinId_TestRun , 41 , "p8");
//static NexPicture pTestAbortpopupMsgBox = NexPicture(en_WinId_TestRun , 68 , "p9");

/*Above is IDs for test abort POP UP  - END*/

/*Below is IDs for test Timer - Start*/

static NexPicture pTImerPicA = NexPicture(en_WinId_TestRun , 105 , "p13");
static NexPicture pTImerPicB = NexPicture(en_WinId_TestRun , 108 , "p14");
static NexButton bTimerStartStop = NexButton(en_WinId_TestRun , 106 , "b24");
static NexButton bTimerboxHide = NexButton(en_WinId_TestRun , 111 , "b28");
static NexButton bTimerReset = NexButton(en_WinId_TestRun , 107 , "b25");
static NexButton bTimerPlus = NexButton(en_WinId_TestRun , 109 , "b26");
static NexButton bTimerMinus = NexButton(en_WinId_TestRun , 110 , "b27");
static NexText tTimerValTxt = NexText(en_WinId_TestRun , 112 , "t73");
static NexText tTimerValRunscrtext = NexText(en_WinId_TestRun , 98 , "t84");

static NexPicture pTimButtonBar = NexPicture(en_WinId_TestRun , 119 , "p9");
static NexButton bTimerOne = NexButton(en_WinId_TestRun , 115 , "b19");
static NexButton bTimerTwo = NexButton(en_WinId_TestRun , 116 , "b31");
static NexButton bTimerThree = NexButton(en_WinId_TestRun , 117 , "b32");
static NexButton bTimerFour = NexButton(en_WinId_TestRun , 118 , "b33");

extern WindowMapping stcWindowMapping[en_WinID_Max];
static NexWaveform wtestGraph = NexWaveform(en_WinId_TestRun , 6 , "s0");
static NexText tADC_Abs = NexText(en_WinId_TestRun , 8 , "t2");
//static NexRtc HandlerRtc;

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&buttonWaterBlank,
									&buttonReagentBlank,
									&buttonStandard,
									&buttonQc,
									&buttonSampleRun,
									&buttonWash,
									&buttonAspirate,
									&buttonShowOption,
									&buttonCloseOption,
									&buttonParameter,
									&buttonCalData,
									&buttonQcData,
									&buttonTimer,
									&buttonBackButton,
									&buttonStd[0],
									&buttonStd[1],
									&buttonStd[2],
									&buttonStd[3],
									&buttonStd[4],
									&buttonStd[5],
//									&bParamsCloseButton,
//									&bParamsPrintButton,
									&bTestAbortPopupYes,
									&bTestAbortPopupNo,
									&bTestAbortPopupOK,
									&textPatientIDBox,
									&bTimerStartStop,
									&bTimerboxHide,
									&bTimerReset,
									&bTimerPlus,
									&bTimerMinus,
									&bTimerOne,
									&bTimerTwo,
									&bTimerThree,
									&bTimerFour,
									&pUserlogin,
									NULL};

std::string Temp_Err_Msg = "";
en_TestPopUp g_en_TestPopUp = enTestPopUp_Null;
float e_RunTimeStdOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard*/
float e_RunTimeStdBlankOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard*/
float e_Runtime4plA;
float e_Runtime4plB;
float e_Runtime4plC;
float e_Runtime4plD;

extern bool e_limit_error_popup;
extern bool e_reagent_blank_limit_flag;
extern bool e_abs_limit_flag;
extern bool e_linearity_limit_flag;
extern bool e_delAmin_limit_flag;
extern bool e_StdDoneflag;
bool Manual_Wash = false;
static bool ReDrawPrevGraph = false; // External trigger issue. Added to avoid the new graph line plot on top of old grpah
extern bool Temp_warning;
extern bool WashBtwSample;
static void HandlerButtonAspirate(void *ptr);
static void HandlerButtonPatientId(void *ptr);
void UpdateTestStatusTextBuffer(std::string str);
static void ShowHideTestParamsDataBox(bool flag);
static void ShowHideOptionsBox(bool flag);

static void HandlerButtonWaterBlank(void *ptr);
static void HandlerButtonReagentBlank(void *ptr);
static void HandlerButtonStandard(void *ptr);
static void HandlerButtonQc(void *ptr);
static void HandlerButtonSampleRun(void *ptr);
static void HandlerButtonWash(void *ptr);
static void HandlerButtonParameter(void *ptr);
static void HandlerButtonTimer(void *ptr);
static void HandlerButtonShowOptions(void *ptr);
static void HandlerButtonCloseOptions(void *ptr);
static void HandlerButtonCalData(void *ptr);
static void HandlerButtonQcData(void *ptr);
static void HandlerButtonBack(void *ptr);
static void HandlerButtonStd0(void *ptr);
static void HandlerButtonStd1(void *ptr);
static void HandlerButtonStd2(void *ptr);
static void HandlerButtonStd3(void *ptr);
static void HandlerButtonStd4(void *ptr);
static void HandlerButtonStd5(void *ptr);
//static void HandlerButtonbParamsCloseButton(void *ptr);
//static void HandlerButtonbParamsPrintButton(void *ptr);
static void HandlerButtonbTestAbortPopupYes(void *ptr);
static void HandlerButtonbTestAbortPopupNo(void *ptr);
static void HandlerButtonbTestAbortPopupOK(void *ptr);

static void HandlerButtonbTimerStartStop(void *ptr);
static void HandlerButtonbTimerboxHide(void *ptr);
static void HandlerButtonbTimerReset(void *ptr);
static void HandlerButtonbTimerPlus(void *ptr);
static void HandlerButtonbTimerMinus(void *ptr);
static void HandlerButtonbTimerOne(void *ptr);
static void HandlerButtonbTimerTwo(void *ptr);
static void HandlerButtonbTimerThree(void *ptr);
static void HandlerButtonbTimerFour(void *ptr);
static void InitilizeTimerBlock(void);
static void  ShowHideTimerWindow(bool bStatus, int timerIndex);
static void ShowHideRunScreenTimer(bool bStatus, int timerIndex);
static void HandlerUserlogin(void *ptr);

enWindowStatus ShowTestRunScreen (NexPage *ptr_obJCurrPage)
{

	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	ReDrawPrevGraph = true;
	/*If prevWind is testcalib and user accept previous cal data - change meas type to sample */
	if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
	{
		textIncuTemp.setVisible(true);
		textIncuTempC.setVisible(true);
		pIncubator.setVisible(true);
	}
	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;

	if(en_WinId_TestResultScreen == PrevWindow && GetInstance_TestParams()[stcTestData.TestId].AspVoleume < 300 )
	{
		WashBtwSample = true;
		stcTestData.CurrentTestStatus = enTestStatus_ManualWash_Busy;
	}
	UpdateStatusBar(&pTestTypePic , &textTestName , &textTestWavelength ,
			&textDate, &textTime, &textTemperature , &textIncuTemp, &textUserName);

	/*Variables Initialization - STARTS*/

	for(int nI = 0; nI < MAX_TIMER ; ++nI)
	{
		if(true == stcTimerBlock[nI].bTimerBackgroundComplete)
		{
			ShowHideTimerWindow(true , nI);
		}
	}
	/*When test screen starts - Copying current std OD saved to RUN time buffer of std OD*/

	if(en_WinID_TestSelectionScreen == PrevWindow)/*Resets variables only if we are selecting new test from test selection screen*/
	{
		/*Initialize test variables - START*/
		NVM_ReadQcSetup();

		stcTestData.CurrentMeasType = en_DIWaterAdcBlank;/*Resetting Measurement type to do water blank*/

		ResetSelectedQcIdx();
		stcTestData.arrPatientIDBuffer[0] = '\0';
		stcTestData.PrevMeasCompletedType = en_MeasurementNull;
		stcTestData.CurrentTestStatus = enTestStatus_Idle;/*Make test run status to IDLE state*/
		stcTestData.DarkAdcPrimary = 0U;
		stcTestData.DarkAdcSecondary = 0U;
		stcTestData.Result = (float)0;
		stcTestData.SelectedStandard = 0/*S0*/;
		stcTestData.SampleBlankAbs = (float)0;
		stcTestData.SampleOpticalDensity = (float)0;
		stcTestData.DIPrimaryFilterWaterAdc = GetInstance_TestParams()[stcTestData.TestId].DiwaterPriADC;
		stcTestData.DISecondaryFilterWaterAdc = GetInstance_TestParams()[stcTestData.TestId].DiwaterSecADC;
		stcTestData.DIPrimaryFilterWaterGain = GetInstance_TestParams()[stcTestData.TestId].DiwaterPriGain;
		stcTestData.DISecondaryFilterWaterGain = GetInstance_TestParams()[stcTestData.TestId].DiwaterSecGain;
		stcTestData.WaterBlankDoneFlag = GetInstance_TestParams()[stcTestData.TestId].DiwaterDoneFlag;
		/*Loop to reset primary wavelength data buffer*/
		for(uint16_t nI = 0 ; nI < MAX_ADC_BUFFER_LENGTH_PRIMARY ; ++nI)
		{
			stcTestData.PrimaryAdcBuffer[nI] = 0;
			stcTestData.PrimaryAbsBuffer[nI] = (float)0;
		}
		/*Loop to reset secondary wavelength data buffer*/
		for(uint16_t nI = 0 ; nI < MAX_ADC_BUFFER_LENGTH_DEFAULT ; ++nI)
		{
			stcTestData.SecondaryAdcBuffer[nI] = 0;
			stcTestData.SecondaryAbsBuffer[nI] = (float)0;
		}

		/*loop to copy standard OD from memory to  run time buffer*/
		for(uint8_t nI = 0 ; nI < GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved ; ++nI)
		{
			e_RunTimeStdOd[nI] = GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI];
			e_RunTimeStdBlankOd[nI] = GetInstance_TestParams()[stcTestData.TestId].StandardODBlank[nI];
		}

		 e_Runtime4plA = GetInstance_TestParams()[stcTestData.TestId].FourplA;
		 e_Runtime4plD = GetInstance_TestParams()[stcTestData.TestId].FourplD;
		 e_Runtime4plC = GetInstance_TestParams()[stcTestData.TestId].FourplC;
		 e_Runtime4plB = GetInstance_TestParams()[stcTestData.TestId].FourplB;

		/*Initialize test variables - END*/
		buttonAspirate.Set_background_image_pic(BUTTON_ASPIRATE_DISABLE);
		buttonAspirate.setText(" ");
		InitilizetestPheripherals();/*Initilize home position of motor once enters the test screen*/
		if(stcTimerBlock[0].bTimerRunFlag != true && stcTimerBlock[1].bTimerRunFlag != true &&
				stcTimerBlock[2].bTimerRunFlag != true && stcTimerBlock[3].bTimerRunFlag != true)
		{
			InitilizeTimerBlock();
		}
	}
	/*Variables Initialization - ENDS*/

	/*UI Initialization - STARTS*/
	ShowHideTestScreenPopup(enTestPopUp_Null , std::string(" "));

	pGraphBG.setPic(PIC_GRAPH);
	for(uint8_t nI = 0 ;
			nI < GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved ; ++nI)
	{
		char buffStd[16] = {0};
		snprintf(buffStd , 15 , "S%d" , nI + 1);
		buttonStd[nI].setText(buffStd);
	}
	/*Updating QC count*/
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	InvalidQCcount = MAX_NUM_OF_QC_CONTROL_PER_TEST;
	for (uint8_t u8SlotIndex = 0; u8SlotIndex < MAX_NUM_OF_QC_CONTROL_PER_TEST; ++u8SlotIndex)
	{
		if (m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].u8QcIdx != NVM_INIT_NOT_OK_FLAG)
		{
			InvalidQCcount--;
		}
	}
	if(en_WinID_TestSelectionScreen != PrevWindow)
	{
		if(en_WinId_TestResultScreen == PrevWindow )
		{
			stcTestData.arrPatientIDBuffer[0] = '\0';
			ResetSelectedQcIdx();
		}
	if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest){
		switch(stcTestData.CurrentMeasType)
		{
			case en_DIWaterAdcBlank:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(COAG_DI_WATER);
				break;
			case en_CoagulationsampleIncubation:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(COAG_SAMPLE);
				break;
			case en_CoagulationQCIncubation:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(COAG_QC);
				break;
			case en_Sample:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(COAG_ADD_REAGENT_MEASU);
				break;
			case en_TestQc:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(COAG_ADD_REAGENT_MEASU);
				break;
			case en_WashProcedure:
				break;
			default: break;
			}
		DrawRescaledGraph();
		}
		else{
		uint8_t IndexStandard = 0;
//		char arrBuff[64] = {0};
		switch(stcTestData.CurrentMeasType)
		{
			case en_DIWaterAdcBlank:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_DI_WATER);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_ReagentBlank:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_REAGENT_BLANK);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_Reagent:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_REAGENT);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_StandardBlank:
				if(0xFF == stcTestData.SelectedStandard)
				{
					IndexStandard = 0;
					stcTestData.SelectedStandard = (GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved - 1);
					UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
					stcTestData.SelectedStandard = 0;
					UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				}
				else
				{
					UpdateTestScreenMenu();/*Update test screen menu / buttons*/
					IndexStandard = stcTestData.SelectedStandard;
					UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				}
				UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
				break;
			case en_Standard:
				if(0xFF == stcTestData.SelectedStandard)
				{
					IndexStandard = 0;
					stcTestData.SelectedStandard = (GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved - 1);
					UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
					stcTestData.SelectedStandard = 0;
					UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				}
				else
				{
					UpdateTestScreenMenu();/*Update test screen menu / buttons*/
					IndexStandard = stcTestData.SelectedStandard;
					UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				}
//				snprintf(arrBuff , 63 , "Please aspirate standard %d" , IndexStandard + 1);
//				UpdateTestStatusTextBuffer(arrBuff);
				UpdateTestStatusTextBuffer(ASPIRATE_STD);
				break;
			case en_SampleBlank:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE_BLANK);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_Sample:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_TestQcBlank:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_QC_BLANK);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_TestQc:
				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				UpdateTestStatusTextBuffer(ASPIRATE_QC);
//				UpdateTestScreenMenu();/*Update test screen menu / buttons*/
				break;
			case en_WashProcedure:
				break;
			default: break;
		}
		DrawRescaledGraph();
		}
		if( en_WinId_SelectQcLot == PrevWindow)
		{
			textConcentration.setText(" ");
			textAbsorbance.setText(" ");
			stcTestData.SampleBlankAbs = 0;
			stcTestData.SampleOpticalDensity = 0;
			stcTestData.Result = 0;

		}
		HideButtonBackground(false);
	}
	else
	{
		UpdateTestScreenMenu();/*Update test screen menu / buttons*/
	}

	/*UI Initialization - ENDS*/

	buttonWaterBlank.attachPush(HandlerButtonWaterBlank, &buttonWaterBlank);
	buttonReagentBlank.attachPush(HandlerButtonReagentBlank, &buttonReagentBlank);
	buttonStandard.attachPush(HandlerButtonStandard, &buttonStandard);
	buttonQc.attachPush(HandlerButtonQc, &buttonQc);
	buttonSampleRun.attachPush(HandlerButtonSampleRun, &buttonSampleRun);
	buttonWash.attachPush(HandlerButtonWash, &buttonWash);
	buttonAspirate.attachPush(HandlerButtonAspirate, &buttonAspirate);
	buttonParameter.attachPush(HandlerButtonParameter, &buttonParameter);
	buttonTimer.attachPush(HandlerButtonTimer, &buttonTimer);
	buttonShowOption.attachPush(HandlerButtonShowOptions, &buttonShowOption);
	buttonCloseOption.attachPush(HandlerButtonCloseOptions, &buttonCloseOption);
	buttonCalData.attachPush(HandlerButtonCalData, &buttonCalData);
	buttonQcData.attachPush(HandlerButtonQcData, &buttonQcData);
	buttonBackButton.attachPush(HandlerButtonBack, &buttonBackButton);
	buttonStd[0].attachPush(HandlerButtonStd0, &buttonStd[0]);
	buttonStd[1].attachPush(HandlerButtonStd1, &buttonStd[1]);
	buttonStd[2].attachPush(HandlerButtonStd2, &buttonStd[2]);
	buttonStd[3].attachPush(HandlerButtonStd3, &buttonStd[3]);
	buttonStd[4].attachPush(HandlerButtonStd4, &buttonStd[4]);
	buttonStd[5].attachPush(HandlerButtonStd5, &buttonStd[5]);
//	bParamsPrintButton.attachPush(HandlerButtonbParamsPrintButton, &bParamsPrintButton);
//	bParamsCloseButton.attachPush(HandlerButtonbParamsCloseButton, &bParamsCloseButton);
	bTestAbortPopupYes.attachPush(HandlerButtonbTestAbortPopupYes, &bTestAbortPopupYes);
	bTestAbortPopupNo.attachPush(HandlerButtonbTestAbortPopupNo, &bTestAbortPopupNo);
	bTestAbortPopupOK.attachPush(HandlerButtonbTestAbortPopupOK, &bTestAbortPopupOK);
	textPatientIDBox.attachPush(HandlerButtonPatientId, &textPatientIDBox);
	bTimerStartStop.attachPush(HandlerButtonbTimerStartStop, &bTimerStartStop);
	bTimerboxHide.attachPush(HandlerButtonbTimerboxHide, &bTimerboxHide);
	bTimerReset.attachPush(HandlerButtonbTimerReset, &bTimerReset);
	bTimerPlus.attachPush(HandlerButtonbTimerPlus, &bTimerPlus);

	bTimerOne.attachPush(HandlerButtonbTimerOne, &bTimerOne);
	bTimerTwo.attachPush(HandlerButtonbTimerTwo, &bTimerTwo);
	bTimerThree.attachPush(HandlerButtonbTimerThree, &bTimerThree);
	bTimerFour.attachPush(HandlerButtonbTimerFour, &bTimerFour);


	bTimerMinus.attachPush(HandlerButtonbTimerMinus, &bTimerMinus);
	pUserlogin.attachPush(HandlerUserlogin, &pUserlogin);
	ReDrawPrevGraph = false;
   return WinStatus;
}

void HandlerTestRunScreen (NexPage *ptr_obJCurrPage)
{
	static uint8_t TimerCnt = 0;

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	enWindowID CurrWindow = stcScreenNavigation.CurrentWindowId;
	enWindowID NextWindow = stcScreenNavigation.NextWindowId;
	if(Temp_warning)
	{
		if(GetInstance_TestParams()[stcTestData.TestId].Temperature - 0.5 > objstcTempControl[en_PhotometerTemp].CurrentTemp || GetInstance_TestParams()[stcTestData.TestId].Temperature + 0.5 < objstcTempControl[en_PhotometerTemp].CurrentTemp )
		{
			if(objstcTempControl[en_PhotometerTemp].CurrentTemp > 100)
			{
				objstcTempControl[en_PhotometerTemp].CurrentTemp = 0.0;
			}
			std::string CTemp = std::string(DoubleToCharConverter(objstcTempControl[en_PhotometerTemp].CurrentTemp  , 1));
			std::string TTemp = std::string(DoubleToCharConverter(GetInstance_TestParams()[stcTestData.TestId].Temperature  , 1));

			Temp_Err_Msg = "";
			Temp_Err_Msg += "Current Temp. " + CTemp + "\xB0"+"C, Target Temp. " + TTemp + "\xB0"+"C\\r";
			Temp_Err_Msg += "Temperature is not suitable for testing.\\rDo you want to continue?";

			enPopUpKeys Key = ShowMainPopUp("Flow Cell",Temp_Err_Msg, false);
			if(enKeyYes == Key)
			{
				ChangeWindowPage((enWindowID)CurrWindow , (enWindowID)NextWindow);
				Temp_warning = false;
				return;
			}
			else
			{
				ChangeWindowPage((enWindowID)PrevWindow , (enWindowID)CurrWindow);
				stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
				return;
			}
		}
		Temp_warning = false;
	}

	if(StatusBarUpdateMillsDelay())
	{
		/*Handler for aspiration button press*/
		if(2 <= TimerCnt)
		{
			TimerCnt = 0;

			UpdateStatusBar(&pTestTypePic , &textTestName , &textTestWavelength ,
					&textDate, &textTime, &textTemperature , &textIncuTemp, &textUserName);

			en_TestRunStatus Status = ReadTestRunStatus();
			if(enTestStatus_Idle == Status )
			{
			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest){
				buttonAspirate.Set_background_image_pic(BUTTON_ASPIRATE);
				buttonAspirate.setText("Start");
				}
				else{
				buttonAspirate.Set_background_image_pic(BUTTON_ASPIRATE);
				buttonAspirate.setText("Aspirate");
				}
			}
			else if(enTestStatus_InitError == Status || enTestStatus_DarkADCError == Status )
			{
				buttonAspirate.Set_background_image_pic(BUTTON_ABORT_DISABLE);
				buttonAspirate.setText("Error");
			}
			else if(enTestStatus_Wash == Status || enTestStatus_WaitingForNextStep == Status){
				buttonAspirate.Set_background_image_pic(BUTTON_ABORT_DISABLE);
				buttonAspirate.setText(" ");
			}
			else if(enTestStatus_Incubation == Status ||
					enTestStatus_PrimaryMeasurement  == Status ||
					enTestStatus_SecondaryMeasurement == Status)
			{
				buttonAspirate.Set_background_image_pic(BUTTON_ABORT);
				buttonAspirate.setText("Abort");
			}
			else if(enTestStatus_MoveFilterHome == Status ||
					enTestStatus_Initialize == Status ||
					enTestStatus_SelectDarkAdcFilter == Status ||
					enTestStatus_ReadDarkAdcPrimary == Status ||
					enTestStatus_ReadDarkAdcSecondary == Status ||
					enTestStatus_Aspiration == Status ||
					enTestStatus_AspirationAirGap == Status)
			{
				buttonAspirate.Set_background_image_pic(BUTTON_ASPIRATE_DISABLE);
				buttonAspirate.setText(" ");
			}
			else if (enTestStatus_SwitchPrimaryWavelength == Status ||
			enTestStatus_SwitchSecondaryWavelength == Status)
			{

			}
			else
			{
				buttonAspirate.Set_background_image_pic(BUTTON_ABORT_DISABLE);
				buttonAspirate.setText(" ");
			}
		}
		else
		{
			TimerCnt++;
		}
	}
	HandlerThreadPheripheral();
	WashProcedure_Task();
	nexLoop(nex_Listen_List);
}
void ShowHideInitializePopup(bool visible, std::string Title , std::string Msg)
{
	pInitPopup.setVisible(visible);
	tInitPopupTitle.setVisible(visible);
	tInitPopupMsg.setVisible(visible);

	tInitPopupTitle.setText(Title.c_str());
	tInitPopupMsg.setText(Msg.c_str());

}
void HideButtonBackground(bool visible)
{
	pInitbackgrd.setVisible(visible);
}
void UpdateTestStatusTextBuffer(std::string str)
{
	textTestStatusBuffer.setText(str.c_str());
}
/*Handler functions for Button - START*/
void HandlerButtonPatientId(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}

	if(en_Sample == stcTestData.CurrentMeasType ||
			en_SampleBlank == stcTestData.CurrentMeasType || en_CoagulationsampleIncubation == stcTestData.CurrentMeasType)
	{
		openKeyBoard(en_AlphaKeyboard , stcTestData.arrPatientIDBuffer , sizeof(stcTestData.arrPatientIDBuffer) , false ,
				"Enter Patient ID" , stcTestData.arrPatientIDBuffer , MAX_PATIENT_ID_LENGTH);
	}
	else if(en_TestQc == stcTestData.CurrentMeasType ||
			en_TestQcBlank == stcTestData.CurrentMeasType || en_CoagulationQCIncubation == stcTestData.CurrentMeasType)
	{
		ChangeWindowPage(en_WinId_SelectQcLot , (enWindowID)NULL);
	}

	BeepBuzzer();
}

void HandlerButtonWaterBlank(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}

	stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
	{
		UpdateTestStatusTextBuffer(COAG_DI_WATER);
	}
	else
	{
		UpdateTestStatusTextBuffer(ASPIRATE_DI_WATER);
	}
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonReagentBlank(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}

	if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
	{
		/*Disabling button*/
		return;
	}

	stcTestData.CurrentMeasType = en_ReagentBlank;
	UpdateTestStatusTextBuffer(ASPIRATE_REAGENT_BLANK);

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonStandard(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}

	if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
	{
		/*Disabled button*/
		return;
	}

//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 0;/*S0*/

	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Aspirate Std. Blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else
	{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Aspirate Std. %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
	}
//
//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonQc(void *ptr)
{

	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}
	if (InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
	{
		return; // Exit the function if all 4 slots are NVM_INIT_NOT_OK_FLAG
	}
	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_TestQcBlank;
		UpdateTestStatusTextBuffer(ASPIRATE_QC_BLANK);
	}
	else
	{
		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
		{
			stcTestData.CurrentMeasType = en_CoagulationQCIncubation;
			UpdateTestStatusTextBuffer(COAG_QC);
		}
		else
		{
			UpdateTestStatusTextBuffer(ASPIRATE_QC);
			stcTestData.CurrentMeasType = en_TestQc;
		}

	}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonSampleRun(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}

	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_SampleBlank;
		UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE_BLANK);
	}
	else
	{
		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
		{
			stcTestData.CurrentMeasType = en_CoagulationsampleIncubation;
			UpdateTestStatusTextBuffer(COAG_SAMPLE);
		}
		else
		{
			UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE);
			stcTestData.CurrentMeasType = en_Sample;
		}

	}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonWash(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}

//	enMeasurementType PrevMeas = stcTestData.CurrentMeasType;
//	stcTestData.CurrentMeasType = en_WashProcedure;

	AspSwitchLed_Red(en_AspLedON);
	AspSwitchLed_White(en_AspLedOFF);

	UpdateTestStatusTextBuffer(WASH_MSG);

	buttonWaterBlank.Set_background_image_pic(PIC_DESELECTED_WATER_BLANK);

	if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
	{
		buttonReagentBlank.Set_background_image_pic(PIC_DISABLED_REG_BLANK);
	}
	else{
		buttonReagentBlank.Set_background_image_pic(PIC_DESELECTED_REG_BLANK);
	}

	if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
	{
		buttonStandard.Set_background_image_pic(PIC_DISABLED_STD);
	}
	else{
		buttonStandard.Set_background_image_pic(PIC_DESELECTED_STD);
	}
	if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
	{
		buttonQc.Set_background_image_pic(PIC_DISABLED_QC);
	}
	else
	{
		buttonQc.Set_background_image_pic(PIC_DESELECTED_QC);
	}
	buttonSampleRun.Set_background_image_pic(PIC_DESELECTED_SAMPLE);
	buttonWash.Set_background_image_pic(PIC_SELECTED_WASH);
	Manual_Wash = true;
	BeepBuzzer();
	Start_Wash();

//	stcTestData.CurrentMeasType = PrevMeas;
}

bool VerifyForBlanksPerformed(void)
{
	bool TestRunValidity = true;
	enWindowID CurrWindow = stcScreenNavigation.CurrentWindowId;
	if(en_WinId_TestRun != CurrWindow)
	{
		return TestRunValidity = false;/*return false if current screen is not test run screen*/
	}
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.NextWindowId;
	/*Check for basic procedure has been done or not*/
	if(en_ReagentBlank == stcTestData.CurrentMeasType ||
			en_Reagent == stcTestData.CurrentMeasType)
	{
		if(false == stcTestData.WaterBlankDoneFlag)/*If water blank test not done -> show errror popup*/
		{
			if(enkeyOk == ShowMainPopUp("Warning","Please perform the water blank test", true))
			{
				ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
				stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
			}
			TestRunValidity = false;
		}
	}
	else if(en_Sample == stcTestData.CurrentMeasType || en_SampleBlank == stcTestData.CurrentMeasType ||
			en_Standard == stcTestData.CurrentMeasType || en_StandardBlank == stcTestData.CurrentMeasType ||
			en_TestQc == stcTestData.CurrentMeasType || en_TestQcBlank == stcTestData.CurrentMeasType ||
			en_CoagulationsampleIncubation == stcTestData.CurrentMeasType || en_CoagulationQCIncubation == stcTestData.CurrentMeasType)
	{
		/*If water blank test not done -> show errror popup*/
		if(false == stcTestData.WaterBlankDoneFlag)
		{
			TestRunValidity = false;
			if(enkeyOk == ShowMainPopUp("Warning","Please perform the water blank test", true))
			{
				ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
				stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
			}
		}
		/*If Reagent blank test is enabled and not done -> show error popup*/
		else if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable && false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag)
		{
			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag/*Reagent*/ )
			{
				TestRunValidity = false;
				if(enkeyOk == ShowMainPopUp("Warning","Please perform the Reagent Blank test", true))
				{
					ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
					stcTestData.CurrentMeasType = en_ReagentBlank;
				}
			}
			/*If Reagent blank and Sample blank is enabled and not done -> show error popup*/
			else if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
					false == GetInstance_TestParams()[stcTestData.TestId].ReagentDoneFlag/*Reagent*/ )
			{
				TestRunValidity = false;
				if(enkeyOk == ShowMainPopUp("Warning","Please perform the Reagent test", true))
				{
					ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
					stcTestData.CurrentMeasType = en_Reagent;

				}
			}

		}
		else if(0 != GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved &&
				(stcTestData.CurrentMeasType == en_Sample || stcTestData.CurrentMeasType == en_SampleBlank || stcTestData.CurrentMeasType == en_TestQcBlank || stcTestData.CurrentMeasType == en_TestQc))
		{
			uint8_t NumOfStdSaved = 0;
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			for(uint8_t nI = 0 ; nI < NumOfStdSaved ; nI++)
			{
				if(e_RunTimeStdOd[nI] == 0)
				{
					TestRunValidity = false;
					if(enkeyOk == ShowMainPopUp("Warning","Please perform the Standard test", true))
					{
						if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
						{
							stcTestData.CurrentMeasType = en_StandardBlank;
							stcTestData.SelectedStandard = 0;/*S0*/
						}
						else
						{
							stcTestData.CurrentMeasType = en_Standard;
							stcTestData.SelectedStandard = 0;/*S0*/
						}
						ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
						break;
					}
				}
			}
		}
	}
	/*Continue initilization of UI if test run validity flag is TRUE*/
	if(true == TestRunValidity)
	{
		buttonAspirate.Set_background_image_pic(BUTTON_ASPIRATE_DISABLE);
		buttonAspirate.setText(" ");
	}
	return TestRunValidity;
}
/*This function will be called when software (UI) aspiration switch is pressed*/
void HandlerButtonAspirate(void *ptr)
{
	HandlerAspButton();
}
void HandlerButtonParameter(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	BeepBuzzer();
	ShowTestParamsViewScreen();
//	ShowHideTestParamsDataBox(true);
}

void HandlerButtonShowOptions(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	BeepBuzzer();
	ShowHideOptionsBox(true);
}
void HandlerButtonCloseOptions(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	BeepBuzzer();
	ShowHideOptionsBox(false);
}

void HandlerButtonCalData(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	ShowTestCalibScreen();
	BeepBuzzer();
}

void HandlerButtonQcData(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if (InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
	{
		InstrumentBusyBuzz();
		return;
	}
	ShowTestQCSummaryScreen();
	BeepBuzzer();
}
void HandlerButtonBack(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_DarkADCError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}
	if(stcTimerBlock[0].bTimerRunFlag != true && stcTimerBlock[1].bTimerRunFlag != true &&
			stcTimerBlock[2].bTimerRunFlag != true && stcTimerBlock[3].bTimerRunFlag != true)
	{
		InitilizeTimerBlock();
	}
	SelectFilter(en_FilterBlack);
	ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestRun);
	stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
	LastTestId = stcTestData.TestId;
	BeepBuzzer();
}

void HandlerButtonStd0(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 0;/*S0*/

	if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Please aspirate standard blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else
	{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Please aspirate standard %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
	}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonStd1(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 1;/*S1*/

	if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Please aspirate standard blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else
	{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Please aspirate standard %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
	}
//	HAL_Delay(100);
//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonStd2(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 2;/*S2*/

	if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Please aspirate standard blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Please aspirate standard %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
		}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonStd3(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 3;/*S3*/

	if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Please aspirate standard blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Please aspirate standard %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
		}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonStd4(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 4;/*S4*/

	if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Please aspirate standard blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Please aspirate standard %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
		}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
void HandlerButtonStd5(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
//	char arrBuff[64] = {0};
	stcTestData.SelectedStandard = 5;/*S5*/

	if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		stcTestData.CurrentMeasType = en_StandardBlank;
//		snprintf(arrBuff , 63 , "Please aspirate standard blank %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
	}
	else{
		stcTestData.CurrentMeasType = en_Standard;
//		snprintf(arrBuff , 63 , "Please aspirate standard %d" , stcTestData.SelectedStandard + 1);
		UpdateTestStatusTextBuffer(ASPIRATE_STD);
		}

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);
	UpdateTestScreenMenu();
	DrawRescaledGraph();
	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	BeepBuzzer();
}
//void HandlerButtonbParamsCloseButton(void *ptr)
//{
//	en_TestRunStatus Status = ReadTestRunStatus();
//	if((enTestStatus_Idle != Status &&
//			enTestStatus_InitError != Status &&
//			enTestStatus_SelectDarkAdcFilter != Status &&
//			enTestStatus_ReadDarkAdcPrimary != Status &&
//			enTestStatus_ReadDarkAdcSecondary != Status &&
//			enTestStatus_SwitchPrimaryWavelength != Status &&
//			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
//	{
//		InstrumentBusyBuzz();
//		return;
//	}
//	BeepBuzzer();
//	ShowHideTestParamsDataBox(false);
//}
//void HandlerButtonbParamsPrintButton(void *ptr)
//{
//	en_TestRunStatus Status = ReadTestRunStatus();
//	if((enTestStatus_Idle != Status &&
//			enTestStatus_InitError != Status &&
//			enTestStatus_SelectDarkAdcFilter != Status &&
//			enTestStatus_ReadDarkAdcPrimary != Status &&
//			enTestStatus_ReadDarkAdcSecondary != Status &&
//			enTestStatus_SwitchPrimaryWavelength != Status &&
//			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
//	{
//		InstrumentBusyBuzz();
//		return;
//	}
//
//	PrintTestParams(stcTestData.TestId);
//	BeepBuzzer();
//}
/*Handler functions for Button - END*/

void UpdateAbsBuffer(std::string abs)
{
	textAdcAbsNameBoard.setVisible(true);
	textAbsorbance.setVisible(true);
	textAbsorbance.setText(abs.c_str());
}

void UpdateConentrationBuffer(std::string conc)
{
	textConcentration.setVisible(true);
	textConcNameBoard.setVisible(true);
	textConcentration.setText(conc.c_str());
}

void UpdateTestScreenMenu(void)
{
	float MaxXVal = 0;
	float MaxYVal = 0;
	float MinXVal = 0;
	float MinYVal = 0;
	float GraphLineXpoint = 0;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8TestQCIdx;
	char arrBuffer[24] = {0};
	if(en_Standard != stcTestData.CurrentMeasType && en_StandardBlank != stcTestData.CurrentMeasType)
	{
		for(uint8_t nI = 0 ; nI < MAX_NUM_OF_STANDARDS ; ++nI)
		{
			buttonStd[nI].setVisible(false);
		}
	}

	if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
	{
		buttonWash.setVisible(false);
		pWash_topline.setVisible(false);
	}
	else
	{
		buttonWash.setVisible(true);
		pWash_topline.setVisible(true);
	}

	switch(stcTestData.CurrentMeasType)
	{
		case en_DIWaterAdcBlank:
			/*Displaying current measurement status*/
			tADC_Abs.Set_background_image_pic(PIC_TXT_ADC);
			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_background_image_pic(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_background_image_pic(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_background_image_pic(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_background_image_pic(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_background_image_pic(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_background_image_pic(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_background_image_pic(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_background_image_pic(PIC_DESELECTED_WASH);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_press_background_image_pic2(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_press_background_image_pic2(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_press_background_image_pic2(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_press_background_image_pic2(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_press_background_image_pic2(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_press_background_image_pic2(PIC_DESELECTED_WASH);
			buttonWaterBlank.Set_background_image_pic(PIC_SELECTED_WATER_BLANK);
			buttonWaterBlank.Set_press_background_image_pic2(PIC_SELECTED_WATER_BLANK);

			GraphLineXpoint = INCUBATION_TIME_FOR_WATER_BLANK_SEC;
			MaxXVal = 0;
			MaxYVal = MAX_ADC_VALUE;
			MinXVal = 0;
			MinYVal = 0;

			textAdcAbsNameBoard.setText("Pri ADC");
			textConcNameBoard.setText("Sec ADC");
			textPatientIDNameBoard.setVisible(false);
			textPatientIDBox.setVisible(false);
			textStdFactorNameBoard.setVisible(false);
			textStdFactor.setVisible(false);
			textUnitNameBoard.setVisible(false);
			textUnit.setVisible(false);
			if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
			{
				textConcentration.setVisible(true);
				textConcNameBoard.setVisible(true);
				MaxXVal = (INCUBATION_TIME_FOR_WATER_BLANK_SEC + (2 * MEASUREMENT_TIME_FOR_WATER_BLANK_SEC));
			}
			else
			{
				textConcentration.setVisible(false);
				textConcNameBoard.setVisible(false);
				MaxXVal = (INCUBATION_TIME_FOR_WATER_BLANK_SEC + MEASUREMENT_TIME_FOR_WATER_BLANK_SEC);
			}
			break;
		case en_ReagentBlank:
		case en_Reagent:
			/*Displaying current measurement status*/
			tADC_Abs.Set_background_image_pic(PIC_TXT_ABS);
			buttonWaterBlank.Set_background_image_pic(PIC_DESELECTED_WATER_BLANK);

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_background_image_pic(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_background_image_pic(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_background_image_pic(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_background_image_pic(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_background_image_pic(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_background_image_pic(PIC_DESELECTED_WASH);
			buttonWaterBlank.Set_press_background_image_pic2(PIC_DESELECTED_WATER_BLANK);
			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_press_background_image_pic2(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_press_background_image_pic2(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_press_background_image_pic2(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_press_background_image_pic2(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_press_background_image_pic2(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_press_background_image_pic2(PIC_DESELECTED_WASH);

			textPatientIDNameBoard.setVisible(false);
			textPatientIDBox.setVisible(false);
			textConcentration.setVisible(false);
			textConcNameBoard.setVisible(false);
			textStdFactorNameBoard.setVisible(false);
			textStdFactor.setVisible(false);
			textUnitNameBoard.setVisible(false);
			textUnit.setVisible(false);

			buttonReagentBlank.Set_background_image_pic(PIC_SELECTED_REG_BLANK);
			buttonReagentBlank.Set_press_background_image_pic2(PIC_SELECTED_REG_BLANK);
			textAdcAbsNameBoard.setText("Abs.");

			GraphLineXpoint = GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec;
			MaxXVal = 0;
			MaxYVal = 3;
			MinXVal = 0;
			MinYVal = (-1);
			if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
			{
				MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
						( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
			}
			else
			{
				MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
							GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
			}
			break;

		case en_Standard:
		case en_StandardBlank:
			/*Displaying current measurement status*/
			tADC_Abs.Set_background_image_pic(PIC_TXT_ABS);
			buttonWaterBlank.Set_background_image_pic(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_background_image_pic(PIC_DISABLED_REG_BLANK);
			}
			else
			{
				buttonReagentBlank.Set_background_image_pic(PIC_DESELECTED_REG_BLANK);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_background_image_pic(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_background_image_pic(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_background_image_pic(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_background_image_pic(PIC_DESELECTED_WASH);

			buttonWaterBlank.Set_press_background_image_pic2(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DESELECTED_REG_BLANK);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_press_background_image_pic2(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_press_background_image_pic2(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_press_background_image_pic2(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_press_background_image_pic2(PIC_DESELECTED_WASH);

			textPatientIDNameBoard.setVisible(false);
			textPatientIDBox.setVisible(false);

			textAdcAbsNameBoard.setText("Abs.");
			textConcNameBoard.setText("Conc.");
			textConcentration.setVisible(true);
			textConcNameBoard.setVisible(true);

			textStdFactorNameBoard.setText("Std.");
			textStdFactorNameBoard.setVisible(true);
			textStdFactor.setVisible(true);

			snprintf(arrBuffer , 23 , "%u",(unsigned int)stcTestData.SelectedStandard + 1);
			textStdFactor.setText(arrBuffer);

			textUnitNameBoard.setText("Unit");
			textUnitNameBoard.setVisible(true);
			textUnit.setVisible(true);

			if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
			{
				textUnit.setText(GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
			}
			else
			{
				textUnit.setText(&arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
			}
			for(uint8_t nI = 0 ;
					nI < GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved ; ++nI)
			{
				buttonStd[nI].setVisible(true);
			}
			buttonStandard.Set_background_image_pic(PIC_SELECTED_STD);
			buttonStandard.Set_press_background_image_pic2(PIC_SELECTED_STD);
			MaxXVal = 0;
			MaxYVal = 3;
			MinXVal = 0;
			MinYVal = (-1);

			GraphLineXpoint = GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec;
			if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
			{
				MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
						( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
			}
			else
			{
				MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
							GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
			}

			for(uint8_t nI = 0 ; nI < MAX_NUM_OF_STANDARDS ; ++nI)
			{
				buttonStd[nI].Set_background_image_pic(PIC_DESELECTED_STANDARD);
				buttonStd[nI].Set_font_color_pco(COLOUR_BLACK);
			}
			buttonStd[stcTestData.SelectedStandard].Set_background_image_pic(PIC_SELECTED_STANDARD);
			buttonStd[stcTestData.SelectedStandard].Set_font_color_pco(COLOUR_WHITE);
			/*Need to update the standard conc and abs */
			UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
			break;
		case en_SampleBlank:
		case en_Sample:
		case en_CoagulationsampleIncubation:
//			textConcNameBoard.setText("Concentration");
			/*Displaying current measurement status*/
			tADC_Abs.Set_background_image_pic(PIC_TXT_ABS);
			buttonWaterBlank.Set_background_image_pic(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_background_image_pic(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_background_image_pic(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_background_image_pic(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_background_image_pic(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_background_image_pic(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_background_image_pic(PIC_DESELECTED_QC);
			}
			buttonWash.Set_background_image_pic(PIC_DESELECTED_WASH);

			buttonWaterBlank.Set_press_background_image_pic2(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_press_background_image_pic2(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_press_background_image_pic2(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_press_background_image_pic2(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_press_background_image_pic2(PIC_DESELECTED_QC);
			}
			buttonWash.Set_press_background_image_pic2(PIC_DESELECTED_WASH);
			textAdcAbsNameBoard.setText("Abs.");
			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
			{
				textConcNameBoard.setText("Second");
			}
			else
			{
				textConcNameBoard.setText("Result");
			}
			textConcentration.setVisible(true);
			textConcNameBoard.setVisible(true);

			textStdFactorNameBoard.setText("Unit");
			textStdFactorNameBoard.setVisible(true);
			textStdFactor.setVisible(true);
			if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
			{
				textStdFactor.setText(GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
			}
			else
			{
				textStdFactor.setText(&arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
			}

			if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_Factor || GetInstance_TestParams()[stcTestData.TestId].Calib ==	en_1PointLinear)
			{
				textUnitNameBoard.setText("Factor");
				textUnitNameBoard.setVisible(true);
				textUnit.setVisible(true);
				snprintf(arrBuffer , 23 , "%.03f",GetInstance_TestParams()[stcTestData.TestId].KFactor);
				textUnit.setText(arrBuffer);
			}
			else
			{
				textUnitNameBoard.setVisible(false);
				textUnit.setVisible(false);
			}
			textPatientIDNameBoard.setText("Patient ID");
			textPatientIDNameBoard.setVisible(true);
			textPatientIDBox.setVisible(true);
			buttonSampleRun.Set_background_image_pic(PIC_SELECTED_SAMPLE);
			buttonSampleRun.Set_press_background_image_pic2(PIC_SELECTED_SAMPLE);


			textPatientIDBox.setText(stcTestData.arrPatientIDBuffer);

			MaxXVal = 0;
			MaxYVal = 3;
			MinXVal = 0;
			MinYVal = (-1);


			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
			{
				GraphLineXpoint = 1 ;
				MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
			}
			else
			{
				GraphLineXpoint = GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec;
				if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
				{
					MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
							( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
				}
				else
				{
					MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
								GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
				}
			}

			break;
		case en_TestQcBlank:
		case en_TestQc:
		case en_CoagulationQCIncubation:

//			textConcNameBoard.setText("Concentration");
			/*Displaying current measurement status*/
			tADC_Abs.Set_background_image_pic(PIC_TXT_ABS);
			buttonWaterBlank.Set_background_image_pic(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_background_image_pic(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_background_image_pic(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_background_image_pic(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_background_image_pic(PIC_DESELECTED_STD);
			}

			buttonSampleRun.Set_background_image_pic(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_background_image_pic(PIC_DESELECTED_WASH);

			buttonWaterBlank.Set_press_background_image_pic2(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_press_background_image_pic2(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_press_background_image_pic2(PIC_DESELECTED_STD);
			}

			buttonSampleRun.Set_press_background_image_pic2(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_press_background_image_pic2(PIC_DESELECTED_WASH);

			textAdcAbsNameBoard.setText("Abs.");
			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
			{
				textConcNameBoard.setText("Second");
			}
			else
			{
				textConcNameBoard.setText("Result");
			}
			textConcentration.setVisible(true);
			textConcNameBoard.setVisible(true);

			textStdFactorNameBoard.setText("Unit");
			textStdFactorNameBoard.setVisible(true);
			textStdFactor.setVisible(true);
			if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
			{
				textStdFactor.setText(GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
			}
			else
			{
				textStdFactor.setText(&arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
			}

			if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_Factor || GetInstance_TestParams()[stcTestData.TestId].Calib ==	en_1PointLinear)
			{
				textUnitNameBoard.setText("Factor");
				textUnitNameBoard.setVisible(true);
				textUnit.setVisible(true);
				snprintf(arrBuffer , 23 , "%.03f",GetInstance_TestParams()[stcTestData.TestId].KFactor);
				textUnit.setText(arrBuffer);
			}
			else
			{
				textUnitNameBoard.setVisible(false);
				textUnit.setVisible(false);
			}

			textPatientIDBox.setVisible(true);
			textPatientIDNameBoard.setText("Lot No");
			textPatientIDNameBoard.setVisible(true);
			buttonQc.Set_background_image_pic(PIC_SELECTED_QC);
			buttonQc.Set_press_background_image_pic2(PIC_SELECTED_QC);
//			buttonQcData.setVisible(true);
			u8TestQCIdx = GetSelectedTestQcIdx();
			if(NVM_INIT_NOT_OK_FLAG != u8TestQCIdx)
			{
				snprintf(arrBuffer , 23 , "%s-%s",m_QcSetup->m_QcControls[u8TestQCIdx].arrControlName, m_QcSetup->m_QcControls[u8TestQCIdx].arRLotNum);
				textPatientIDBox.setText(arrBuffer);
			}
			else
			{
				textPatientIDBox.setText("");
			}
			MaxXVal = 0;
			MaxYVal = 3;
			MinXVal = 0;
			MinYVal = (-1);

			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
			{
				GraphLineXpoint = 1 ;
				MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
			}
			else
			{
				GraphLineXpoint = GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec;
				if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
				{
					MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
							( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
				}
				else
				{
					MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
								GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
				}
			}
			break;
		case en_WashProcedure:
			buttonWaterBlank.Set_background_image_pic(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_background_image_pic(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_background_image_pic(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_background_image_pic(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_background_image_pic(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_background_image_pic(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_background_image_pic(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_background_image_pic(PIC_DESELECTED_SAMPLE);
			buttonWaterBlank.Set_press_background_image_pic2(PIC_DESELECTED_WATER_BLANK);

			if(false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)/*Show error popup if reagent blank is not enabled*/
			{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DISABLED_REG_BLANK);
			}
			else{
				buttonReagentBlank.Set_press_background_image_pic2(PIC_DESELECTED_REG_BLANK);
			}

			if(0 == GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)/*Show error popup if reagent blank is not enabled*/
			{
				buttonStandard.Set_press_background_image_pic2(PIC_DISABLED_STD);
			}
			else{
				buttonStandard.Set_press_background_image_pic2(PIC_DESELECTED_STD);
			}
			if(InvalidQCcount == MAX_NUM_OF_QC_CONTROL_PER_TEST)
			{
				buttonQc.Set_press_background_image_pic2(PIC_DISABLED_QC);
			}
			else
			{
				buttonQc.Set_press_background_image_pic2(PIC_DESELECTED_QC);
			}
			buttonSampleRun.Set_press_background_image_pic2(PIC_DESELECTED_SAMPLE);
			buttonWash.Set_background_image_pic(PIC_SELECTED_WASH);
			buttonWash.Set_press_background_image_pic2(PIC_SELECTED_WASH);
			textAdcAbsNameBoard.setText("Abs.");
			break;
		default: break;
	}
	if(Manual_Wash == false)
	{
		ClearGraph(en_WinId_TestRun /*Window ID*/,
								TEST_GRAPH_PIXEL_MIN_X - 5 ,
								TEST_GRAPH_PIXEL_MIN_Y - 5,
								TEST_GRAPH_PIXEL_X_WIDTH + 10,
								TEST_GRAPH_PIXEL_Y_HEIGHT + 5);
		HAL_Delay(5);
		pGraphBG.setPic(PIC_GRAPH);
		HAL_Delay(5);
		UpdateGraphAxisData(&textMainGraphXMax /*X Max*/, &textMainGraphYMax /*Y Max*/,
				MaxXVal,
				MaxYVal,//3/*Y max value*/,
				&textMainGraphXMin /*X Min*/,
				&textMainGraphYMin /*Y Min*/,
				1/*X max value*/,
				MinYVal/*Y max value*/);
		HAL_Delay(50);
		DrawVerticalLineOnGraph(en_WinId_TestRun /*Window ID*/,	(GraphLineXpoint - 1),
				MinXVal/*X min value*/ , (MaxXVal - 1)/*X max value*/);

	}
	else
	{
		Manual_Wash = false;
	}
}

void ShowHideOptionsBox(bool flag)
{
	pOptionsBG.setVisible(flag);
	buttonCloseOption.setVisible(flag);
	buttonParameter.setVisible(flag);
	buttonCalData.setVisible(flag);
	buttonQcData.setVisible(flag);
}

void ShowHideTestParamsDataBox(bool flag)
{
	if(flag)
	{
		buttonShowOption.setVisible(false);
		ShowHideOptionsBox(false);
		pParamsBox.setVisible(flag);/*Hide Test parameter data window*/
	}
	else
	{
		pParamsBox.setVisible(flag);/*Hide Test parameter data window*/
		ShowHideOptionsBox(false);
		buttonShowOption.setVisible(true);
	}
	bParamsPrintButton.setVisible(flag);
	bParamsCloseButton.setVisible(flag);
	textParamsName.setVisible(flag);
	textParamsUnits.setVisible(flag);
	textParamsReagentBlank.setVisible(flag);
	textParamsSampleBlank.setVisible(flag);
	textParamsMethod.setVisible(flag);
	textParamsPrimaryFilter.setVisible(flag);
	textParamsSecondaryFilter.setVisible(flag);
	textParamsTemperature.setVisible(flag);
	textParamsAspVol.setVisible(flag);
	textParamsLagTimeS.setVisible(flag);
	textParamsReadTimeS.setVisible(flag);
	textParamsCuvetteType.setVisible(flag);
	textParamsRefRange.setVisible(flag);
	textParamsLinearityRange.setVisible(flag);
	textParamsFactor.setVisible(flag);
	textParamsCalibrartion.setVisible(flag);
	textParamsStandards.setVisible(flag);

	for(uint8_t nI = 0 ; nI < MAX_TEST_PARAMS_DATA_STATIC_TEXT_FEILDS ; ++nI)
	{
		textparamStaticTexts[nI].setVisible(flag);
	}
	if(flag)
	{
		textParamsName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
		if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
		{
			textParamsUnits.setText(GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
		}
		else
		{
			textParamsUnits.setText(&arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
		}

		if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
		{
			textParamsSampleBlank.setText("Yes");
		}
		else//en_BlankTypeReagentBlank :  if(en_ReagentBlank == GetInstance_TestParams()[stcTestData.TestId].BlankType)
		{
			textParamsSampleBlank.setText("No");
		}

		if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
		{
			textParamsReagentBlank.setText("Yes");
		}
		else//en_BlankTypeReagentBlank :  if(en_ReagentBlank == GetInstance_TestParams()[stcTestData.TestId].BlankType)
		{
			textParamsReagentBlank.setText("No");
		}

		textParamsMethod.setText(&g_arrTestmethodnameBuffer[GetInstance_TestParams()[stcTestData.TestId].TestProcedureType][0]);
		textParamsPrimaryFilter.setText(&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].PrimWavelength][0]);
		textParamsSecondaryFilter.setText(&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].SecWavelength][0]);
		char arrBuffer[24] = {0};

		if(ROOM_TEMP_VAL == GetInstance_TestParams()[stcTestData.TestId].Temperature)
		{
			snprintf(arrBuffer , 23 , "%s","Disabled");
		}
		else
		{
			snprintf(arrBuffer , 23 , "%.02f",GetInstance_TestParams()[stcTestData.TestId].Temperature);
		}
		textParamsTemperature.setText(arrBuffer);

		snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].AspVoleume);
		textParamsAspVol.setText(arrBuffer);
		snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);
		textParamsLagTimeS.setText(arrBuffer);
		snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
		textParamsReadTimeS.setText(arrBuffer);
		textParamsCuvetteType.setText(&g_arrCuvveteNameBuffer[GetInstance_TestParams()[stcTestData.TestId].CuvetteType][0]);
		snprintf(arrBuffer , 23 , "%.03f - %.03f",GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[0],
				GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[1]);
		textParamsRefRange.setText(arrBuffer);
		snprintf(arrBuffer , 23 , "%.03f - %.03f",GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0],
				GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1]);
		textParamsLinearityRange.setText(arrBuffer);
		snprintf(arrBuffer , 23 , "%.03f",GetInstance_TestParams()[stcTestData.TestId].KFactor);
		textParamsFactor.setText(arrBuffer);
		snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
		textParamsStandards.setText(arrBuffer);
		textParamsCalibrartion.setText(&g_CalibrationTypeNameBUffer[GetInstance_TestParams()[stcTestData.TestId].Calib][0]);
	}
	else/*if closes test params screen - show graph again*/
	{
//		ClearGraph(en_WinId_TestRun /*Window ID*/,
//								TEST_GRAPH_PIXEL_MIN_X ,
//								TEST_GRAPH_PIXEL_MIN_Y,
//								TEST_GRAPH_PIXEL_X_WIDTH,
//								TEST_GRAPH_PIXEL_Y_HEIGHT);
		UpdateTestScreenMenu();
		DrawRescaledGraph();
		UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	}
}

void ShowHideTestScreenPopup(en_TestPopUp Popup , std::string Msg)
{
	bool Flag = true;
	g_en_TestPopUp = Popup;
	switch((uint8_t)Popup)
	{
		case enTestPopUp_Abort:
			bTestAbortPopupOK.setVisible(false);
			bTestAbortPopupYes.setVisible(true);
			bTestAbortPopupNo.setVisible(true);
			{
			std::string Msg1 = " ";
			tTestAbortPopupMsg1.setVisible(false);
			tTestAbortPopupMsg1.setText(Msg1.c_str());
			std::string Msg2 = "Do you want to stop the run?";
			tTestAbortPopupMsg2.setVisible(true);
			tTestAbortPopupMsg2.setText(Msg2.c_str());
			std::string Msg3 = " ";
			tTestAbortPopupMsg3.setVisible(false);
			tTestAbortPopupMsg3.setText(Msg3.c_str());
			}
			break;
		case enTestPopUp_Error:
			bTestAbortPopupOK.setVisible(true);
			bTestAbortPopupYes.setVisible(false);
			bTestAbortPopupNo.setVisible(false);
			{
			std::string Msg1 = " ";
			tTestAbortPopupMsg1.setVisible(false);
			tTestAbortPopupMsg1.setText(Msg1.c_str());
			std::string Msg2 = "Please restart the device";
			tTestAbortPopupMsg2.setVisible(true);
			tTestAbortPopupMsg2.setText(Msg2.c_str());
			std::string Msg3 = " ";
			tTestAbortPopupMsg3.setVisible(false);
			tTestAbortPopupMsg3.setText(Msg3.c_str());
			}
		case enTestPopUp_DarkADCError:
			bTestAbortPopupOK.setVisible(true);
			bTestAbortPopupYes.setVisible(false);
			bTestAbortPopupNo.setVisible(false);
			{
			std::string Msg1 = " ";
			tTestAbortPopupMsg1.setVisible(false);
			tTestAbortPopupMsg1.setText(Msg1.c_str());
			std::string Msg2 = "Optical light path error";
			tTestAbortPopupMsg2.setVisible(true);
			tTestAbortPopupMsg2.setText(Msg2.c_str());
			std::string Msg3 = " ";
			tTestAbortPopupMsg3.setVisible(false);
			tTestAbortPopupMsg3.setText(Msg3.c_str());
			}
		break;
		case enTestPopUp_fitlerwheelError:
			bTestAbortPopupOK.setVisible(true);
			bTestAbortPopupYes.setVisible(false);
			bTestAbortPopupNo.setVisible(false);
			{
			std::string Msg1 = " ";
			tTestAbortPopupMsg1.setVisible(false);
			tTestAbortPopupMsg1.setText(Msg1.c_str());
			std::string Msg2 = "Home Position Detection Error";
			tTestAbortPopupMsg2.setVisible(true);
			tTestAbortPopupMsg2.setText(Msg2.c_str());
			std::string Msg3 = " ";
			tTestAbortPopupMsg3.setVisible(false);
			tTestAbortPopupMsg3.setText(Msg3.c_str());
			}
		break;
		case enTestPopUp_Null:
		default:
			bTestAbortPopupOK.setVisible(false);
			bTestAbortPopupYes.setVisible(false);
			bTestAbortPopupNo.setVisible(false);
			std::string Msg1 = " ";
			tTestAbortPopupMsg1.setVisible(false);
			tTestAbortPopupMsg1.setText(Msg1.c_str());
			std::string Msg2 = " ";
			tTestAbortPopupMsg2.setVisible(false);
			tTestAbortPopupMsg2.setText(Msg2.c_str());
			std::string Msg3 = " ";
			tTestAbortPopupMsg3.setVisible(false);
			tTestAbortPopupMsg3.setText(Msg3.c_str());
			Msg = " ";
			Flag = false;
			e_limit_error_popup = false;
			e_reagent_blank_limit_flag = false;
			e_abs_limit_flag = false;
			e_delAmin_limit_flag = false;
			break;
	};
	/*Pop up title msg from fucniton call*/
	pTestAbortpopup.setVisible(Flag);
	tTestAbortPopuptitle.setVisible(Flag);
	tTestAbortPopuptitle.setText(Msg.c_str());
}

void HandlerButtonbTestAbortPopupYes(void *ptr)
{
	switch((uint8_t)g_en_TestPopUp)
	{
		case enTestPopUp_Abort:
			BeepBuzzer();
			buttonAspirate.Set_background_image_pic(BUTTON_ABORT_DISABLE);
			buttonAspirate.setText(" ");
			ShowHideTestScreenPopup(enTestPopUp_Null , std::string(" "));
			AbortTestRun();
			break;
		case enTestPopUp_Error:
		break;
		case enTestPopUp_Null:
		default:
			break;
	};

}
void HandlerButtonbTestAbortPopupNo(void *ptr)
{
	BeepBuzzer();
	ShowHideTestScreenPopup(enTestPopUp_Null , std::string(" "));
}
void HandlerButtonbTestAbortPopupOK(void *ptr)
{
	BeepBuzzer();
	ShowHideTestScreenPopup(enTestPopUp_Null , std::string(" "));
}

void ShowResultScreen(void)
{
	ChangeWindowPage(en_WinId_TestResultScreen , en_WinId_TestRun);
}
void ShowTestCalibScreen(void)
{
	ChangeWindowPage(en_WinId_TestCalibScreen , en_WinId_TestRun);
}

void ShowTestParamsViewScreen(void)
{
	ChangeWindowPage(en_WinId_TestParamsView , en_WinId_TestRun);
}

void ShowQCResLJPlotScreen(void)
{
	ChangeWindowPage(en_WinID_LJ_Plot , en_WinId_TestRun);
}
void ShowTestTimerScreen(void)
{
//	ChangeWindowPage(en_WinId_TestTimerScreen , en_WinId_TestRun);
}
void ShowTestQCSummaryScreen(void)
{
	uint8_t u8TestQcIdx = GetSelectedTestQcIdx();

	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.NextWindowId;
	if(NVM_INIT_NOT_OK_FLAG == u8TestQcIdx)
	{
		if(enkeyOk == ShowMainPopUp("Quality Control","Please select a valid QC", true))
		{
			ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
		}
	}
	else
	{
		ChangeWindowPage(en_WinID_LJ_Plot , en_WinId_TestRun);
	}
}

void HandlerSaveTestParams(void)/*show Saving Please wait popups*/
{
	WriteTestparameterBuffer();/*Writing tets params to memory*/
}
void HandlerAspButton(void)
{
	en_TestRunStatus Status = enTestStatus_Idle;
	if(en_WashProcess_Idle != Get_WashStatus())
	{
		InstrumentBusyBuzz();
		return;
	}
	if(ReDrawPrevGraph == true)
	{
		InstrumentBusyBuzz();
		return;
	}

	Status = ReadTestRunStatus();
	if(enTestStatus_Idle == Status)
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.NextWindowId;
		/*Check if patient id or lot num is selected*/
		if(en_Sample == stcTestData.CurrentMeasType ||
				en_SampleBlank == stcTestData.CurrentMeasType)
		{

		}
		else if(en_TestQc == stcTestData.CurrentMeasType ||
				en_TestQcBlank == stcTestData.CurrentMeasType)
		{
			uint8_t u8TestQcIdx = GetSelectedTestQcIdx();
			if(NVM_INIT_NOT_OK_FLAG == u8TestQcIdx)
			{
				if(enkeyOk == ShowMainPopUp("Quality Control","Please select a valid QC", true))
				{
					ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
				}
				return;
			}
		}
		textConcentration.setText(" ");
		textAbsorbance.setText(" ");
		UpdateTestScreenMenu();
		/*Enable test - START*/
		TriggerTestProcess();
		/*Enable test - END*/
		BeepBuzzer();
	}
	else if(enTestStatus_InitError == Status)
	{
		ShowHideTestScreenPopup(enTestPopUp_fitlerwheelError ,  std::string("Filter Wheel"));
		BeepBuzzer();
	}
	else if(enTestStatus_DarkADCError == Status)
	{
		ShowHideTestScreenPopup(enTestPopUp_DarkADCError ,  std::string("Initialization Error"));
		BeepBuzzer();
	}
	else if(enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_SelectDarkAdcFilter_Busy != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_SwitchPrimaryWavelength_Busy != Status &&
			enTestStatus_MoveFilterHome != Status &&
			enTestStatus_MoveFilterHome_Busy != Status &&
			enTestStatus_Initialize != Status &&
			enTestPopUp_Null == g_en_TestPopUp /*Close popup first*/)
	{
		ShowHideTestScreenPopup(enTestPopUp_Abort , std::string("Abort"));
		BeepBuzzer();
	}
}

void DrawRescaledGraph(void)
{
	bool FlagUpdateGraph = false;
	#define Y_AXIS_ABS_OFFSET (0.2)
	#define Y_AXIS_ADC_OFFSET (100)
	enMeasurementType CurrentMeasType = stcTestData.CurrentMeasType;
	enMeasurementType PrevMeasCompletedType = stcTestData.PrevMeasCompletedType;
    // Allocate GraphBuffer dynamically on the heap
    float* GraphBuffer = (float*)malloc((MAX_ADC_BUFFER_LENGTH_PRIMARY + MAX_ADC_BUFFER_LENGTH_DEFAULT) * sizeof(float));
    if (GraphBuffer == NULL) {
        // Handle allocation failure
        return;
    }
	float MaxXVal = (float)0;
	float MaxYVal = (float)0;
	float MinXVal = (float)0;
	float MinYVal = (float)0;
	float GraphLineXpoint = 0;
	uint32_t Colour = 0;
	uint16_t TotalMeaurementPoints = 0;

	if(en_DIWaterAdcBlank == CurrentMeasType && en_DIWaterAdcBlank == PrevMeasCompletedType)
	{
		FlagUpdateGraph = true;
	}
	else if((en_Reagent == CurrentMeasType && en_Reagent == PrevMeasCompletedType) ||
			(en_ReagentBlank == CurrentMeasType && en_ReagentBlank == PrevMeasCompletedType) ||
			(en_Reagent == CurrentMeasType && en_ReagentBlank == PrevMeasCompletedType) ||
			(en_ReagentBlank == CurrentMeasType && en_Reagent == PrevMeasCompletedType))
	{
		FlagUpdateGraph = true;
	}
	else if((en_Standard == CurrentMeasType && en_Standard == PrevMeasCompletedType) ||
			(en_StandardBlank == CurrentMeasType && en_StandardBlank == PrevMeasCompletedType) ||
			(en_Standard == CurrentMeasType && en_StandardBlank == PrevMeasCompletedType) ||
			(en_StandardBlank == CurrentMeasType && en_Standard == PrevMeasCompletedType))
	{
		FlagUpdateGraph = true;
	}
	else if((en_TestQc == CurrentMeasType && en_TestQc == PrevMeasCompletedType) ||
			(en_TestQcBlank == CurrentMeasType && en_TestQcBlank == PrevMeasCompletedType) ||
			(en_TestQc == CurrentMeasType && en_TestQcBlank == PrevMeasCompletedType) ||
			(en_TestQcBlank == CurrentMeasType && en_TestQc == PrevMeasCompletedType) ||
			(en_CoagulationQCIncubation == CurrentMeasType && en_TestQc == PrevMeasCompletedType))
	{
		FlagUpdateGraph = true;
	}
	else if((en_Sample == CurrentMeasType && en_Sample == PrevMeasCompletedType) ||
			(en_SampleBlank == CurrentMeasType && en_SampleBlank == PrevMeasCompletedType) ||
			(en_Sample == CurrentMeasType && en_SampleBlank == PrevMeasCompletedType) ||
			(en_SampleBlank == CurrentMeasType && en_Sample == PrevMeasCompletedType) ||
			(en_CoagulationsampleIncubation == CurrentMeasType && en_Sample == PrevMeasCompletedType))
	{
		FlagUpdateGraph = true;
	}

	if(FlagUpdateGraph)
	{
		if(en_DIWaterAdcBlank == PrevMeasCompletedType)
		{
			GraphLineXpoint = INCUBATION_TIME_FOR_WATER_BLANK_SEC;
			/*Copy incubation data plus primary measurement data to array buffer*/
			TotalMeaurementPoints = INCUBATION_TIME_FOR_WATER_BLANK_SEC + MEASUREMENT_TIME_FOR_WATER_BLANK_SEC;
			for(uint16_t Idx = 0 ; Idx < TotalMeaurementPoints ; ++Idx)
			{
				GraphBuffer[Idx] = 	stcTestData.PrimaryAdcBuffer[Idx];
			}

			/*If Bichromatic, Copy Secondary measurement data to array buffer*/
			if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
			{
				for(uint16_t Idx = 0 ; Idx < MEASUREMENT_TIME_FOR_WATER_BLANK_SEC ; ++Idx)
				{
					GraphBuffer[Idx + TotalMeaurementPoints] = 	stcTestData.SecondaryAdcBuffer[Idx];
				}

				TotalMeaurementPoints += MEASUREMENT_TIME_FOR_WATER_BLANK_SEC;
			}
			MinXVal = 0;
			MaxXVal = TotalMeaurementPoints;
			MaxYVal = FindMaxValueFromList(GraphBuffer , TotalMeaurementPoints);
			MinYVal = FindMinValueFromList(GraphBuffer , TotalMeaurementPoints);
			MaxYVal += Y_AXIS_ADC_OFFSET;
			MinYVal -= Y_AXIS_ADC_OFFSET;
		}
		else/*For all other measurements which outputs absorbance*/
		{
			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest){

				GraphLineXpoint = 1;
				/*Copy primary measurement data to array buffer*/
				TotalMeaurementPoints = GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec;
				for(uint16_t Idx = 0 ; Idx < TotalMeaurementPoints ; ++Idx)
				{
					GraphBuffer[Idx] = 	stcTestData.PrimaryAbsBuffer[Idx];
				}

				MinXVal = 0;
				MaxXVal = TotalMeaurementPoints;

				MaxYVal = FindMaxValueFromList(GraphBuffer , TotalMeaurementPoints);
				MinYVal = FindMinValueFromList(GraphBuffer , TotalMeaurementPoints);
				MaxYVal += Y_AXIS_ABS_OFFSET;
				MinYVal -= Y_AXIS_ABS_OFFSET;
			}
			else{
				GraphLineXpoint = GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec;
				/*Copy incubation data plus primary measurement data to array buffer*/
				TotalMeaurementPoints = GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec +
						GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec;
				for(uint16_t Idx = 0 ; Idx < TotalMeaurementPoints ; ++Idx)
				{
					GraphBuffer[Idx] = 	stcTestData.PrimaryAbsBuffer[Idx];
				}

				/*If Bichromatic, Copy Secondary measurement data to array buffer*/
				if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
				{
					for(uint16_t Idx = 0 ; Idx < GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec ; ++Idx)
					{
						GraphBuffer[Idx + TotalMeaurementPoints] = 	stcTestData.SecondaryAbsBuffer[Idx];
					}
					TotalMeaurementPoints += GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec;
				}
				MinXVal = 0;
				MaxXVal = TotalMeaurementPoints;

				MaxYVal = FindMaxValueFromList(GraphBuffer , TotalMeaurementPoints);
				MinYVal = FindMinValueFromList(GraphBuffer , TotalMeaurementPoints);
				MaxYVal += Y_AXIS_ABS_OFFSET;
				MinYVal -= Y_AXIS_ABS_OFFSET;
			}

		}
		ClearGraph(en_WinId_TestRun /*Window ID*/,
								TEST_GRAPH_PIXEL_MIN_X - 5 ,
								TEST_GRAPH_PIXEL_MIN_Y - 5,
								TEST_GRAPH_PIXEL_X_WIDTH + 10,
								TEST_GRAPH_PIXEL_Y_HEIGHT + 5);
		HAL_Delay(5);
		pGraphBG.setPic(PIC_GRAPH);
		HAL_Delay(5);
		UpdateGraphAxisData(&textMainGraphXMax /*X Max*/, &textMainGraphYMax /*Y Max*/,
				MaxXVal,
				MaxYVal,//3/*Y max value*/,
				&textMainGraphXMin /*X Min*/, &textMainGraphYMin /*Y Min*/,
				1/*X max value*/,
				MinYVal/*Y max value*/);
		HAL_Delay(50);
		DrawVerticalLineOnGraph(en_WinId_TestRun /*Window ID*/,
				(GraphLineXpoint - 1) ,
				MinXVal/*X min value*/ , (MaxXVal - 1)/*X max value*/);
		HAL_Delay(5);
		for(uint16_t nI = 1 ; nI < TotalMeaurementPoints ; ++nI)
		{
			HAL_Delay(5);
			if(en_DIWaterAdcBlank == PrevMeasCompletedType)
			{
				if(nI < INCUBATION_TIME_FOR_WATER_BLANK_SEC)
				{
					Colour = COLOUR_MEAS_INCU;
				}
				else if(nI >= INCUBATION_TIME_FOR_WATER_BLANK_SEC &&
						nI < (INCUBATION_TIME_FOR_WATER_BLANK_SEC +
								MEASUREMENT_TIME_FOR_WATER_BLANK_SEC))
				{
					Colour = COLOUR_MEAS_PRI;
				}
				else/*For secondary measuremnet*/
				{
					Colour = COLOUR_MEAS_SEC;
				}
			}
			else
			{
				if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType != en_CoagulationTest)
				{
					if(nI < GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec)
					{
						Colour = COLOUR_MEAS_INCU;
					}
					else if(nI >= GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec &&
							nI < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
									GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec))
					{
						Colour = COLOUR_MEAS_PRI;
					}
					else/*For secondary measuremnet*/
					{
						Colour = COLOUR_MEAS_SEC;
					}
				}
				else
				{
					Colour = COLOUR_MEAS_PRI;
				}
			}
			UpdateGraph(en_WinId_TestRun /*Window ID*/,
					(nI - 1)/*X1*/,
					GraphBuffer[nI - 1] /*Y1*/,
					nI/*X2*/,
					GraphBuffer[nI] /*Y2*/,
					MinXVal/*X min value*/,
					(MaxXVal - 1)/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
					TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					Colour);
		}
	}
    // Free the allocated memory
    free(GraphBuffer);
}
void UpdateRealTimeBuffers(enMeasurementType CurrentMeasType)
{
	/*Show buffers*/
	float StandardConcVal = 0;
	float StandardODVal = 0;
	std::string strStdConc;
	switch(CurrentMeasType)
	{
		case en_DIWaterAdcBlank:
			UpdateAbsBuffer(std::string(IntToCharConverter(stcTestData.DIPrimaryFilterWaterAdc)));
			/*If Bichromatic*/
			if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
			{
				UpdateConentrationBuffer(std::string(IntToCharConverter(stcTestData.DISecondaryFilterWaterAdc)));
			}
			break;
		case en_ReagentBlank:
			UpdateAbsBuffer(std::string(DoubleToCharConverter(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs ,
					STANDARD_PRECISION)));
			break;
		case en_Reagent:
			if(stcTestData.PrevMeasCompletedType == en_ReagentBlank)
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs ,
						STANDARD_PRECISION)));
			}
			else
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(GetInstance_TestParams()[stcTestData.TestId].ReagentAbs ,
						STANDARD_PRECISION)));
			}
			break;
		case en_StandardBlank:
			if(stcTestData.PrevMeasCompletedType == en_Standard)
			{
				StandardConcVal = 0;
				StandardODVal = 0;
				StandardConcVal = GetInstance_TestParams()[stcTestData.TestId].StandardConc[stcTestData.SelectedStandard];
				StandardODVal = e_RunTimeStdOd[stcTestData.SelectedStandard];
				strStdConc = std::string(DoubleToCharConverter(StandardConcVal , STANDARD_PRECISION));
				textConcentration.setText(strStdConc.c_str());
				UpdateAbsBuffer(std::string(DoubleToCharConverter(StandardODVal, STANDARD_PRECISION)));
			}
			else
			{
				StandardConcVal = 0;
				StandardODVal = 0;
				StandardConcVal = GetInstance_TestParams()[stcTestData.TestId].StandardConc[stcTestData.SelectedStandard];
				StandardODVal = e_RunTimeStdBlankOd[stcTestData.SelectedStandard];
				strStdConc = std::string(DoubleToCharConverter(StandardConcVal , STANDARD_PRECISION));
				textConcentration.setText(strStdConc.c_str());
				UpdateAbsBuffer(std::string(DoubleToCharConverter(StandardODVal, STANDARD_PRECISION)));
			}
			break;
		case en_Standard:
			if(stcTestData.PrevMeasCompletedType == en_StandardBlank)
			{
				StandardConcVal = 0;
				StandardODVal = 0;
				StandardConcVal = GetInstance_TestParams()[stcTestData.TestId].StandardConc[stcTestData.SelectedStandard];
				StandardODVal = e_RunTimeStdBlankOd[stcTestData.SelectedStandard];
				strStdConc = std::string(DoubleToCharConverter(StandardConcVal , STANDARD_PRECISION));
				textConcentration.setText(strStdConc.c_str());
				UpdateAbsBuffer(std::string(DoubleToCharConverter(StandardODVal, STANDARD_PRECISION)));
			}
			else
			{
				StandardConcVal = 0;
				StandardODVal = 0;
				StandardConcVal = GetInstance_TestParams()[stcTestData.TestId].StandardConc[stcTestData.SelectedStandard];
				StandardODVal = e_RunTimeStdOd[stcTestData.SelectedStandard];
				strStdConc = std::string(DoubleToCharConverter(StandardConcVal , STANDARD_PRECISION));
				textConcentration.setText(strStdConc.c_str());
				UpdateAbsBuffer(std::string(DoubleToCharConverter(StandardODVal, STANDARD_PRECISION)));
			}
			break;
		case en_SampleBlank:
			if(stcTestData.PrevMeasCompletedType == en_TestQc)
			{
				UpdateAbsBuffer("");
			}
			else
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity,	STANDARD_PRECISION)));
				UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , RESULT_PRECISION)));
			}
			break;
		case en_Sample:
			if(stcTestData.PrevMeasCompletedType == en_TestQc)
			{
				UpdateAbsBuffer("");
				UpdateConentrationBuffer("");
			}
			else if(stcTestData.PrevMeasCompletedType == en_SampleBlank)
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleBlankAbs,STANDARD_PRECISION)));
				UpdateConentrationBuffer("");
			}
			else
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity,	STANDARD_PRECISION)));
				UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , RESULT_PRECISION)));
			}
			break;
		case en_CoagulationsampleIncubation:
			UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity,	STANDARD_PRECISION)));
			UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , RESULT_PRECISION)));
			break;
		case en_TestQcBlank:
			if(stcTestData.PrevMeasCompletedType == en_Sample)
			{
				UpdateAbsBuffer("");
			}
			else
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity,	STANDARD_PRECISION)));
				UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , RESULT_PRECISION)));
			}
			break;
		case en_TestQc:
			if(stcTestData.PrevMeasCompletedType == en_Sample)
			{
				UpdateAbsBuffer("");
				UpdateConentrationBuffer("");
			}
			else if(stcTestData.PrevMeasCompletedType == en_TestQcBlank)
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleBlankAbs, STANDARD_PRECISION)));
				UpdateConentrationBuffer("");
			}
			else
			{
				UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity,	STANDARD_PRECISION)));
				UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , RESULT_PRECISION)));
			}
			break;
		case en_CoagulationQCIncubation:
			UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity,	STANDARD_PRECISION)));
			UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , RESULT_PRECISION)));
			break;
		case en_WashProcedure:
			break;
		default: break;
	}
}

/*Timer logic - start*/
void InitilizeTimerBlock(void)
{
	for (int i = 0; i < MAX_TIMER; i++)
	{
		InitilizeTimer(&(stcTimerBlock[i].m_Timer));
		stcTimerBlock[i].u8Minute = 0;
		stcTimerBlock[i].u8Second = 0;
		stcTimerBlock[i].bTimerRunFlag = false;
		stcTimerBlock[i].bTimerBackgroundComplete = false;
		stcTimerBlock[i].bTimerWindowshown = false;
	}
}

void HandlerButtonTimer(void *ptr)
{
	if (stcTimerBlock[timerIndex].bTimerWindowshown == true)
	{
		ShowHideTimerWindow(false, timerIndex);
	}
	else
	{
		ShowHideTimerWindow(true, timerIndex);
	}
	BeepBuzzer();
}
void HandlerButtonbTimerOne(void *ptr)
{
	timerIndex = 0;
	ShowHideTimerWindow(true, timerIndex);
	BeepBuzzer();
}
void HandlerButtonbTimerTwo(void *ptr)
{
	timerIndex = 1;
	ShowHideTimerWindow(true, timerIndex);
	BeepBuzzer();
}
void HandlerButtonbTimerThree(void *ptr)
{
	timerIndex = 2;
	ShowHideTimerWindow(true, timerIndex);
	BeepBuzzer();
}
void HandlerButtonbTimerFour(void *ptr)
{
	timerIndex = 3;
	ShowHideTimerWindow(true, timerIndex);
	BeepBuzzer();
}

void HandlerButtonbTimerboxHide(void *ptr)
{
	ShowHideTimerWindow(false, timerIndex);
	BeepBuzzer();
}

void HandlerButtonbTimerStartStop(void *ptr)
{
	if (true == stcTimerBlock[timerIndex].bTimerRunFlag)
	{
		bTimerStartStop.Set_background_image_pic(417);
		stcTimerBlock[timerIndex].bTimerRunFlag = false;
		stcTimerBlock[timerIndex].bTimerBackgroundComplete = false;
		StopTimer(&(stcTimerBlock[timerIndex].m_Timer));
		BeepBuzzer();
	}
	else
	{
		if(stcTimerBlock[timerIndex].u8Minute != 0 || stcTimerBlock[timerIndex].u8Second != 0)
		{
			bTimerStartStop.Set_background_image_pic(424);
			stcTimerBlock[timerIndex].bTimerRunFlag = true;
			StartTimer(&(stcTimerBlock[timerIndex].m_Timer), DEFAULT_TEST_TIMER_CNT_VAL_MS);
			BeepBuzzer();
		}
		else
		{
			InstrumentBusyBuzz();
		}
	}
}
void HandlerButtonbTimerReset(void *ptr)
{
	char arrBuff[32] = {0};
	if (true == stcTimerBlock[timerIndex].bTimerRunFlag)
	{
		InstrumentBusyBuzz();
		return; /* No changes while timer is running */
	}
	stcTimerBlock[timerIndex].u8Minute = 0;
	stcTimerBlock[timerIndex].u8Second = 0;
	stcTimerBlock[timerIndex].bTimerRunFlag = false;
	stcTimerBlock[timerIndex].bTimerBackgroundComplete = false;
	StopTimer(&(stcTimerBlock[timerIndex].m_Timer));
	snprintf(arrBuff, 31, "%02d : %02d", stcTimerBlock[timerIndex].u8Minute, stcTimerBlock[timerIndex].u8Second);
	tTimerValTxt.setText(arrBuff); // Assuming `tTimerValTxt` corresponds to the correct timer
	BeepBuzzer();
}
void HandlerButtonbTimerPlus(void *ptr)
{
	char arrBuff[32] = {0};
	if (true == stcTimerBlock[timerIndex].bTimerRunFlag)
	{
		InstrumentBusyBuzz();
		return; /* No changes while timer is running */
	}
	stcTimerBlock[timerIndex].u8Minute++;
	if (MAX_TIMER_MINS <= stcTimerBlock[timerIndex].u8Minute)
	{
		stcTimerBlock[timerIndex].u8Minute = MAX_TIMER_MINS;
	}
	snprintf(arrBuff, 31, "%02d : %02d", stcTimerBlock[timerIndex].u8Minute, stcTimerBlock[timerIndex].u8Second);
	tTimerValTxt.setText(arrBuff); // Assuming `tTimerValTxt` corresponds to the correct timer
	BeepBuzzer();
}

void HandlerButtonbTimerMinus(void *ptr)
{
	char arrBuff[32] = {0};
	if (true == stcTimerBlock[timerIndex].bTimerRunFlag)
	{
		InstrumentBusyBuzz();
		return; /* No changes while timer is running */
	}
	stcTimerBlock[timerIndex].u8Minute--;
	if (0 >= stcTimerBlock[timerIndex].u8Minute)
	{
		stcTimerBlock[timerIndex].u8Minute = 0;
	}
	snprintf(arrBuff, 31, "%02d : %02d", stcTimerBlock[timerIndex].u8Minute, stcTimerBlock[timerIndex].u8Second);
	tTimerValTxt.setText(arrBuff); // Assuming `tTimerValTxt` corresponds to the correct timer
	BeepBuzzer();
}
void ShowHideRunScreenTimer(bool bStatus, int timerIndex)
{
	if (true == bStatus)
	{
		tTimerValRunscrtext.setVisible(true); // Assuming `tTimerValRunscrtext` corresponds to the correct timer
	}
	else
	{
		tTimerValRunscrtext.setVisible(false);
	}
}
void ShowHideTimerWindow(bool bStatus, int timerIndex)
{
	char arrBuff[32] = {0};
	tTimerValTxt.setVisible(bStatus); // Assuming `tTimerValTxt` corresponds to the correct timer
	pTImerPicA.setVisible(bStatus);   // Assuming `pTImerPicA` corresponds to the correct timer
	pTImerPicB.setVisible(bStatus);   // Assuming `pTImerPicB` corresponds to the correct timer
	bTimerStartStop.setVisible(bStatus); // Assuming `bTimerStartStop` corresponds to the correct timer
	bTimerboxHide.setVisible(bStatus);   // Assuming `bTimerboxHide` corresponds to the correct timer
	bTimerReset.setVisible(bStatus);     // Assuming `bTimerReset` corresponds to the correct timer
	bTimerPlus.setVisible(bStatus);      // Assuming `bTimerPlus` corresponds to the correct timer
	bTimerMinus.setVisible(bStatus);     // Assuming `bTimerMinus` corresponds to the correct timer
	bTimerOne.setVisible(bStatus);
	bTimerTwo.setVisible(bStatus);
	bTimerThree.setVisible(bStatus);
	bTimerFour.setVisible(bStatus);
	pTimButtonBar.setVisible(bStatus);

	if (true == bStatus)
	{
		bTimerOne.Set_background_image_pic(PIC_TIMER_DESELECT);
		bTimerTwo.Set_background_image_pic(PIC_TIMER_DESELECT);
		bTimerThree.Set_background_image_pic(PIC_TIMER_DESELECT);
		bTimerFour.Set_background_image_pic(PIC_TIMER_DESELECT);
		bTimerOne.Set_font_color_pco(COLOUR_WHITE);
		bTimerTwo.Set_font_color_pco(COLOUR_WHITE);
		bTimerThree.Set_font_color_pco(COLOUR_WHITE);
		bTimerFour.Set_font_color_pco(COLOUR_WHITE);

		switch(timerIndex)
		{
			case 0:
				bTimerOne.Set_background_image_pic(PIC_TIMER_SELECT);
				bTimerOne.Set_font_color_pco(COLOUR_POPUP_GRAY);
			break;

			case 1:
				bTimerTwo.Set_background_image_pic(PIC_TIMER_SELECT);
				bTimerTwo.Set_font_color_pco(COLOUR_POPUP_GRAY);
			break;

			case 2:
				bTimerThree.Set_background_image_pic(PIC_TIMER_SELECT);
				bTimerThree.Set_font_color_pco(COLOUR_POPUP_GRAY);
			break;
			case 3:
				bTimerFour.Set_background_image_pic(PIC_TIMER_SELECT);
				bTimerFour.Set_font_color_pco(COLOUR_POPUP_GRAY);
			break;
		}
		ShowHideRunScreenTimer(false, timerIndex);
		stcTimerBlock[timerIndex].bTimerWindowshown = true;
		stcTimerBlock[timerIndex].bTimerBackgroundComplete = false;
		if (true == stcTimerBlock[timerIndex].bTimerRunFlag)
		{
			bTimerStartStop.Set_background_image_pic(424); // Assuming `bTimerStartStop` corresponds to the correct timer
		}
		else
		{
			bTimerStartStop.Set_background_image_pic(417);
		}
		snprintf(arrBuff, 31, "%02d : %02d", stcTimerBlock[timerIndex].u8Minute, stcTimerBlock[timerIndex].u8Second);
		tTimerValTxt.setText(arrBuff);
	}
	else
	{
		stcTimerBlock[timerIndex].bTimerWindowshown = false;
		if (true == stcTimerBlock[timerIndex].bTimerRunFlag)
		{
			ShowHideRunScreenTimer(true, timerIndex);
		}
	}
}
void TestTimer_Task(void)
{
	char arrBuff[32] = {0};
	for (int i = 0; i < MAX_TIMER ; i++)
	{
		if (true == stcTimerBlock[i].bTimerRunFlag)
		{
			Reset_AutoSleepMinsCounterTimer(); /* Reset auto sleep timer counter when test timer is running */
			if (true == Timer_IsTimeout(&(stcTimerBlock[i].m_Timer), enTimerNormalStop))
			{
				snprintf(arrBuff, 31, "%02d : %02d", stcTimerBlock[timerIndex].u8Minute, stcTimerBlock[timerIndex].u8Second);
				tTimerValTxt.setText(arrBuff); // Assuming `tTimerValTxt` corresponds to the correct timer
				tTimerValRunscrtext.setText(arrBuff);

				stcTimerBlock[i].u8Second--;
				if (0 > stcTimerBlock[i].u8Second)
				{
					stcTimerBlock[i].u8Second = 59;
					stcTimerBlock[i].u8Minute--;
					if (0 > stcTimerBlock[i].u8Minute) /* End of timer */
					{
						stcTimerBlock[i].u8Minute = 0;
						stcTimerBlock[i].u8Second = 0;
						stcTimerBlock[i].bTimerRunFlag = false;
						StopTimer(&(stcTimerBlock[i].m_Timer));
						timerIndex = i;
						TestTimerCompleteBuzzer();
						if (en_WinId_TestRun != stcScreenNavigation.CurrentWindowId)
						{
							stcTimerBlock[i].bTimerBackgroundComplete = true;
						}
						else
						{
							ShowHideTimerWindow(true, timerIndex);
							tTimerValTxt.setText("00 : 00");
						}
						return;
						/* Timer ends */
					}
				}
				/* Restart Timer */
				StartTimer(&(stcTimerBlock[i].m_Timer), DEFAULT_TEST_TIMER_CNT_VAL_MS);
			}
		}
	}
}

/*Timer logic - end*/
static void HandlerUserlogin(void *ptr)
{
	en_TestRunStatus Status = ReadTestRunStatus();
	if((enTestStatus_Idle != Status &&
			enTestStatus_InitError != Status &&
			enTestStatus_SelectDarkAdcFilter != Status &&
			enTestStatus_ReadDarkAdcPrimary != Status &&
			enTestStatus_ReadDarkAdcSecondary != Status &&
			enTestStatus_SwitchPrimaryWavelength != Status &&
			enTestStatus_MoveFilterHome != Status) || (enTestPopUp_Null != g_en_TestPopUp /*Close popup first*/))
	{
		InstrumentBusyBuzz();
		return;
	}
	ChangeWindowPage(en_WinId_UserLogin , en_WinId_TestRun);
	BeepBuzzer();
}

