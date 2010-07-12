/**
 * @mainpage
 * @todo Add Documentation to mainpage: Examples, Description etc....
 * @todo Implement another object for min max and average analysis on QD0 packages: Think of name for that object...
 */




#ifndef FLUKE189_H_
#define FLUKE189_H_


#include "SerialDevice.hpp"
#include "SerialResponse.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <exception>

#include <inttypes.h>
#include <vector>
#include <math.h>
#include <stdlib.h>


using namespace LibSerial;
namespace Fluke {


/**
 * Class for accessing a Fluke189 multimeter\n
 *
 * Initialization Example:\n
 * Fluke::Fluke189 Device("/dev/ttyUSB0");\n
 *
 * Sending a command:\n
 * @todo finish this Fluke189 description, add examples and notice other classes...
 *
 */
class Fluke189: public LibSerial::SerialDevice
{
public:



	// TResponseMask Types / ResponseContainer (RCT_<CMD>) / Functions
	// Variable Prefix
	//		I_<variable>=important data
	//		n_<variable>=none important data (like commas, space and line feeds)
	//		u_<variable>=data purpose unknown
	//		<CR> Carriage Return == /r

	//Get Device Name, Firmware Version, ID Number
	//Command: ID<CR>

	/**
	 * This struct is used as datamask for the ID command
	 */
	typedef struct {
		char I_CMD_ACK; 			///< Acknowledge code (0=ok, 1=Error)
		char n_CR0;  				///< carriage return
		char I_DeviceName[9];       ///< Fluke189
		char n_CommaAndSpace0[2];   ///< just a comma and a space char
		char I_FirmwareVersion[5];  ///< Firmware version string
		char n_Comma0;				///< a comma
		char I_ID [10];             ///< string of ID number of the multimeter
		char n_CR1;                 ///< second carriage return
	} cmdr_ID_t;

	/**
	 * Type of the container for the ID command
	 */
	typedef  SerialResponse<cmdr_ID_t> RCT_ID;


	/**
	 *
	 */
	RCT_ID CMD_ID(bool TerminatedSubstrings,void* progressbar_object ,void (*progressbar_function)(void* progressbar_object ,unsigned int current_byte, unsigned int byte_amount))
	{
		RCT_ID Response = this->SendCommand<cmdr_ID_t>("ID\r",this->DelayResponse_us, this->DelayChar_us, 30, 0, 0, false, 0, progressbar_object ,*progressbar_function);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[11]=0;
			Response[18]=0;
			Response[30]=0;
		}
		return Response;
	}


//////////////////////
////Query Setup QS////
//////////////////////

#pragma pack(push,1)
	/**
	 * Mask for QS command response container\n
	 * Showing setup information of the multimeter\n
	 * Packed in 8 bit wise via pragma\n
	 */
	typedef struct
	{
		/** Command acknowledge code (without termination)*/
		char I_CMD_ACK;
		/** Line feed char (/r) (without termination)*/
		char n_CR0;
		/** Holding "QS," (without termination) */
		char n_QSHeaderInfo_Comma[3];
		/**Interval for Logging Mode deciseconds*/
		unsigned int I_LogInterval : 16;
		/** dB Reference 1=V 0=mV*/
		unsigned int I_dBREFmV     : 8;
		/** @todo What does that do? (always 00?) */
		unsigned int u_unknown0	   : 8;
		/** dB Reference*/
		unsigned int I_dBREF	   : 16;
		/** Temperature Offset*/
				 int I_TempOffset  : 16;
		/** High if Celsius is selected, Low for Fahrenheit*/
		unsigned int I_DegC_nDegF  :  8;
		/** @todo Find out what that is used for (always 00?)*/
		unsigned int u_unkown1	   :  8;
		/** Backlite off time, deciseconds*/
		unsigned int I_BL_Off	   : 16;
		/** Current time of the multimeter in deciseconds from day 0*/
		unsigned int I_MMTime	   : 32;
		/** Power off time in minutes*/
		unsigned int I_Power_Off   : 16;
		/** Frequency Setting 0=50Hz 1=60Hz*/
		unsigned int I_Frequency   :  8;
		/** @todo Find out what that is used for (always 00?)*/
		unsigned int u_unknown2	   :  8;
		/** Digits setting (0=5 digits , 1=4 digits)*/
		unsigned int I_4Digits     :  8;
		/** @todo Find out what that is used for (always 00?)*/
		unsigned int u_unknown3	   :  8;
		/** Beep setting (0=off, 1=on)*/
		unsigned int I_Beep_On	   :  8;
		/** @todo Find out what the ending is used for*/
		unsigned int u_ending1	   : 32;
		/** @todo Find out what the ending is used for*/
		unsigned int u_ending2	   : 16;
	} cmdr_QS_t;
#pragma pack(pop)

