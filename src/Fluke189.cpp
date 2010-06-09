/*
 * Fluke189.cpp
 *
 *  Created on: 06.01.2010
 *      Author: cyborg-x1
 */

#include "Fluke189.hpp"

using namespace LibSerial;
namespace Fluke {

	Fluke189::Fluke189() {
		SetupInterface();
	}

	Fluke189::Fluke189(std::string filename){
		SetupInterface();
		this->SetDeviceFilename(filename);
	}

	Fluke189::~Fluke189() {
		// TODO Auto-generated destructor stub
	}

	void Fluke189::SetupInterface( void )
	{
		//Set Device Communication Parameters
		//They never change, so lets fix them here.
			this->SetDeviceBaudRate(SerialPort::BAUD_9600);
			this->SetDeviceCharacterSize(SerialPort::CHAR_SIZE_8);
			this->SetDeviceStopBits(SerialPort::STOP_BITS_1);
			this->SetDeviceFlowControl(SerialPort::FLOW_CONTROL_NONE);
			this->SetDeviceParity(SerialPort::PARITY_NONE);
		//Setup optional information
			this->SetManufacturer("Fluke Corporation");
			this->SetDeviceName("Fluke 189 Logging Multimeter");
		//Setup delay to wait for multimeter response
			this->DelayChar_us=5000;
			this->DelayResponse_us=120000;
	}


