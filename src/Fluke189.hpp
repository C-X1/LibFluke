/*
 * Fluke189.h
 *
 *  Created on: 06.01.2010
 *      Author: cyborg-x1
 */

#ifndef FLUKE189_H_
#define FLUKE189_H_


#include "SerialDevice.hpp"
#include "SerialResponse.hpp"
#include <string>
#include <inttypes.h>
#include <vector>


using namespace LibSerial;
namespace Fluke {



class Fluke189: public LibSerial::SerialDevice
{
public:



	// TResponseMask Types / ResponseContainer (RCT_<CMD>) / Functions
	// Variable Prefix
	//		I_<variable>=important data
	//		n_<variable>=none important data (like commas, space and line feeds)
	//		u_<variable>=data purpose unknown
	//		m_<variable>=special variable for data extraction with union
	//		<CR> Carriage Return == /r

	//Get Device Name, Firmware Version, ID Number
	//Command: ID<CR>

	typedef struct {
		char I_CMD_ACK;
		char n_CR0;
		char I_DeviceName[9];
		char n_CommaAndSpace0[2];
		char I_FirmwareVersion[5];
		char n_Comma0;
		char I_ID [10];
		char n_CR1;
	} cmdr_ID_t;

	typedef  SerialResponse<cmdr_ID_t> RCT_ID;

	RCT_ID CMD_ID(bool TerminatedSubstrings)
	{
		RCT_ID Response = this->SendCommand<cmdr_ID_t>("ID\r",this->DelayResponse_us, this->DelayChar_us);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[11]=0;
			Response[18]=0;
			Response[30]=0;
		}
		return Response;
	}


	//Get Setup Informations
	//Command: QS<CR>
#pragma pack(push,1)
	typedef struct{
		char I_CMD_ACK;
		char n_CR0;
		char n_QSHeaderInfo_Comma[3];

		unsigned int I_LogInterval : 16; // Interval for Logging Mode deciseconds
		unsigned int I_dBREFmV     : 8;  // dB Reference 1=V 0=mV
		unsigned int u_unknown0	   : 8;  // TODO:What does that do? (always 00?)
		unsigned int I_dBREF	   : 16; // dB Reference
				 int I_TempOffset  : 16; // Temperature Offset
		unsigned int I_DegC_nDegF  :  8; // High if Celsius is selected, Low for Fahrenheit
		unsigned int u_unkown1	   :  8; // TODO: Find out what that is used for (always 00?)
		unsigned int I_BL_Off	   : 16; // Backlite off time, deciseconds
		unsigned int I_MMTime	   : 32; // Current time of the multimeter in deciseconds from day 0
		unsigned int I_Power_Off   : 16; // Power off time in minutes
		unsigned int I_Frequency   :  8; // Frequency Setting 0=50Hz 1=60Hz
		unsigned int u_unknown2	   :  8; // TODO: Find out what that is used for (always 00?)
		unsigned int I_4Digits     :  8; // Digits setting (0=5 digits , 1=4 digits)
		unsigned int u_unknown3	   :  8; // TODO: Find out what that is used for (always 00?)
		unsigned int I_Beep_On	   :  8; // Beeo setting (0=off, 1=on)
		unsigned int u_ending1	   : 32; // TODO: Find out what the ending is used for (always 0x 00 90 01 c0 00 00 ??? )
		unsigned int u_ending2	   : 16; // ^^
	} cmdr_QS_t;
#pragma pack(pop)

	typedef SerialResponse<cmdr_QS_t> RCT_QS;


	RCT_QS CMD_QS(bool TerminatedSubstrings)
	{
		RCT_QS Response = this->SendCommand<cmdr_QS_t>("QS\r",this->DelayResponse_us, this->DelayChar_us);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}