	/**Type of the container for QS command*/
	typedef SerialResponse<cmdr_QS_t> RCT_QS;


	/**
	 * Executing this command will send the QS command to the device
	 * @param [in] TerminatedSubstrings If \b true this will insert string terminations for the header information in the container
	 *
	 *
	 * @return Container from type RCT_QS
	 */
	RCT_QS CMD_QS(bool TerminatedSubstrings,void* progressbar_object ,void (*progressbar_function)(void* progressbar_object ,unsigned int current_byte, unsigned int byte_amount))
	{
		RCT_QS Response = this->SendCommand<cmdr_QS_t>("QS\r",this->DelayResponse_us, this->DelayChar_us, 34, 0, 0, false, 0, progressbar_object ,*progressbar_function);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}

/////////////////////
/////QUERY DATA//////
/////////////////////

#pragma pack(push,1)
	/**
	 * QD Information Field
	 * Holds information over various settings and states.
	 * It's part of the function responses of QD0 QD2 and QD4
	 */
	typedef struct
	{
				/**High if Auto Hold*/
				bool I_S_Auto		 		 :1;
				/**High if Hold or Auto Hold*/
				bool I_S_Hold				 :1;
				/**High if Log Mode*/
				bool I_S_Log				 :1;
				/**High if MinMax:AVG (also Fast)*/
				bool I_S_Avg		 		 :1;
				/**High if MinMax:MAX (also Fast)*/
				bool I_S_Max		 		 :1;
				/**High if MinMax:MIN (also Fast)*/
				bool I_S_Min		 		 :1;
				/**High if Fast MinMax*/
				bool I_S_Fast		 	     :1;
				/**High if LowBattery is on*/
				bool I_LowBattery	 		 :1;
				/**Seems to be one when in Setup or in ViewMem*/
				bool I_SetupOrViewMem 		 :1;
				/**High if Delta is on*/
				bool I_S_Delta		 		 :1;
				/**High if Percent is on*/
				bool I_S_Percent     		 :1;
				/** In ViewMem its 3 if there are saves or logs, if not its 00, if Clr? is displayed its 1*/
		unsigned int I_MEMclr		 		 :2;
				/**High when Auto Range*/
		unsigned int I_AutoRange	 		 :1;
				/**High when Manual Range*/
		unsigned int I_ManualRange			 :1;
				/**Is 1 as long the Shift sign is displayed in screen [__]*/
				bool I_ShiftDisplayed        :1;
				/**Is 1 as long the Shift sign is displayed in screen [__]*/
		unsigned int I_DBREF_Set	 		 :16;
				/**Mode (ACV DCV etc...)*/
		unsigned int I_MeasureMode	 		 :6;
				/**(Hz=1 %=2 ms=3)*/
		unsigned int I_Hz_Percent_ms 		 :2;
				/**High if MinMax Mode(also Fast)*/
				bool I_MinMaxMode	 		 :1;
				/**High if Fast*/
				bool I_Fast			  		 :1;
				/** @todo Find a use for this: always 0 (Maybe Cal?)*/
				bool u_bit1 		 		 :1;
				/**(1=Max 2=Min 3=AVG)*/
		unsigned int I_MinMaxAvg	 		 :2;
				/** @todo Find a use for this: always 0 (Maybe Cal?)*/
				bool u_bit2			 		 :1;
				/**High if rising etch displayed*/
				bool I_RisingEtch	 		 :1;
				/**High if falling etch displayed*/
				bool I_FallingEtch	 		 :1;
				/**Substates of V mV mA uA (DC Modes ONLY)*/
		unsigned int I_SubState_ACDC 		 :2;
				/** @todo Find a use for this: always 0 (Maybe Cal?)*/
				bool u_bit3		 	  		 :1;
				/**High if Hold     (Low on AutoHold)*/
				bool I_Hold			   		 :1;
				/**High if AutoHold (Low on Hold)*/
				bool I_AutoHold		 	 	 :1;
				/** @todo Find a use for this: always 0 (Maybe Cal?)*/
				bool u_bit4			 		 :1;
				/**High if V selected for dB_Ref, Low if m selected*/
				bool I_dBRef_V_nm	 		 :1;
				/**High if Celsius is selected, Low for Fahrenheit*/
				bool I_DegC_nDegF	 		 :1;
				/**High if Delta(Low if Delta Percent)*/
				bool I_Delta				 :1;
				/**High if DeltaPercent(Low if Delta)*/
				bool I_DeltaPercent			 :1;
				/** @todo Find a use for this: always 0 (Maybe Cal?)*/
				bool u_bit5					 :1;
				/**High if(Prim is dB and Sec AC) only V and mV (AC)*/
				bool I_Unit_dB_AC			 :1;
				/**High if(Prim is AC and Sec dB) only V and mV (AC)*/
				bool I_Unit_AC_dB			 :1;
				/** @todo Find a use for this: always 0 (Maybe Cal?)*/
				bool u_bit6					 :1;
				/**Only four digits selected in setup when 1*/
				bool I_FourDigitMode		 :1;
				/**
				 * Bit is one when Range is displayed (manual or auto)
				 * I am a bit unsure about this..
				 * Bit is 0 when in Setup and in Temperature Mode or displaying Questions/Status
				 * (Range Selection changeable when 1?)
				 */
				bool I_RangeDisplayed	 	 :1;
				/**Range Mode*/
		unsigned int I_SelectedRange 	 	 :8;
				/**Displays Data of what is on screen currently 1=Measure, 2=Setup, 22=Setup(Timesetting)*/
		unsigned int I_CurrentView   		 :8;
	} qdInfo_t;
#pragma pack(pop)


/////////////////////////////////
//Query Data 0: Current reading//
/////////////////////////////////

#pragma pack(push,1)
	/**
	 * Mask for QD0 command response container\n
	 * Showing setup information of the multimeter\n
	 * Packed in 8 bit wise via pragma\n
	 */
	typedef struct{
		/** Command acknowledge code (without termination)*/
		char I_CMD_ACK;
		/** Line feed char (/r) (without termination)*/
		char n_CR0;
		/** Holding "QS," (without termination) */
		char n_QDHeaderInfo_Comma[3];


		unsigned int I_Time0				 :32; ///<Time(1st occurrence)
		union
		{
		  signed int I_priValue0			 :32; ///<Primary Value	 (1st occurrence)
		  struct
		  {
			unsigned int I_ErrorNoPV0		 :8;  ///<8 bit error code (if error == 1)
			unsigned int 					 :22; ///<unneeded bits for error
			unsigned int I_ErrorPV0			 :2;  ///<error when value == 1 (no error: when 3 (negative) or 0 (positive))
		  };
		};
		unsigned int I_priDecimal0  		 :8;  ///<Primary decimal point location(1st occurrence)
		  signed int I_priSI_Prefix0		 :8;  ///<Primary SI-Prefix (1st occurrence)
		union
		{
		  signed int I_secValue0    		 :32; ///<Secondary display value
		  struct
		  {
			unsigned int I_ErrorNoSV0		 :8;  ///<8 bit error code (if error == 1)
			unsigned int 					 :22; ///<unneeded bits for Error
			unsigned int I_ErrorSV0 		 :2;  ///<Error when value == 1 (no error: when 3 (negative) or 0 (positive))
		   };
		 };
		unsigned int I_secDecimal			 :8;  ///<Secondary decimal point location
		  signed int I_secSi_Prefix	 		 :8;  ///<Secondary SI-Prefix @todo Check if it is different from first any time...

		union
		{
		  signed int I_secValue1     	     :32; ///<Secondary display value (2. occurrence)
		  struct
		  {
			unsigned int I_ErrorNoSV1 	     :8;  ///<8 bit Error code (if error == 1)
			unsigned int 					 :22; ///<unneeded bits for error
			unsigned int I_ErrorSV1 		 :2;  ///<Error when value == 1 (no error: when 3 (negative) or 0 (positive))
		   };
		 };

		unsigned int I_Time1				 :32; ///<Time(2nd occurrence)

		union
		{
		  signed int I_priValue1			 :32; ///<Primary Value	 (2nd occurrence)
		  struct
		  {
			unsigned int I_ErrorNoPV1	 	 :8;  ///<8 bit error code (if error == 1)
			unsigned int 					 :22; ///<unneeded bits for Error
			unsigned int I_ErrorPV1 		 :2;  ///<Error when value == 1 (no error: when 3 (negative) or 0 (positive))
		  };
		};

		unsigned int I_priDecimal1  		 :8;  ///<Primary decimal point location (2nd occurrence) This is 0xf when in ViewMem with no Logs "-----"
		  signed int I_priSI_Prefix1 		 :8;  ///<Primary SI-Prefix	(2nd occurrence)

		  qdInfo_t I_QDInfo;					  ///<Settings information

		unsigned int u_byte0         		 :8;  ///<@todo Find out what that is for //always 1
		unsigned int u_byte1         		 :8;  ///<@todo Find out what that is for //always 0
	} cmdr_QD0_t;
#pragma pack(pop)