	Fluke189::analysedInfo_t Fluke189AnalyseQdInfo(Fluke::Fluke189::qdInfo_t* qdInfo)
	{
		Fluke189::analysedInfo_t info;

		//Primary Unit and Secondary Unit
		switch(qdInfo->I_MeasureMode)
		{
		//Off or Empty MemoryView
		case 0:
			info.i_priUnit=Fluke189::AU_None;
			info.s_priUnit="";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		// V~  /  dB + V~  / V~ + dB
		case 1: case 2:
			if(qdInfo->I_Unit_dB_AC)
			{
				info.i_priUnit=(qdInfo->I_dBRef_V_nm)?Fluke189::AU_dB_V:Fluke189::AU_dBm;
				info.s_priUnit=(qdInfo->I_dBRef_V_nm)?"dB V":"dBm";
				info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=Fluke189::AU_Volts;
				info.s_secUnit="V";
				info.i_secCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_secCurrentType="~";
			}
			else if(qdInfo->I_Unit_AC_dB)
			{
				info.i_priUnit=Fluke189::AU_Volts;
				info.s_priUnit="V";
				info.i_priCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_priCurrentType="~";

				info.i_secUnit=(qdInfo->I_dBRef_V_nm)?Fluke189::AU_dB_V:Fluke189::AU_dBm;
				info.s_secUnit=(qdInfo->I_dBRef_V_nm)?"dB V":"dBm";
				info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
				info.s_secCurrentType="";
			}
			else
			{
				info.i_priUnit=Fluke189::AU_Volts;
				info.s_priUnit="V";
				info.i_priCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_priCurrentType="~";

				info.i_secUnit=Fluke189::AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
				info.s_secCurrentType="";
			}
		break;

		// V=
		case 3: case 4:
			info.i_priUnit=Fluke189::AU_Volts;
			info.s_priUnit="V";
			info.i_priCurrentType=Fluke189::ACT_DirectCurrent;
			info.s_priCurrentType="=";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		// V= Substates
		case 5: case 6:
			if(qdInfo->I_SubState_ACDC==1)
			{
				info.i_priUnit=Fluke189::AU_Volts;
				info.s_priUnit="V";
				info.i_priCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_priCurrentType="~";

				info.i_secUnit=Fluke189::AU_Volts;
				info.s_secUnit="V";
				info.i_secCurrentType=Fluke189::ACT_DirectCurrent;
				info.s_secCurrentType="=";
			}
			else if (qdInfo->I_SubState_ACDC==2)
			{
				info.i_priUnit=Fluke189::AU_Volts;
				info.s_priUnit="V";
				info.i_priCurrentType=Fluke189::ACT_DirectCurrent;
				info.s_priCurrentType="=";

				info.i_secUnit=Fluke189::AU_Volts;
				info.s_secUnit="V";
				info.i_secCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_secCurrentType="~";
			}
			else if (qdInfo->I_SubState_ACDC==3)
			{
				info.i_priUnit=Fluke189::AU_ACDC_V;
				info.s_priUnit="V";
				info.i_priCurrentType=Fluke189::ACT_DirectandAlternatingCurrent;
				info.s_priCurrentType="≃";

				info.i_secUnit=Fluke189::AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
				info.s_secCurrentType="";
			}
		break;

		//Ohm
		case 9: case 11:
			info.i_priUnit=Fluke189::AU_Ohm;
			info.s_priUnit="Ohm";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";
			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		//nS
		case 10:
			info.i_priUnit=Fluke189::AU_Siemens;
			info.s_priUnit="S";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		//Cap
		case 12:
			info.i_priUnit=Fluke189::AU_Farad;
			info.s_priUnit="F";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		//Diode
		case 13:
			info.i_priUnit=Fluke189::AU_DC_V;
			info.s_priUnit="V=";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		case 26:
			info.i_priUnit=Fluke189::AU_Celsius;
			info.s_priUnit="°C";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_Celsius;
			info.s_secUnit="°C";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;


		case 27:
			info.i_priUnit=Fluke189::AU_Fahrenheit;
			info.s_priUnit="°F";
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_Fahrenheit;
			info.s_secUnit="°F";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		case 14: case 15: case 16:
			info.i_priUnit=Fluke189::AU_Ampere;
			info.s_priUnit="A";
			info.i_priCurrentType=Fluke189::ACT_AlternatingCurrent;
			info.s_priCurrentType="~";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;


		case 17: case 18: case 19:
			info.i_priUnit=Fluke189::AU_Ampere;
			info.s_priUnit="A";
			info.i_priCurrentType=Fluke189::ACT_DirectCurrent;
			info.s_priCurrentType="=";

			info.i_secUnit=Fluke189::AU_None;
			info.s_secUnit="";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;

		//A= Substates
		case 20: case 22: case 21:
			if(qdInfo->I_SubState_ACDC==1)
			{
				info.i_priUnit=Fluke189::AU_Ampere;
				info.s_priUnit="A";
				info.i_priCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_priCurrentType="~";

				info.i_secUnit=Fluke189::AU_Ampere;
				info.s_secUnit="A";
				info.i_secCurrentType=Fluke189::ACT_DirectCurrent;
				info.s_secCurrentType="=";
			}
			else if (qdInfo->I_SubState_ACDC==2)
			{
				info.i_priUnit=Fluke189::AU_Ampere;
				info.s_priUnit="A";
				info.i_priCurrentType=Fluke189::ACT_DirectCurrent;
				info.s_priCurrentType="=";

				info.i_secUnit=Fluke189::AU_Ampere;
				info.s_secUnit="A";
				info.i_secCurrentType=Fluke189::ACT_AlternatingCurrent;
				info.s_secCurrentType="~";
			}
			else if (qdInfo->I_SubState_ACDC==3)
			{
				info.i_priUnit=Fluke189::AU_Ampere;
				info.s_priUnit="A";
				info.i_priCurrentType=Fluke189::ACT_DirectandAlternatingCurrent;
				info.s_priCurrentType="≃";

				info.i_secUnit=Fluke189::AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
				info.s_secCurrentType="";
			}

		break;


		}
		//Hz % ms
		switch(qdInfo->I_Hz_Percent_ms)
		{
		case 1: //Hz
			info.i_secUnit=info.i_priUnit;
			info.s_secUnit=info.s_priUnit;
			info.i_secCurrentType=info.i_priCurrentType;
			info.s_secCurrentType=info.s_priCurrentType;
			info.i_priCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_priCurrentType="";

			info.i_priUnit=Fluke189::AU_Hz;
			info.s_priUnit="Hz";
		break;
		case 2:	//%
			info.i_priUnit=Fluke189::AU_Percent;
			info.s_priUnit="%";
			info.i_priCurrentType=Fluke189::ACT_DirectCurrent;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_Hz;
			info.s_secUnit="Hz";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";

		break;
		case 3: //ms
			info.i_priUnit=Fluke189::AU_Seconds;
			info.s_priUnit="s";
			info.i_priCurrentType=Fluke189::ACT_DirectCurrent;
			info.s_priCurrentType="";

			info.i_secUnit=Fluke189::AU_Hz;
			info.s_secUnit="Hz";
			info.i_secCurrentType=Fluke189::ACT_NoCurrentType;
			info.s_secCurrentType="";
		break;
		}

		//Min Max Avg Mode
		if(qdInfo->I_MinMaxMode)
		{
			//Secondary unit becomes primary unit(showing current value)
			info.i_secUnit=info.i_priUnit;
			info.s_secUnit=info.s_priUnit;
			info.i_secCurrentType=info.i_priCurrentType;
			info.s_secCurrentType=info.s_priCurrentType;
		}

		//Delta and Delta Percent
		if(qdInfo->I_Delta)
		{
			//Secondary unit becomes primary unit (showing start value)
			info.i_secUnit=info.i_priUnit;
			info.s_secUnit=info.s_priUnit;
			info.i_secCurrentType=info.i_priCurrentType;
			info.s_secCurrentType=info.s_priCurrentType;
		}
		else if(qdInfo->I_DeltaPercent)
		{
			//secondary unit becomes primary and primary changes to percent
			info.i_secUnit=info.i_priUnit;
			info.s_secUnit=info.s_priUnit;
			info.i_secCurrentType=info.i_priCurrentType;
			info.s_secCurrentType=info.s_priCurrentType;
			info.i_priUnit=Fluke189::AU_Percent;
			info.s_priUnit="%";
		}

		//Does the multimeter log new data atm?
		info.b_Logging=qdInfo->I_MEMclr != 3 && qdInfo->I_S_Log &&qdInfo->I_CurrentView != 6 ;

		//Are we in ViewMode? If no data CurrentView==6 else MEMclr==3 //TODO (change into enum symbols)
		info.b_ViewMem=qdInfo->I_CurrentView == 6 || qdInfo->I_MEMclr == 3;

		//Check if mode switch is stuck between two positions
		info.b_ModeSwitchERR=qdInfo->I_MeasureMode == 0 && qdInfo->I_CurrentView == 1;



		return info;
	}