	//Get Current Reading
	//Command QD 0<CR>
#pragma pack(push,1)
	typedef struct{
		char I_CMD_ACK;
		char n_CR0;
		char n_QDHeaderInfo_Comma[3];


		unsigned int I_Time0				 :32; //Time(1st Occur.)
		union
		{
		  signed int I_priValue0			 :32; //Primary Value	 (1st Occur.)
		  struct
		  {
			unsigned int I_ErrorNoPV0		 :8;  //8 Bit Error Code (If Error == 1)
			unsigned int 					 :22; //unneeded bits for Error
			unsigned int I_ErrorPV0			 :2;  //Error when value == 1 (NoError: is 3 when negative or 0 when positiv)
		  };
		};
		unsigned int I_priDecimal0  		 :8;  //Pri. Dec. Pnt Loc.(1st Occur.)
		  signed int I_priSI_Prefix0		 :8;  //Pri. SI-Prefix 	 (1st Occur.)
		union
		{
		  signed int I_secValue0    		 :32; //Sec. Disp. Value
		  struct
		  {
			unsigned int I_ErrorNoSV0		 :8;  //8 Bit Error Code (If Error == 1)
			unsigned int 					 :22; //unneeded bits for Error
			unsigned int I_ErrorSV0 		 :2;  //Error when value == 1 (NoError: is 3 when negative or 0 when positiv)
		   };
		 };
		unsigned int I_secDecimal			 :8;  //Sec. Disp. Dec. Pnt Locaction
		  signed int I_secSi_Prefix	 		 :8;  //Sec. Disp. Prefix TODO: Check if correct

		union
		{
		  signed int I_secValue1     	     :32; //Sec. Disp. Value (2. Occurence)
		  struct
		  {
			unsigned int I_ErrorNoSV1 	     :8;  //8 Bit Error Code (If Error == 1)
			unsigned int 					 :22; //unneeded bits for Error
			unsigned int I_ErrorSV1 		 :2;  //Error when value == 1 (NoError: is 3 when negative or 0 when positiv)
		   };
		 };
		unsigned int I_Time1				 :32; //Time(2nd Occur.)
		union
		{
		  signed int I_priValue1			 :32; //Primary Value	 (2nd Occur.)
		  struct
		  {
			unsigned int I_ErrorNoPV1	 	 :8;  //8 Bit Error Code (If Error == 1)
			unsigned int 					 :22; //unneeded bits for Error
			unsigned int I_ErrorPV1 		 :2;  //Error when value == 1 (NoError: is 3 when negative or 0 when positiv)
		  };
		};
		unsigned int I_priDecimal1  		 :8;  //Pri. Dec. Pnt Loc.(2nd Occur.)
		  signed int I_priSI_Prefix1 		 :8;  //Pri. SI-Prefix 	 (2nd Occur.)


			    bool I_S_Auto		 		 :1;  //High if Auto Hold
			    bool I_S_Hold				 :1;  //High if Hold or Auto Hold
			    bool I_S_Log				 :1;  //High if Log Mode
			    bool I_S_Avg		 		 :1;  //High if MinMax:AVG (also Fast)
			    bool I_S_Max		 		 :1;  //High if MinMax:MAX (also Fast)
			    bool I_S_Min		 		 :1;  //High if MinMax:MIN (also Fast)
			    bool I_S_Fast		 	     :1;  //High if Fast MinMax
			    bool I_LowBattery	 		 :1;  //High if LowBattery is on

			    bool I_SetupOrViewMem 		 :1;  //Seems to be one when in Setup or in ViewMem
			    bool I_S_Delta		 		 :1;  //High if Delta is on
				bool I_S_Percent     		 :1;  //High if Percent is on
		unsigned int I_MEMclr		 		 :2;  //In ViewMem its 2 if there are saves or logs, if not its 00, if Clr? is displayed its 1
		unsigned int I_AutoRange	 		 :1;  //1 when Auto Range
		unsigned int I_ManualRange			 :1;  //1 when Manual Range
			    bool u_bit1 				 :1;  //TODO Find out what that is for

		unsigned int I_DBREF_Set	 		 :16; //dBRef Value STD:600

		unsigned int I_MeasureMode	 		 :6;  //Mode (ACV DCV etc...)
		unsigned int I_Hz_Percent_ms 		 :2;  //(Hz=1 %=2 ms=3)

				bool I_MinMaxMode	 		 :1;  //High if MinMax Mode(also Fast)
				bool I_Fast			  		 :1;  //High if Fast
				bool u_bit2 		 		 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
        unsigned int I_MinMaxAvg	 		 :2;  //(1=Max 2=Min 3=AVG)
				bool u_bit3			 		 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_RisingEtch	 		 :1;  //High if rising etch displayed
				bool I_FallingEtch	 		 :1;  //High if falling etch displayed

		unsigned int I_SubState_ACDC 		 :2;  //Substates of V mV mA uA (DC Modes ONLY)
				bool u_bit4		 	  		 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_Hold			   		 :1;  //High if Hold     (Low on AutoHold)
				bool I_AutoHold		 	 	 :1;  //High if AutoHold (Low on Hold)
				bool u_bit5			 		 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_dBRef_V_nm	 		 :1;  //High if V selected for dB_Ref, Low if m selected
				bool I_DegC_nDegF	 		 :1;  //High if Celsius is selected, Low for Fahrenheit

				bool I_Delta				 :1;  //High if Delta(Low if Delta Percent)
				bool I_DeltaPercent			 :1;  //High if DeltaPercent(Low if Delta)
				bool u_bit6					 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_Unit_AC_dB			 :1;  //High if(Prim is AC and Sec dB) only V and mV (AC)
				bool I_Unit_dB_AC			 :1;  //High if(Prim is dB and Sec AC) only V and mV (AC)
				bool u_bit7					 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool u_bit8					 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_nTemperature			 :1;  //Bit is one when NOT in Temperature Measure or Setup or Clr? //TODO Redefine Name

		unsigned int I_SelectedRange 	 	 :8;  //Range Mode
		unsigned int I_CurrentView   		 :8;  //Displays Data of what is on screen currently 1=Measure, 2=Setup, 22=Setup(Timesetting) ...
		unsigned int u_byte0         		 :8;  //TODO:Find out what that is for //always 1
		unsigned int u_byte1         		 :8;  //TODO:Find out what that is for //always 0
	} cmdr_QD0_t;
#pragma pack(pop)