	/**
	 * Type of the container for the QD0 command
	 */
	typedef SerialResponse<cmdr_QD0_t> RCT_QD0;


	RCT_QD0 CMD_QD0(bool TerminatedSubstrings, void* progressbar_object ,void (*progressbar_function)(void* progressbar_object ,unsigned int current_byte, unsigned int byte_amount))
	{
		RCT_QD0 Response = this->SendCommand<cmdr_QD0_t>("QD 0\r",this->DelayResponse_us, this->DelayChar_us, 47, 0, 0, false, 0, progressbar_object ,*progressbar_function);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}





//////////////////////////
//Query Data 2: Get logs//
//////////////////////////

#pragma pack(push, 1) //Pragma packing width 1

	/**
	 * This is a data mask for the repetitive information in the QD2 response
	 * Packed in 8 bit wise via pragma\n
	 */
	typedef struct
	{
		unsigned int I_TimeBeg	 :32; 	///<Beginning timestamp for entry in units of 0.1s
        unsigned int I_Decimal   :8; 	///<Decimal point location
        signed int I_SI_Prefix	 :8;    ///<Units.  -1 for milli, 1 for kilo, etc.
        signed int I_MinValue    :32;	///<Lowest value for this time period
        signed int I_MaxValue    :32;	///<Maximum value for this time period
        signed int I_SummedValue :32;	///<Actual value, sum of N
        unsigned int u_byte0 	 :32;   ///<?, always all 0 in my tests
        unsigned int I_NumberSum :32;   ///<Number of values which were summed
        unsigned int I_Status	 :8;    ///<Status - 0x05 for normal, 0x85 for last entry, other //values for range changes or other oddities
        unsigned int u_byte1     :8;	///<? - always 0x01 in my tests
        unsigned int I_TimeEnd	 :32; 	///<Ending timestamp for this entry
     }qd2_set_t;