 	std::string getFluke189ValueErrorString(unsigned int DisplayErrorNo)
 	{
 		std::string ErrorString;
		switch(DisplayErrorNo)
		{
		case Fluke189::VE_Display_OFFLINE:
			ErrorString = "-OFF-";
		break;

		case Fluke189::VE_LEADS_CONNECTION_WRONG:
			ErrorString = "LEADS";
		break;

		case Fluke189::VE_OL_OUTOFRANGE_NOCON:
			ErrorString = "OL";
		break;

		case Fluke189::VE_OPEN__NOTHING_CONNECTED:
			ErrorString = "OPEN";
		break;

		case Fluke189::VE_FUSE:
			ErrorString = "FUSE";
		break;

		default:
			ErrorString = "ERR_UNKNOWN";
		break;
		}

		return ErrorString;
 	}




	bool fluke189ValueSmallerThan(fluke189Value_t operandSmall,fluke189Value_t operandBig)
	{

		//Calculation of decimal point location:
		//each step in prefix shifts the decimal point 3 times in a direction
		int decimalPosSmall=(-3)*(operandSmall.intPrefix)+operandSmall.intDecimal;
		int decimalPosBig  =(-3)*(operandBig.intPrefix)+operandBig.intDecimal;

		//Note: DecimalPosSmall of a small value is bigger
		int decimaldiff=decimalPosSmall-decimalPosBig;
		if(decimaldiff==0)
		{
			return operandSmall.intValue < operandBig.intValue;
		}
		else if(decimaldiff < 0)
		{
			return (operandSmall.intValue / pow(10,decimaldiff))< operandBig.intValue;
		}
		else //decimaldiff>0
		{
			return (operandSmall.intValue * pow(10,decimaldiff)) < operandBig.intValue;
		}
	}

	std::string fluke189ValueToString(fluke189Value_t value)
	{
		std::string strvalue, valmem;

		std::stringstream convert_int;
		//Add Symbols before Value
		strvalue.append(value.strSymbolsBefore);
		//Insert space
		strvalue.append(" ");

		//Convert Value into string
		convert_int<<value.intValue;
		convert_int>>valmem;
		//Insert Decimal Point
		valmem.insert(valmem.length()-value.intDecimal,1,'.');
		//Append to Output
		strvalue.append(valmem);
		//Insert space
		strvalue.append(" ");
		//add Unit
		strvalue.append(&value.charUnit);

		//Add Symbols after Value
		strvalue.append(value.strSymbolsAfter);
		return strvalue;
	}

/*Namespace End*/}