	typedef SerialResponse<cmdr_QD0_t> RCT_QD0;


	RCT_QD0 CMD_QD0(bool TerminatedSubstrings)
	{
		RCT_QD0 Response = this->SendCommand<cmdr_QD0_t>("QD 0\r",this->DelayResponse_us, this->DelayChar_us);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}






	//Get Logs
	//Command QD 2<CR>

#pragma pack(push, 1) //Pragma packing width 1

	typedef struct
	{
		unsigned int I_TimeBeg	 :32; 	//Beginning timestamp for entry in units of 0.1s
        unsigned int I_Decimal   :8; 	//Decimal point location
        signed int I_SI_Prefix	 :8;    //Units.  -1 for milli, 1 for kilo, etc.
        signed int I_MinValue    :32;	//Lowest value for this time period
        signed int I_MaxValue    :32;	//Maximum value for this time period
        signed int I_ActualValue :32;	//Actual value, sum of N
        unsigned int u_byte0 	 :32;   //?, always all 0 in my tests
        unsigned int I_SummedVal :32;   //Number of values which were summed
        unsigned int I_Status	 :8;     //Status - 0x05 for normal, 0x85 for last entry, other //values for range changes or other oddities
        unsigned int u_byte1     :8;	//? - always 0x01 in my tests
        unsigned int I_TimeEnd	 :32; 	//Ending timestamp for this entry
     }qd2_set_t;