    /**
     * This is the complete data mask for the QD2 response
     */
	typedef struct
	{
		char 		 I_CMD_ACK;               ///< Command acknowledgement
		char 		 n_CR0;					  ///< Carriage return
		char		 n_QDHeaderInfo_Comma[3]; ///< holds QD,

		unsigned int I_DataCount       	 :16; ///< number of stored SAVE data sets

		unsigned int I_InitalValue   	 :32; ///< Measurement value at startup
		unsigned int I_InitDecimal	 	 :8;  ///< Decimal point location for the startup measurement
		  signed int I_InitPrefix    	 :8;  ///< Prefix for the startup measurement

		  qdInfo_t I_QDInfo;				  ///< Struct for standard status informations

		qd2_set_t dsets[1];					  ///< Struct for a single data set
	} cmdr_QD2_t;

#pragma pack(pop) //Reset pack pragma

	/**
	 * Type of the container for QD2 command
	 */
	typedef  SerialResponse<cmdr_QD2_t> RCT_QD2;


	RCT_QD2 CMD_QD2(bool TerminatedSubstrings, void* progressbar_object ,void (*progressbar_function)(void* progressbar_object ,unsigned int current_byte, unsigned int byte_amount))
	{
		RCT_QD2 Response = this->SendCommand<cmdr_QD2_t>("QD 2\r",this->DelayResponse_us, this->DelayChar_us, 23, 6, 2, false, 32, progressbar_object ,*progressbar_function);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}

//////////////////////////////////
//Query Data 4: Get Single Saves//
//////////////////////////////////

#pragma pack(push, 1) //Pragma packing width 1
	/**
	 * Data mask of the container for QD4 command data sets
	 * (packed 8bit wise)
	 */
	typedef struct
			{
				unsigned int I_Time0		 :32; ///<Time(1st occurrence)
				union
				{
				  signed int I_priValue0	 :32; ///<Primary value (1st occurrence)
				  struct
				  {
					unsigned int I_ErrorNoPV0:8;  ///<8 bit error code (If error == 1)
					unsigned int 			 :22; //unneeded for error
					unsigned int I_ErrorPV0	 :2;  ///<Error when value == 1
				  };
				};
				unsigned int I_priDecimal0   :8;  ///<Primary decimal point location(1st occurrence)
				  signed int I_priSI_Prefix0 :8;  ///<Primary SI-prefix 	 (1st occurrence)
				union
				{
				  signed int I_secValue		 :32; ///<Secondary value
				  struct
				  {
					unsigned int I_ErrorNoSV :8;  ///<8 bit error code (If error == 1)
					unsigned int 			 :22; //unneeded bits for Error
					unsigned int I_ErrorSV	 :2;  ///<Error when value == 1
				   };
				 };
				unsigned int I_secDecimal	 :8;  ///<Secondary value decimal point location
				  signed int I_secSi_Prefix	 :8;  ///<Secondary value prefix
				unsigned int u_unknown0		 :16; ///<@todo FIND OUT::Most times 0x0013 sometimes 0x0001 or 0x0002 WTF?
				unsigned int u_unknown1		 :16; ///<@todo Make it change to find its purpose
				unsigned int I_Time1		 :32; ///<Time (2nd occurrence)
				union
				{
				  signed int I_priValue1	 :32; ///<Primary value	 (2nd occurrence)
				  struct
			      {
				    unsigned int I_ErrorNoPV1:8;  ///<8 bit error code (If error == 1)
					unsigned int 			 :22; //unneeded bits for error
					unsigned int I_ErrorPV1	 :2;  ///<Error when value == 1
				  };
				};
				unsigned int I_priDecimal1	 :8;  ///<Primary decimal point location (2nd occurrence)

				  signed int I_priSI_Prefix1 :8;  ///<Primary SI-prefix (2nd occurrence)

				  qdInfo_t I_QDInfo;			 ///< Standard settings and mode information

				unsigned int u_unknown2	    :8;  ///<@todo Find out how to change this always 0x01
				unsigned int I_DataEnding	:8;	 ///<@todo Find a way to calculate this ending byte (always the same series)
			}qd4_set_t;

