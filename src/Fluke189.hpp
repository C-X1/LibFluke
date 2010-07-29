/**
 * @mainpage
 * @todo Add Documentation to mainpage: Examples, Description etc....
 *
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
 * Initialization:\n
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
		  signed int I_priValue0			 :32; ///<Primary Value	 (1st occurrence)
		unsigned int I_priDecimal0  		 :8;  ///<Primary decimal point location(1st occurrence)
		  signed int I_priSI_Prefix0		 :8;  ///<Primary SI-Prefix (1st occurrence)
		  signed int I_secValue0    		 :32; ///<Secondary display value
		unsigned int I_secDecimal			 :8;  ///<Secondary decimal point location (Gets 128 in Hz Mode, If in Hz it should be 2 (always))
		  signed int I_secSi_Prefix	 		 :8;  ///<Secondary SI-Prefix @todo Check if it is different from first any time...
		  signed int I_secValue1     	     :32; ///<Secondary display value (2. occurrence)
		unsigned int I_Time1				 :32; ///<Time(2nd occurrence)
		  signed int I_priValue1			 :32; ///<Primary Value	 (2nd occurrence)
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
				  signed int I_priValue0	 :32; ///<Primary value (1st occurrence)
				unsigned int I_priDecimal0   :8;  ///<Primary decimal point location(1st occurrence)
				  signed int I_priSI_Prefix0 :8;  ///<Primary SI-prefix 	 (1st occurrence)
				  signed int I_secValue		 :32; ///<Secondary value
				unsigned int I_secDecimal	 :8;  ///<Secondary value decimal point location
				  signed int I_secSi_Prefix	 :8;  ///<Secondary value prefix
				unsigned int u_unknown0		 :16; ///<@todo FIND OUT::Most times 0x0013 sometimes 0x0001 or 0x0002 WTF?
				unsigned int u_unknown1		 :16; ///<@todo Make it change to find its purpose
				unsigned int I_Time1		 :32; ///<Time (2nd occurrence)
				  signed int I_priValue1	 :32; ///<Primary value	 (2nd occurrence)
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


	 ////////////////
	 //DATA STRUCTS//
	 ////////////////


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
		AU_None=0,

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
	 * Display Errors
	 */
	enum DispError
	{
		ERR_NONE				  =0,
		ERR_DISPLAY_OFF				,
		ERR_OPEN					,
		ERR_FUSE					,
		ERR_LEADS					,
		ERR_OUTOFRANGE_POSITIVE		,
		ERR_OUTOFRANGE_NEGATIVE		,
		ERR_UNKNOWN					,
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
		 CurrentType i_priCurrentType;	///<Integer number for current type AC DC AC+DC(primary Display)


		//secondary
                Unit i_secUnit;			///<Integer number for unit (sec. Disp) (enum Unit)
		 CurrentType i_secCurrentType;	///<Integer number for current type AC DC AC+DC(sec. Disp)


		//Special Modes
			    bool b_Logging;			///<True if currently logging
			    bool b_ViewMem;			///<True if in ViewMem (and CLR is not displayed)
			    bool b_ModeSwitchERR;	///<True if switch is stuck between two modes
   ModeSwitchSetting i_ModeSwitchPos;   ///<Position of mode switch


	} analyzedInfo_t;



	/////////////////////////////////////////////
	//BASE FUNCTIONS USEABLE FOR ALL CONTAINERS//
	/////////////////////////////////////////////

	/**
	 * Returns a struct with additional information created by analyzing qdInfo.\n
	 * It containes the primary and secondary units, current types and shows if the multimeter is currently in logging or viewmem mode\n
	 * <b>ATTENTION:</b> Currently its not possible to be sure that the multimeter is in viewmem. If "Clr?" is displayed the return value will
	 * fall back to the mode which was selected when the log/save was stored.
	 * @todo find a fix for the recognize viemem correctly bug
	 * @param[in] qdInfo Struct containing setup for the current reading
	 * @return Struct containing additional information.
	 */
	static analyzedInfo_t analyzeQdInfo(Fluke::Fluke189::qdInfo_t* qdInfo);

	/**
	 * Returns the error string according to the ValueError number
	 * @param[in] number ValueError
	 * @return A human readable error string
	 */
	static std::string valueErrorToString(DispError number);

	/**
	 * This function will extract a error number out of a multimeter value
	 * @param [in] value Value from Fluke189 data set
	 * @return Returns a Value from the DispError enum.
	 */
	static DispError valueExtractError(int value);

	/**
	 * This function will convert a currentType to a string
	 * @param [in] type CurrentType variable
	 * @param [in] symbol If true it will return a symbol instead of words
	 * @return Returns symbol or abbreviation of current type
	 */
	static std::string currentTypeToString(CurrentType type, bool symbol);

	/**
	 * This function will convert a Unit to a string
	 * @param [in] unit The unit enum variable
	 * @param [in] symbol If true it returns only the symbol (V instead of Volts)
	 * @return Returns the string describing the unit
	 */
	static std::string unitToString(Unit unit, bool symbol);

	/**
	 * This function will convert a Etch type variable to a string
	 * @param [in] etch The etch variable
	 * @param [in] symbol If true it returns only a symbole instead of a full string
	 * @return Returns a string for the current etch mode
	 */
	static std::string etchToString(Etch etch, bool symbol);

	/////////////////////
	//VIRTUAL FUNCTIONS//
	/////////////////////

	/**
	 * This function returns the analyzedInfo_t struct of the current value qdInfo_t struct
	 * Return Returns analyzedInfo_t struct
	 */
	virtual analyzedInfo_t getAnalyzedInfoStruct() = 0;

	/**
	 * Checks if a error in measurement is present in the current primary reading
	 * @return <b>true</b> if the primary display value has an error
	 */
	virtual bool hasErrorPRIdisplay() = 0;

	/**
	 * Checks if a error in measurement is present in the current secondary reading
	 * @return <b>true</b> if the secondary display value has an error
	 */
	virtual bool hasErrorSECdisplay() = 0;

	/**
	 * This function returns the ErrorValue of the primary reading
	 * @return Error numbers according to DispError
	 */
	virtual DispError get_PRIdisplayError() = 0;

	/**
	 * This function returns the ErrorValue of the secondary reading
	 * @return Error numbers according to DispError
	 */
	virtual DispError get_SECdisplayError() = 0;

	/**
	 * Get the current mode switch setting
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

	/**
	 * @return Returns the current physical unit symbol of primary value as string
	 */
	virtual std::string getPrimaryUnitSymbol() = 0;

	/**
	 * @return Returns the current physical unit symbol of secondary value as string
	 */
	virtual std::string getSecondaryUnitSymbol() = 0;

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
	 * This function returns the analyzedInfo_t struct of the current value qdInfo_t struct
	 * Return Returns analyzedInfo_t struct
	 */
	analyzedInfo_t getAnalyzedInfoStruct();

	/**
	 * Checks if a error in measurement is present in the current primary reading
	 * @return <b>true</b> if the primary display value has an error
	 */
	bool hasErrorPRIdisplay();

	/**
	 * Checks if a error in measurement is present in the current secondary reading
	 * @return <b>true</b> if the secondary display value has an error
	 */
	bool hasErrorSECdisplay();

	/**
	 * This function returns the ErrorValue of the primary reading
	 * @return Error numbers according to DispError
	 */
	DispError get_PRIdisplayError();

	/**
	 * This function returns the ErrorValue of the secondary reading
	 * @return Error numbers according to DispError
	 */
	DispError get_SECdisplayError();

	/**
	 * Get the current mode switch setting
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

	/**
	 * @return Returns the current physical unit symbol of primary value as string
	 */
	std::string getPrimaryUnitSymbol();

	/**
	 * @return Returns the current physical unit symbol of secondary value as string
	 */
	std::string getSecondaryUnitSymbol();




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