	typedef struct
	{
		char 		 I_CMD_ACK;
		char 		 n_CR0;
		char		 n_QDHeaderInfo_Comma[3];

		unsigned int DataCount       	 :16; //995 max

		unsigned int I_InitalValue   	 :32; //Value on start of logging
		unsigned int I_InitDecimal	 	 :8;  //Initial Disp. Dec. Pnt Locaction
		  signed int I_InitPrefix    	 :8;  //Initial SI-Prefix

			    bool I_S_Auto		 	 :1;  //High if Auto Hold
				bool I_S_Hold		 	 :1;  //High if Hold or Auto Hold
		        bool I_S_Log		 	 :1;  //High if Log Mode
		        bool I_S_Avg		 	 :1;  //High if MinMax:AVG (also Fast)
		        bool I_S_Max		 	 :1;  //High if MinMax:MAX (also Fast)
		        bool I_S_Min		 	 :1;  //High if MinMax:MIN (also Fast)
		        bool I_S_Fast		 	 :1;	 //High if Fast MinMax
		        bool I_LowBattery	 	 :1;	 //High if LowBattery is on

		        bool I_SetupOrViewMem	 :1;  //Seems to be one when in Setup or in ViewMem //Unknown if its available in Log Mode
		        bool I_S_Delta		 	 :1;  //High if Delta is on
		        bool I_S_Percent     	 :1;  //High if Percent is on
		unsigned int I_MEMclr		  	 :2;  //In ViewMem its 2 if there are saves or logs, if not its 00, if Clr? is displayed its 1 //Unknown if its available in Log Mode
		unsigned int I_AutoRange	 	 :1;  //1 when Auto Range
		unsigned int I_ManualRange	 	 :1;  //1 when Manual Range
	 	        bool u_bit1			 	 :1;  //TODO Find out what that is for

		unsigned int I_DBREF_Set	     :16; //dBRef Value STD:600
		unsigned int I_MeasureMode	 	 :6;  //Mode (ACV DCV etc...)
		unsigned int I_Hz_Percent_ms 	 :2;  //(Hz=1 %=2 ms=3)

				bool I_MinMaxMode		 :1;  //High if MinMax Mode(also Fast)
				bool I_Fast				 :1;  //High if Fast
				bool u_bit2				 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
		unsigned int I_MinMaxAvg		 :2;  //(1=Max 2=Min 3=AVG)
				bool u_bit3				 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_RisingEtch	 	 :1;  //High if rising etch displayed
				bool I_FallingEtch	 	 :1;  //High if falling etch displayed

		unsigned int I_SubState_ACDC     :2;  //Substates of V mV mA uA (DC Modes ONLY)
				bool u_bit4		 	  	 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_Hold			   	 :1;  //High if Hold     (Low on AutoHold)
				bool I_AutoHold		 	 :1;  //High if AutoHold (Low on Hold)
				bool u_bit5			 	 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_dBRef_V_nm	 	 :1;  //High if V selected for dB_Ref, Low if m selected
				bool I_DegC_nDegF	 	 :1;  //High if Celsius is selected, Low for Fahrenheit

				bool I_Delta			 :1;  //High if Delta(Low if Delta Percent)
				bool I_DeltaPercent		 :1;  //High if DeltaPercent(Low if Delta)
				bool u_bit6				 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_Unit_AC_dB		 :1;  //High if(Prim is AC and Sec dB) only V and mV (AC)
				bool I_Unit_dB_AC		 :1;  //High if(Prim is dB and Sec AC) only V and mV (AC)
				bool u_bit7				 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool u_bit8				 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				bool I_TODO	 			 :1;  //TODO Redefine Name //Maybe 1 when Decimal Loc is not in use ???

		unsigned int I_SelectedRange 	 :8;  //Range: 5 50 500 5000 1 10 100 1000

		unsigned int uh10:8;


		qd2_set_t dsets[1];
	} cmdr_QD2_t;

#pragma pack(pop) //Reset pack pragma


	typedef  SerialResponse<cmdr_QD2_t> RCT_QD2;


	RCT_QD2 CMD_QD2(bool TerminatedSubstrings)
	{
		RCT_QD2 Response = this->SendCommand<cmdr_QD2_t>("QD 2\r",this->DelayResponse_us, this->DelayChar_us);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}