	/**
	 * This is the main mask for the QD4 container
	 */
	typedef struct
	{
		char I_CMD_ACK;					///< Command Acknowledge
		char n_CR0;						///< Carriage Return
		char n_QDHeaderInfo_Comma[3];   ///< QD,
		unsigned int I_DataCount:8;     ///< Amount of following data sets
		qd4_set_t dsets[1]; 			///< Data set array
	} cmdr_QD4_t;
	#pragma pack(pop) //Reset pack pragma


	/**
	 * Type of the QD4 container
	 */
	typedef  SerialResponse<cmdr_QD4_t> RCT_QD4;

	RCT_QD4 CMD_QD4(bool TerminatedSubstrings, void* progressbar_object ,void (*progressbar_function)(void* progressbar_object ,unsigned int current_byte, unsigned int byte_amount))
	{
		RCT_QD4 Response = this->SendCommand<cmdr_QD4_t>("QD 4\r",this->DelayResponse_us, this->DelayChar_us, 6, 6, 1, false, 42, progressbar_object ,*progressbar_function);
		if(TerminatedSubstrings)//Insert String Terminators by overwriting unneeded stuff like commas and returns etc...
		{
			Response[1]=0;
			Response[4]=0;
		}
		return Response;
	}

////////////////////////
//Fluke Protocol Enums//
////////////////////////

	/**
	 * Command Acknowledge Values
	 */
	enum CMD_ACK
	{
		CA_ERR_ACK   =  '0', ///< Acknowledge (no Error)
		CA_ERR_FAIL  =  '1', ///< Error
		CA_ERR_NODATA=  '5', ///< This is returned when executing a QD for logs or saves and the memory is empty
	};

	/**
	 * Ranges of the multimeter
	 */
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

	/**
	 * Range modes of the multimeter
	 */
	enum RangeMode
	{
		RANGEMODE_AUTO	=0xA0,
		RANGEMODE_MANUAL=0xC0,
		RANGEMODE_NONE	=0x80,
	};

	/**
	 * Display Errors
	 */
	enum ValueError
	{
		VE_Display_OFFLINE			=0x01,
		VE_OPEN__NOTHING_CONNECTED	=0x16,
		VE_FUSE						=0x1B,
		VE_LEADS_CONNECTION_WRONG	=0x1D,
		VE_OL_OUTOFRANGE_NOCON		=0x21,
		//Usage in class functions only:
		VE_NO_ERROR					=0,
		VE_NOT_APPLICABLE			=0xFF,
	};

	/**
	 * Different Measure modes
	 */
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

	/**
	 * The return values for what is shown on the multimeter
	 */
	enum CurrentView
	{
		CV_DisplayingMeasure=0x01, ///< Just displaying a reading
		CV_InSetup_noTimeSet=0x02, ///< Displaying setup screen (A time setting: intervall, poweroff etc)
	    CV_InSetup_TimeSet=0x22,   ///< Displaying setup screen (no time setting)
	    CV_Showing_CLR_Saves=0x03, ///< Clr? (saves) is displayed on screen
	    CV_Showing_CLR_Log=0x05,   ///< Clr? (log) is displayed on screen
	    CV_ViewMem_NoData=0x06,    ///< ViewMem with no data is displayed
	};



private:
	//Private Attributes
	unsigned int DelayChar_us;
	unsigned int DelayResponse_us;

public:

	/**
	 * Constuctor for class Fluke189
	 * @param[in] filename Interface file name
	 */
	Fluke189(std::string filename);

	/**
	 * Destructor for class Fluke189
	 * (empty)
	 */
	virtual ~Fluke189();

private:
	void SetupInterface( void );

};

class Fluke189DataResponseAnalyzer;

/**
 * This Wrapper is the superclass for the different data sets.
 * The class to creating a subclass of this is only used to store the current SerialResponseContainer and its type.
 * The member functions of this Wrapper are called by Fluke189ResponseAnalyser::operator[](unsigned int)
 * This class is a superclass so it can not be constructed.
 */
class Fluke189DataResponseAnalyzerWrapper
{
protected:

	friend class Fluke189DataResponseAnalyzer;
	unsigned int datasetnumber; ///<helds the number of the current data set
	void* currentContainer;     ///<helds the address of a supported container type

public:




	/**
	 * Constructor
	 * param[in] datasetnumber Number of the data set to be edited
	 * param[in] currentContainer Points to the current data set container
	 */
	Fluke189DataResponseAnalyzerWrapper(unsigned int datasetnumber, void* currentContainer)
	: datasetnumber(datasetnumber), currentContainer(currentContainer){};

	~Fluke189DataResponseAnalyzerWrapper(){};


	/***************
	 * DATA STRUCTS*
	 ***************/


	/**
	 * Return values of the analysis for the position of the mode switch
	 */
	enum ModeSwitchSetting
	{
		MS_Unknown			=0,
		MS_VAC				=1,
		MS_mVAC				=2,
		MS_VDC				=3,
		MS_mVDC				=4,
		MS_OhmSiemens		=5,
		MS_CapDiode			=6,
		MS_Temp				=7,
		MS_AACmAAC			=8,
		MS_uAAC				=9,
		MS_ADCmADC			=10,
		MS_uADC				=11,
		MS_VIEWMEM			=12,
		MS_STUCKBETW2POS  	=99,
	};

	/**
	 * Returned units by analysis
	 */
	enum Unit
	{
		AU_None,

		AU_AC_V,
		AU_DC_V,
		AU_ACDC_V,

		AU_Volts,

		AU_Ampere,

		AU_dBm,
		AU_dB_V,

		AU_Hz,
		AU_Seconds,

		AU_Percent,

		AU_Ohm,
		AU_Siemens,

		AU_Farad,

		AU_Celsius,
		AU_Fahrenheit,

	};

	/**
	 * Voltage Types (AC, DC, AC+DC)
	 */
	enum CurrentType
	{
		ACT_NoCurrentType,
		ACT_AlternatingCurrent,
		ACT_DirectCurrent,
		ACT_DirectandAlternatingCurrent
	};

	/**
	 * Etch information (falling etch, rising etch, not applicable)
	 */
	enum Etch
	{
		AE_NOT_APPLICABLE,
		AE_FALLING_ETCH,
		AE_RISING_ETCH,
	};

	/**
	 * This struct is returned from Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo and holds
	 * additional data which results from qdInfo_t analysis
	 */
	typedef struct
	{
		//Units information

		//primary
		        Unit i_priUnit;			///<Unit (primary Display) (enum Unit)
		 std::string s_priUnit;			///<Unit string(primary Display)
		 CurrentType i_priCurrentType;	///<Integer number for current type AC DC AC+DC(primary Display)
	     std::string s_priCurrentType;	///<String for current type(primary Display)

		//secondary
                Unit i_secUnit;			///<Integer number for unit (sec. Disp) (enum Unit)
		 std::string s_secUnit;			///<Unit string(sec. Disp)
		 CurrentType i_secCurrentType;	///<Integer number for current type AC DC AC+DC(sec. Disp)
		 std::string s_secCurrentType;	///<string for current type(sec. Disp)

		//Special Modes
			    bool b_Logging;			///<True if currently logging
			    bool b_ViewMem;			///<True if in ViewMem (and CLR is not displayed)
			    bool b_ModeSwitchERR;	///<True if switch is stuck between two modes
   ModeSwitchSetting i_ModeSwitchPos;   ///<Position of mode switch

	} analyzedInfo_t;



	/********************************************
	 * BASE FUNCTIONS USEABLE FOR ALL CONTAINERS*
	 ********************************************/