/**
 * This class will handle continous logging with the QD0 command
 * multiple QD0 commands
 */
class Fluke189QD0Logging
{
public: /*Types*/
	/**
	 * Struct to store value information in the min max variables
	 */
	typedef struct
	{
		signed   int Value;     ///<Value (without decimal)
		unsigned int Decimal;	///<Decimal point location
		signed   int Prefix;	///<Prefix
	}  minMaxAvgValueStorage_t;

	/**
	 * Struct for modes information
	 */
	typedef struct modes_t
	{
		unsigned int minmaxavg; ///< 0=NoMinMaxAvg 1=Max 2=Min 3=Avg
 		bool delta;				///< This is true when delta mode is used but not in delta percent mode
		bool deltapercent;	 	///< This is true when delta percent mode is used but not in delta mode
		bool autohold;			///< This is true in autohold mode but not in hold
		bool hold;				///< This is true in hold mode but not in auto hold
		Fluke189DataResponseAnalyzerWrapper::Etch etch; ///< This will show if there is rising or falling etch or neither

		/**
		 * Operator == for this struct type (std-behavior)
		 */
		bool operator==(modes_t modes)
		{
			return (this->minmaxavg    == modes.minmaxavg   &&
					this->delta        == modes.delta       &&
					this->deltapercent == modes.deltapercent&&
					this->etch		   == modes.etch        &&
					this->autohold     == modes.autohold    &&
					this->hold         == modes.hold
			);
		}
		/**
		 * Operator != for this struct type (std-behavior)
		 */
		bool operator!=(modes_t modes)
		{
			return !this->operator ==(modes);
		}
	} modes_t;

private:/*Variables*/