	//Get Single Saves
	//Command QD 4<CR>

#pragma pack(push, 1) //Pragma packing width 1
	typedef struct
			{
				unsigned int I_Time0		 :32; //Time(1st Occur.)
				union
				{
				  signed int I_priValue0	 :32; //Primary Value	 (1st Occur.)
				  struct
				  {
					unsigned int I_ErrorNoPV0:8;  //8 Bit Error Code (If Error == 1)
					unsigned int 			 :22; //unneeded bits for Error
					unsigned int I_ErrorPV0	 :2;  //Error when value == 1
				  };
				};
				unsigned int I_priDecimal0   :8;  //Pri. Dec. Pnt Loc.(1st Occur.)
				  signed int I_priSI_Prefix0 :8;  //Pri. SI-Prefix 	 (1st Occur.)
				union
				{
				  signed int I_secValue		 :32; //Sec. Disp. Value
				  struct
				  {
					unsigned int I_ErrorNoSV :8;  //8 Bit Error Code (If Error == 1)
					unsigned int 			 :22; //unneeded bits for Error
					unsigned int I_ErrorSV	 :2;  //Error when value == 1
				   };
				 };
				unsigned int I_secDecimal	 :8;  //Sec. Disp. Dec. Pnt Locaction
				  signed int I_secSi_Prefix	 :8;  //Sec. Disp. Prefix TODO: Check if correct
				unsigned int u_unknown0		 :16; //TODO::FIND OUT::Most times 0x0013 sometimes 0x0001 or 0x0002 WTF?
				unsigned int u_unknown1		 :16; //TODO::Make it change to find its purpose
				unsigned int I_Time1		 :32; //Time(2nd Occur.)
				union
				{
				  signed int I_priValue1	 :32; //Primary Value	 (2nd Occur.)
				  struct
			      {
				    unsigned int I_ErrorNoPV1:8;  //8 Bit Error Code (If Error == 1)
					unsigned int 			 :22; //unneeded bits for Error
					unsigned int I_ErrorPV1	 :2;  //Error when value == 1
				  };
				};
				unsigned int I_priDecimal1	 :8;  //Pri. Dec. Pnt Loc.(2nd Occur.)

				  signed int I_priSI_Prefix1 :8;  //Pri. SI-Prefix 	 (2nd Occur.)

						bool I_S_Auto		 :1;  //High if Auto Hold
						bool I_S_Hold		 :1;  //High if Hold or Auto Hold
						bool I_S_Log		 :1;  //High if Log Mode
						bool I_S_Avg		 :1;  //High if MinMax:AVG (also Fast)
						bool I_S_Max		 :1;  //High if MinMax:MAX (also Fast)
						bool I_S_Min		 :1;  //High if MinMax:MIN (also Fast)
						bool I_S_Fast		 :1;	 //High if Fast MinMax
						bool I_LowBattery	 :1;	 //High if LowBattery is on

				unsigned int I_RangeMode	 :8;  //Range Mode

				unsigned int I_DBREF_Set	 :16; //dBRef Value STD:600

				unsigned int I_MeasureMode	 :6;  //Mode (ACV DCV etc...)
				unsigned int I_Hz_Percent_ms :2;  //(Hz=1 %=2 ms=3)

						bool I_MinMaxMode	 :1;  //High if MinMax Mode(also Fast)
						bool I_Fast			 :1;  //High if Fast
						bool u_bit6			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
				unsigned int I_MinMaxAvg	 :2;  //(1=Max 2=Min 3=AVG)
						bool u_bit5			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
						bool I_RisingEtch	 :1;  //High if rising etch displayed
						bool I_FallingEtch	 :1;  //High if falling etch displayed

				unsigned int I_SubState_ACDC :2;  //Substates of V mV mA uA (DC Modes ONLY)
						bool u_bit4			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
						bool I_Hold			 :1;  //High if Hold     (Low on AutoHold)
						bool I_AutoHold		 :1;  //High if AutoHold (Low on Hold)
						bool u_bit3			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
						bool I_dBRef_V_nm	 :1;  //High if V selected for dB_Ref, Low if m selected
						bool I_DegC_nDegF	 :1;  //High if Celsius is selected, Low for Fahrenheit

						bool I_Delta		 :1;  //High if Delta(Low if Delta Percent)
						bool I_DeltaPercent	 :1;  //High if DeltaPercent(Low if Delta)
						bool u_bit2			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
						bool I_Unit_AC_dB	 :1;  //High if(Prim is AC and Sec dB) only V and mV (AC)
						bool I_Unit_dB_AC	 :1;  //High if(Prim is dB and Sec AC) only V and mV (AC)
						bool u_bit1			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
						bool u_bit0			 :1;  //TODO: Find a use for this: always 0 (Maybe Cal?)
						bool I_TODO	 		 :1;  //TODO Redefine Name see usage in QD0 //use Decimal???

				unsigned int I_SelectedRange :8;  //Range: 5 50 500 5000 1 10 100 1000
				unsigned int u_unknown2	    :16; //TODO: Find out how to change this always 0x0101<ending>
				unsigned int I_DataEnding	:8;	 //TODO: Find a way to calculate this ending byte (always the same series)
			}qd4_set_t;

	typedef struct
	{
		char I_CMD_ACK;
		char n_CR0;
		char n_QDHeaderInfo_Comma[3];
		unsigned int DataCount:8;
		qd4_set_t dsets[1];
	} cmdr_QD4_t;
	#pragma pack(pop) //Reset pack pragma


	typedef  SerialResponse<cmdr_QD4_t> RCT_QD4;

	RCT_QD4 CMD_QD4(bool TerminatedSubstrings)
	{
		RCT_QD4 Response = this->SendCommand<cmdr_QD4_t>("QD 4\r",this->DelayResponse_us, this->DelayChar_us);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}


		//TODO Add Function for generating a simple list... //extra field for prefix or direct calculated to Unit
		//Time, ValuePri, ValuePriUnit, PrefixPri, ValueSec, ValueSecUnit, PrefixSec