	/**
	 * Returns a struct with additional information created by analyzing qdInfo.\n
	 * It containes the primary and secondary units, current types and shows if the multimeter is currently in logging or viewmem mode\n
	 * <b>ATTENTION:</b> Currently its not possible to be sure that the multimeter is in viewmem. If "Clr?" is displayed the return value will
	 * fall back to the mode which was selected when the log/save was stored.
	 * @todo find a fix for the recognize viemem correctly bug
	 * @param[in] qdInfo Struct containing setup for the current reading
	 * @return Struct containing additional information.
	 */
	analyzedInfo_t analyzeQdInfo(Fluke::Fluke189::qdInfo_t* qdInfo);

	/**
	 * Returns the error string according to the ValueError number
	 * @param[in] number ValueError
	 * @return A human readable error string
	 */
	std::string valueErrorToString(Fluke::Fluke189::ValueError number);




	/********************
	 * VIRTUAL FUNCTIONS*
	 ********************/


	/**
	 * Checks if a error in measurement is present in the current primary reading
	 * @param[in] reading2 If <b>true</b> second primary reading will be used.
	 * @return <b>true</b> if the primary display value has an error
	 */
	virtual bool hasErrorPRIdisplay(bool reading2) = 0;

	/**
	 * Checks if a error in measurement is present in the current secondary reading
	 * @param[in] reading2 If <b>true</b> second primary reading will be used.
	 * @return <b>true</b> if the secondary display value has an error
	 */
	virtual bool hasErrorSECdisplay(bool reading2) = 0;

	/**
	 * This function returns the ErrorValue of the primary reading
	 * @param[in] reading2 If <b>true</b> second primary reading will be used.
	 * @return Error numbers according to Fluke189::ValueError
	 */
	virtual Fluke189::ValueError get_PRIdisplayError(bool reading2) = 0;

	/**
	 * This function returns the ErrorValue of the secondary reading
	 * @param[in] reading2 If <b>true</b> second secondary reading will be used (QD0 only).
	 * @return Error numbers according to Fluke189::ValueError
	 */
	virtual Fluke189::ValueError get_SECdisplayError(bool reading2) = 0;

	/**
	 * Shows the current mode switch setting
	 * @return Number of mode switch or 0 when stuck between two positions
	 */
	virtual ModeSwitchSetting get_ModeSwitchSetting() = 0;

	/**
	 * Get physical unit of the primary reading.
	 * @return Physical unit of the current reading according to Fluke189DataResponseAnalyzerWrapper::enum Unit
	 */
	virtual Unit get_primaryUnit() = 0;

	/**
	 * Get physical unit of the secondary reading.
	 * @return Physical unit of the current reading according to Fluke189DataResponseAnalyzerWrapper::enum Unit
	 */
	virtual Unit get_secondaryUnit() = 0;

	/**
	 * Get current type of the primary reading (AC, DC, AC+DC, not applicable)
	 * @return CurrentType according to Fluke189DataResponseAnalyzerWrapper::CurrentType
	 */
	virtual CurrentType get_primaryCurrentType() = 0;

	/**
	 * Get current type of the secondary reading (AC, DC, AC+DC, not applicable)
	 * @return CurrentType according to Fluke189DataResponseAnalyzerWrapper::CurrentType
	 */
	virtual CurrentType get_secondaryCurrentType() = 0;

	/**
	 * Get current etch information (rising, falling, not applicable)
	 * @return etch information according to Fluke189DataResponseAnalyzerWrapper::Etch
	 */
	virtual Etch get_EtchInfo() = 0;
};

/**
 * This is the subclass which holds the real functions for analyzing the response of QD0 command
 * Can only be constructed from Fluke189ResponseAnalyser(private constructor)
 * The deconstruction is also done by Fluke189ResponseAnalyser (private destructor)
 */
class Fluke189DataResponseAnalyzerWrapperQD0 : public Fluke189DataResponseAnalyzerWrapper
{
	friend class Fluke189DataResponseAnalyzer;

	Fluke189DataResponseAnalyzerWrapperQD0(unsigned int datasetnumber, void* currentContainer)
	: Fluke189DataResponseAnalyzerWrapper(datasetnumber, currentContainer){};

	~Fluke189DataResponseAnalyzerWrapperQD0(){};

public:
	/**
	 * Checks if a error in measurement is present in the current primary reading
	 * @param[in] reading2 If <b>true</b> second primary reading will be used.
	 * @return <b>true</b> if the primary display value has an error
	 */
	bool hasErrorPRIdisplay(bool reading2);

	/**
	 * Checks if a error in measurement is present in the current secondary reading
	 * @param[in] reading2 If <b>true</b> second primary reading will be used.
	 * @return <b>true</b> if the secondary display value has an error
	 */
	bool hasErrorSECdisplay(bool reading2);