	/*
	 * Variables for storing minimum maximum and average value
	 */
	 minMaxAvgValueStorage_t current_pri, current_sec;

	 /*
	  * Error Variables
	  */
	 Fluke189DataResponseAnalyzerWrapper::DispError pri_error, sec_error;

    //Primary Display
		minMaxAvgValueStorage_t  pri_min;
		minMaxAvgValueStorage_t  pri_max;
		minMaxAvgValueStorage_t  pri_avg;
		long long pri_avg_ll;
		unsigned int pri_count;

		modes_t modes;

    //Secondary Display
		minMaxAvgValueStorage_t  sec_min;
		minMaxAvgValueStorage_t  sec_max;
		minMaxAvgValueStorage_t  sec_avg;
		long long sec_avg_ll;
		unsigned int sec_count;

	//Reset Variable
		bool pri_reset;
		bool sec_reset;

	/*
	 * Variables for storing current units
	 */
	 Fluke189DataResponseAnalyzerWrapper::Unit pri_unit, sec_unit;
	 Fluke189DataResponseAnalyzerWrapper::CurrentType pri_current, sec_current;
	 Fluke189DataResponseAnalyzerWrapper::Unit pri_unit_str, sec_unit_str;

public: /*Functions*/

	/**
	 * Constructor
	 */
	Fluke189QD0Logging()
	{
		this->pri_unit=Fluke189DataResponseAnalyzerWrapper::AU_None;
		this->pri_unit=Fluke189DataResponseAnalyzerWrapper::AU_None;
	};
	/**
	 * Deconstructor (stub)
	 */
	~Fluke189QD0Logging(){};



private:

    /*
     * Check if operandSmall is smaller than operandBig
     * needed to get
     */
	bool fluke189ValueSmallerThan(minMaxAvgValueStorage_t operandSmall,minMaxAvgValueStorage_t operandBig)
	{

		//Calculation of decimal point location:
		//each step in prefix shifts the decimal point 3 times in a direction
		int decimalPosSmall=(-3)*(operandSmall.Prefix)+operandSmall.Decimal;
		int decimalPosBig  =(-3)*(operandBig.Prefix)+operandBig.Decimal;


		long long big,small; //Stores all values in piko prefix

		big  =operandBig.Value*pow(10, 13-decimalPosBig);
		small=operandSmall.Value*pow(10, 13-decimalPosSmall);

		return (small<big);
	}




public:

	/**
	 * This function converts a minMaxAvgValueStorage_t to a string with dot and prefix
	 * @param [in] value The value storage
	 * @return Returns a standard string of the value
	 */
	std::string minMaxAvgValueStorageToString(minMaxAvgValueStorage_t value);

	 /**
	  * This function will add the container to the calculated min, max and average values
	  * Internally its the function which does the most important work, calculating etc.
	  * @param [in] container The container object
	  */
	 void addContainer(Fluke189::RCT_QD0& container);

	 /**
	  * This function will reset primary min max and avg values
	  * @todo: Implement in add Container
	  */
	 void reset_primary()
	 {
		 this->pri_reset=true;
	 }

	 /**
	  * This function will reset secondary min max and avg values
	  *	@todo Implement in add Container
	  */
	 void reset_secondary()
	 {
		 this->sec_reset=true;
	 }

	 /**
	  * @return This function will return the internal variable pri_min.
	  */
	 minMaxAvgValueStorage_t get_Primary_Minimum()
	 {
		 return this->pri_min;
	 };

	 /**
	  * @return This function will return the internal variable pri_max.
	  */
	 minMaxAvgValueStorage_t get_Primary_Maximum()
	 {
		 return this->pri_max;
	 }

	 /**
	  * @return This function will return the internal variable pri_avg.
	  */
	 long long get_Primary_Average_LL()
	 {
		 return this->pri_avg_ll;
	 }

	 /**
	  * @return This function will return the internal variable sec_min.
	  */
	 minMaxAvgValueStorage_t get_Secondary_Minimum()
	 {
		 return this->sec_min;
	 }

	 /**
	  * @return This function will return the internal variable sec_max.
	  */
	 minMaxAvgValueStorage_t get_Secondary_Maximum()
	 {
		 return this->sec_max;
	 }

	 /**
	  * @return This function will return the internal variable sec_avg.
	  */
	 long long get_Secondary_Average_LL()
	 {
		 return this->sec_avg_ll;
	 }

	 /**
	  * @return This function will return the unit (type: Fluke189DataResponseAnalyzerWrapper::Unit) value of the internal variable.
	  */
	 Fluke189DataResponseAnalyzerWrapper::Unit get_Primary_Unit()
	 {
		 return this->pri_unit;
	 }

	 /**
	  * @return This function will return the unit (type: Fluke189DataResponseAnalyzerWrapper::Unit) value of the internal variable.
	  */
	 Fluke189DataResponseAnalyzerWrapper::Unit get_Secondary_Unit()
	 {
		 return this->sec_unit;
	 }

	 /**
	  * @return This function will return the modes variable
	  */
	 modes_t get_Modes()
	 {
		 return this->modes;
	 }

	 /**
	  * @return Returns a String containing the primary value with dot, prefix and physical unit.
	  */
	 std::string get_Primary_ValueAndUnit_String();

	 /**
	  * @return Returns a String containing the primary max value with dot, prefix and physical unit.
	  */
	 std::string get_Primary_Max_ValueAndUnit_String();

	 /**
	  * @return Returns a String containing the primary min value with dot, prefix and physical unit.
	  */
	 std::string get_Primary_Min_ValueAndUnit_String();

	 /**
	  * @return Returns a String containing the primary avg value with dot, prefix and physical unit.
	  */
	 std::string get_Primary_Avg_ValueAndUnit_String();


	 /**
	  * @return Returns a String containing the Secondary value with dot, prefix and physical unit.
	  */
	 std::string get_Secondary_ValueAndUnit_String();

	 /**
	  * @return Returns a String containing the Secondary max value with dot, prefix and physical unit.
	  */
	 std::string get_Secondary_Max_ValueAndUnit_String();

	 /**
	  * @return Returns a String containing the Secondary min value with dot, prefix and physical unit.
	  */
	 std::string get_Secondary_Min_ValueAndUnit_String();

	 /**
	  * @return Returns a String containing the Secondary avg value with dot, prefix and physical unit.
	  */
	 std::string get_Secondary_Avg_ValueAndUnit_String();

};




}

#endif /* FLUKE189_H_ */