	typedef struct
	{
		struct time
		{
			unsigned short year;
			unsigned short month:8;
			unsigned short day:9;
			unsigned short hour:9;
			unsigned short minute:10;
			unsigned short second:10;
		};
		signed int PriValue;
		signed int PriPrefixPowerFact;
		char PriUnit;
		signed int SecValue;
		signed int SecPrefixPowerFact;
		char SecUnit;
		unsigned int Range;

		bool RisingEtch :1;
		bool FallingEtch:1;
		bool Percent	:1;
		bool Delta		:1;


		bool Auto		:1;
		bool Hold		:1;
		bool Log		:1;
		bool Max		:1;
		bool Min		:1;
		bool Avg		:1;
		bool LowBat		:1;
		bool Error		:1;
		bool Fuse		:1;
		bool Leads		:1;
		bool NoConOORan	:1;
		bool Open		:1;
		bool StrangeVal :1;


	} SimpleListElement__QD4_BIN__t;


	std::vector<SimpleListElement__QD4_BIN__t> CMD_QD4_CreateSimpleList_BIN();


	//Units for Generated Binary SimpleLists
	enum SimpleListUnits_BIN
	{
		SL_DISP_OFF		=0,
		SL_AC_Ampere	=1,
		SL_ACDC_Ampere	=2,
		SL_AC_Volts		=3,
		SL_ACDC_Volts	=4,
		SL_Celsius		=5,
		SL_dB			=6,
		SL_Fahrenheit	=7,
		SL_Farad		=8,
		SL_DC_Ampere	=9,
		SL_DC_Volts		=10,
		SL_Hz			=11,
		SL_ms			=12,
		SL_Ohm 			=13,
		SL_Percent		=14,
	};




	//Fluke Protocol Field Enums
	enum CMD_ACK //Command Acknowledge and Error Codes
	{
		ERR_ACK   =  '0',
		ERR_FAIL  =  '1',
		ERR_NODATA=  '5',
	};


	enum Range
	{
		RANGE_1		=0x01,
		RANGE_10	=0x41,
		RANGE_100	=0x81,
		RANGE_1000	=0xc1,
		RANGE_5		=0x21,
		RANGE_50	=0x61,
		RANGE_500	=0xA1,
		RANGE_5000	=0xE1,
	};

	enum RangeMode
	{
		RANGEMODE_AUTO	=0xA0,
		RANGEMODE_MANUAL=0xC0,
		RANGEMODE_NONE	=0x80,
	};

	enum ValueError
	{
		Display_OFFLINE			=0x01,
		OPEN__NOTHING_CONNECTED	=0x16,
		FUSE					=0x1B,
		LEADS_CONNECTION_WRONG	=0x1D,
		OL_OUTOFRANGE_NOCON		=0x21,
	};

	enum MeasureMode
	{
		MM_POS_BETW_2_MODES	=0x000,
		MM_AC_Volt 			=0x001,
		MM_AC_mVolt			=0x002,
		MM_DC_Volt 			=0x003,
		MM_DC_mVolt			=0x004,
		MM_R_Ohm   			=0x009,
		MM_R_BEEP_Ohm		=0x00b,
		MM_CAP_Farad		=0x00c,
		MM_Diode_Volt		=0x00d,
		MM_Temp_Celsius		=0x01a,
		MM_Temp_Fahrenheit	=0x01b,
		MM_AC_Ampere		=0x00e,
		MM_AC_uAmpere		=0x010,
		MM_DC_Ampere		=0x011,
		MM_DC_uAmpere		=0x013,
		MM_DC_V_Submode		=0x005, //one of three
		MM_DC_mV_Submode	=0x006,
		MM_DC_A_Submode		=0x014,
		MM_DC_mA_Submode	=0x016,
	};

	enum CurrentView
	{
		CV_DisplayingMeasure=0x01,
		CV_InSetup_noTimeSet=0x02,
	    CV_InSetup_TimeSet=0x22,
	    CV_Showing_CLR_Saves=0x03,
	    CV_Showing_CLR_Log=0x05,
	    CV_ViewMem_NoData=0x06,
	};



private:
	//Private Attributes
	unsigned int DelayChar_us;
	unsigned int DelayResponse_us;




public:

	Fluke189();
	Fluke189(std::string filename);
	virtual ~Fluke189();

	//Fluke189 Specific Command Functions





private:
	void SetupInterface( void );


};

}

#endif /* FLUKE189_H_ */