	/**
	 * This function returns the ErrorValue of the primary reading
	 * @param[in] reading2 If <b>true</b> second primary reading will be used.
	 * @return Error numbers according to Fluke189::ValueError
	 */
	Fluke189::ValueError get_PRIdisplayError(bool reading2);

	/**
	 * This function returns the ErrorValue of the secondary reading
	 * @param[in] reading2 If <b>true</b> second secondary reading will be used (QD0 only).
	 * @return Error numbers according to Fluke189::ValueError
	 */
	Fluke189::ValueError get_SECdisplayError(bool reading2);

	/**
	 * Shows the current mode switch setting
	 * @return Number of mode switch or 0 when stuck between two positions
	 */
	ModeSwitchSetting get_ModeSwitchSetting();


	/**
	 * Get physical unit of the primary reading.
	 * @return Physical unit of the current reading according to Fluke189DataResponseAnalyzerWrapper::enum Unit
	 */
	Unit get_primaryUnit();

	/**
	 * Get physical unit of the secondary reading
	 * @return Physical unit of the current reading according to  Fluke189DataResponseAnalyzerWrapper::enum Unit
	 */
	Unit get_secondaryUnit();



	/**
	 * Get current type of the primary reading (AC, DC, AC+DC, not applicable)
	 * @return CurrentType according to Fluke189DataResponseAnalyzerWrapper::CurrentType
	 */
	CurrentType get_primaryCurrentType();

	/**
	 * Get current type of the secondary reading (AC, DC, AC+DC, not applicable)
	 * @return CurrentType according to Fluke189DataResponseAnalyzerWrapper::CurrentType
	 */
	CurrentType get_secondaryCurrentType();

	/**
	 * Get current etch information (rising, falling, not applicable)
	 * @return etch information according to enum Fluke189DataResponseAnalyzerWrapper::Etch
	 */
	Etch get_EtchInfo();

};

/**
 * This class is used to analyze the response container.
 * \n Currently supported are: \n RCT_QD0, RCT_QD2, RCT_QD4 \n
 * All functions are implemented in a wrapper for operator[].\n
 * \n A common usage should look like this:\n
 * FlukeResponseAnalyser(container); \n
 * container[datasetnumber].function;\n
 * For containers which do not have repetitive information use:
 * datasetnumber = 0;
 * Other numbers will result to a cerr message and will be ignored.
 *
 * Header Informations in some containers are also called with operator[]
 * but the integer value of the function will be ignored.
 *
 */
class Fluke189DataResponseAnalyzer
{
	//This are ResponseContainerTypes for internal usage
	enum ResponseContainerType
	{
		QD0,
		QD2,
		QD4,
	};

	//This variable stores the current ResponseContainerType
	ResponseContainerType currentResponseContainerType;
	//This stores the address of the current container
	void* container;

	//This is vector stores the addresses to the created Fluke189ResponseAnalyserWrappers
	std::vector<Fluke189DataResponseAnalyzerWrapper*> delAtDeconstruction;

public:
	/** Constructor for a QD0 response container*/
	Fluke189DataResponseAnalyzer(Fluke189::RCT_QD0& container);
	/** Constructor for a QD2 response container*/
	Fluke189DataResponseAnalyzer(Fluke189::RCT_QD2& container);
	/** Constructor for a QD4 response container*/
	Fluke189DataResponseAnalyzer(Fluke189::RCT_QD4& container);


	/** The destructor for the class ResponseAnalyser will automatically delete all created wrappers at its own destruction*/
	virtual ~Fluke189DataResponseAnalyzer()
	{
		std::vector<Fluke189DataResponseAnalyzerWrapper*>::iterator itDel = this->delAtDeconstruction.begin();
		for(;itDel<this->delAtDeconstruction.end(); itDel++ )
		{
			if(*itDel != 0) delete *itDel;
		}
	};

	/**
	 * This function will return a Fluke189ResponseAnalyserWrapper which
	 * holds the analysis functions.
	 */
	Fluke189DataResponseAnalyzerWrapper* operator[](unsigned int datasetnumber)
	{
		Fluke189DataResponseAnalyzerWrapper* wrapper;
		if(this->currentResponseContainerType==QD0)
		{
			if(datasetnumber != 0)throw std::range_error("There is no repetitive data for this type!");
			wrapper=new Fluke189DataResponseAnalyzerWrapperQD0(datasetnumber, this->container);
			this->delAtDeconstruction.push_back(wrapper);
		}
		else
		{
			throw std::runtime_error("Unknown/not implemented container type");
		}

		 if(! *this->delAtDeconstruction.rend())
		 {
			 throw std::runtime_error("FlukeResponseAnalyserWrapper could not be created");
		 }

		return  wrapper;

	};



};









}

#endif /* FLUKE189_H_ */
