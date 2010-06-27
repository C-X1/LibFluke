/*
 * Fluke189.cpp
 *
 *  Created on: 06.01.2010
 *      Author: cyborg-x1
 */

#include "Fluke189.hpp"

using namespace LibSerial;
namespace Fluke {


	Fluke189::Fluke189(std::string filename){
		SetupInterface();
		this->SetDeviceFilename(filename);
	}

	Fluke189::~Fluke189() {}

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






	/*
	 * ResponseAnalyzer Functions
	 */
	Fluke189DataResponseAnalyzer::Fluke189DataResponseAnalyzer(Fluke189::RCT_QD0& container)
	{
		this->container=&container;
		this->currentResponseContainerType=QD0;
	}
	Fluke189DataResponseAnalyzer::Fluke189DataResponseAnalyzer(Fluke189::RCT_QD2& container)
	{
		this->container=&container;
		this->currentResponseContainerType=QD2;
	}
	Fluke189DataResponseAnalyzer::Fluke189DataResponseAnalyzer(Fluke189::RCT_QD4& container)
	{
		this->container=&container;
		this->currentResponseContainerType=QD4;
	}

 	std::string getFluke189ValueErrorString(unsigned int DisplayErrorNo) //TODO DELETE
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

	std::string fluke189ValueToString(fluke189Value_t value)
	{
		std::string strvalue, valmem;

		std::stringstream convert_int;

		//Insert space
		strvalue.append(" ");

		//Convert Value into string
		convert_int<<value.intValue;
		convert_int>>valmem;

		//if negative remove minus for the next step
		if(value.intValue<0)valmem.replace(0,1,"");


		//calculate insert location
		int insertlocation=valmem.length()-value.intDecimal;

		if(insertlocation <= 0)
		{
			//insert leading zeros and decimal point //limited to a maximum of 5 zeros (we won't need more afaik)
			for(int i=0;insertlocation+i<0 && i<5;i++)
			{
				valmem.insert(0,1,'0');
			}
			valmem.insert(0,1,'.');
			valmem.insert(0,1,'0');
		}
		else if(insertlocation >= 0)
		{
			//insert decimal point
			valmem.insert(insertlocation,1,'.');
		}

		//if negative insert minus again here
		if(value.intValue<0)valmem.insert(0,1,'-');

		//Append to Output
		strvalue.append(valmem);
		//Insert space
		strvalue.append(" ");
		//addPrefix





		switch(value.intPrefix)
		{
		case 0:break;

		case 1:
			strvalue.append("k");
		break;

		case 2:
			strvalue.append("M");
		break;

		case 3:
			strvalue.append("G");
		break;

		case -1:
			strvalue.append("m");
		break;

		case -2:
			strvalue.append("µ");
		break;

		case -3:
			strvalue.append("n");
		break;

		}


		//add Unit
		strvalue.append(value.strUnit);

		//Add Symbols before Value
		strvalue.insert(0,value.strSymbolsBefore);
		//Add Symbols after Value
		strvalue.append(value.strSymbolsAfter);



		return strvalue;
	}


	bool fluke189ValueSmallerThan(fluke189Value_t operandSmall,fluke189Value_t operandBig)
	{

		//Calculation of decimal point location:
		//each step in prefix shifts the decimal point 3 times in a direction
		int decimalPosSmall=(-3)*(operandSmall.intPrefix)+operandSmall.intDecimal;
		int decimalPosBig  =(-3)*(operandBig.intPrefix)+operandBig.intDecimal;


	    long long big,small; //Stores all values in piko prefix

	    big=operandBig.intValue*pow(10, 13-decimalPosBig);
	    small=operandSmall.intValue*pow(10, 13-decimalPosSmall);

		return (small<big);
	}

	void fluke189ValueMinMaxAverage(fluke189Value_t current, fluke189Value_t& min, fluke189Value_t& max, fluke189Value_t& avg, long long &stat_average, long &stat_datanumber, bool &reset,std::string &stat_unit)
	{
		//reset if unit (mode) changes...
		if(current.strUnit!=stat_unit)
		{
			reset=true;
			stat_unit=current.strUnit;
		}

		//Clear average on reset
		if(reset)
		{
			stat_average=0;
			stat_datanumber=0;
		}
		stat_datanumber++;

		//Max < Current
		if(Fluke::fluke189ValueSmallerThan(max,current)  || reset)
		{
			max=current;
		}
		//Min > Current
		if(Fluke::fluke189ValueSmallerThan(current,min) || reset)
		{
			min=current;
		}

		reset=false;

		stat_average=(stat_average*stat_datanumber+current.intValue*pow(10,13-(current.intPrefix*(-3)+current.intDecimal)))/(stat_datanumber+1);


		int highest_place_notzero;
		//get highest place of value which is not zero
		for(highest_place_notzero=18; (  abs(stat_average/pow(10,highest_place_notzero)) )==0 && highest_place_notzero >= 0; highest_place_notzero--)

		//Set prefix according highest part of value
		//if pico set it to nano
		avg.intPrefix=((highest_place_notzero/3)==0)? -3  : (highest_place_notzero/3)-4;
		avg.intDecimal=3;
		avg.intValue=(int)(stat_average/pow(10,13-(avg.intPrefix*(-3)+avg.intDecimal)));
		avg.strUnit=current.strUnit;
	}




	/*
	 * Fluke189ResponseAnalyzerWrapper functions
	 */


	Fluke189DataResponseAnalyzerWrapper::analyzedInfo_t Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo(Fluke::Fluke189::qdInfo_t* qdInfo)
	{
		Fluke189DataResponseAnalyzerWrapper::analyzedInfo_t info;

			//Primary Unit and Secondary Unit
			switch(qdInfo->I_MeasureMode)
			{
			//Off or Empty MemoryView
			case 0:
				info.i_priUnit=AU_None;
				info.s_priUnit="";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			// V~  /  dB + V~  / V~ + dB
			case 1: case 2:
				if(qdInfo->I_Unit_dB_AC)
				{
					info.i_priUnit=(qdInfo->I_dBRef_V_nm)?AU_dB_V:AU_dBm;
					info.s_priUnit=(qdInfo->I_dBRef_V_nm)?"dB V":"dBm";
					info.i_priCurrentType=ACT_NoCurrentType;
					info.s_priCurrentType="";

					info.i_secUnit=AU_Volts;
					info.s_secUnit="V";
					info.i_secCurrentType=ACT_AlternatingCurrent;
					info.s_secCurrentType="~";
				}
				else if(qdInfo->I_Unit_AC_dB)
				{
					info.i_priUnit=AU_Volts;
					info.s_priUnit="V";
					info.i_priCurrentType=ACT_AlternatingCurrent;
					info.s_priCurrentType="~";

					info.i_secUnit=(qdInfo->I_dBRef_V_nm)?AU_dB_V:AU_dBm;
					info.s_secUnit=(qdInfo->I_dBRef_V_nm)?"dB V":"dBm";
					info.i_secCurrentType=ACT_NoCurrentType;
					info.s_secCurrentType="";
				}
				else
				{
					info.i_priUnit=AU_Volts;
					info.s_priUnit="V";
					info.i_priCurrentType=ACT_AlternatingCurrent;
					info.s_priCurrentType="~";

					info.i_secUnit=AU_None;
					info.s_secUnit="";
					info.i_secCurrentType=ACT_NoCurrentType;
					info.s_secCurrentType="";
				}
			break;

			// V=
			case 3: case 4:
				info.i_priUnit=AU_Volts;
				info.s_priUnit="V";
				info.i_priCurrentType=ACT_DirectCurrent;
				info.s_priCurrentType="=";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			// V= Substates
			case 5: case 6:
				if(qdInfo->I_SubState_ACDC==1)
				{
					info.i_priUnit=AU_Volts;
					info.s_priUnit="V";
					info.i_priCurrentType=ACT_AlternatingCurrent;
					info.s_priCurrentType="~";

					info.i_secUnit=AU_Volts;
					info.s_secUnit="V";
					info.i_secCurrentType=ACT_DirectCurrent;
					info.s_secCurrentType="=";
				}
				else if (qdInfo->I_SubState_ACDC==2)
				{
					info.i_priUnit=AU_Volts;
					info.s_priUnit="V";
					info.i_priCurrentType=ACT_DirectCurrent;
					info.s_priCurrentType="=";

					info.i_secUnit=AU_Volts;
					info.s_secUnit="V";
					info.i_secCurrentType=ACT_AlternatingCurrent;
					info.s_secCurrentType="~";
				}
				else if (qdInfo->I_SubState_ACDC==3)
				{
					info.i_priUnit=AU_ACDC_V;
					info.s_priUnit="V";
					info.i_priCurrentType=ACT_DirectandAlternatingCurrent;
					info.s_priCurrentType="≃";

					info.i_secUnit=AU_None;
					info.s_secUnit="";
					info.i_secCurrentType=ACT_NoCurrentType;
					info.s_secCurrentType="";
				}
			break;

			//Ohm
			case 9: case 11:
				info.i_priUnit=AU_Ohm;
				info.s_priUnit="Ohm";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";
				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			//nS
			case 10:
				info.i_priUnit=AU_Siemens;
				info.s_priUnit="S";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			//Cap
			case 12:
				info.i_priUnit=AU_Farad;
				info.s_priUnit="F";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			//Diode
			case 13:
				info.i_priUnit=AU_DC_V;
				info.s_priUnit="V=";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			case 26:
				info.i_priUnit=AU_Celsius;
				info.s_priUnit="°C";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=AU_Celsius;
				info.s_secUnit="°C";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;


			case 27:
				info.i_priUnit=AU_Fahrenheit;
				info.s_priUnit="°F";
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_secUnit=AU_Fahrenheit;
				info.s_secUnit="°F";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			case 14: case 15: case 16:
				info.i_priUnit=AU_Ampere;
				info.s_priUnit="A";
				info.i_priCurrentType=ACT_AlternatingCurrent;
				info.s_priCurrentType="~";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;


			case 17: case 18: case 19:
				info.i_priUnit=AU_Ampere;
				info.s_priUnit="A";
				info.i_priCurrentType=ACT_DirectCurrent;
				info.s_priCurrentType="=";

				info.i_secUnit=AU_None;
				info.s_secUnit="";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";
			break;

			//A= Substates
			case 20: case 22: case 21:
				if(qdInfo->I_SubState_ACDC==1)
				{
					info.i_priUnit=AU_Ampere;
					info.s_priUnit="A";
					info.i_priCurrentType=ACT_AlternatingCurrent;
					info.s_priCurrentType="~";

					info.i_secUnit=AU_Ampere;
					info.s_secUnit="A";
					info.i_secCurrentType=ACT_DirectCurrent;
					info.s_secCurrentType="=";
				}
				else if (qdInfo->I_SubState_ACDC==2)
				{
					info.i_priUnit=AU_Ampere;
					info.s_priUnit="A";
					info.i_priCurrentType=ACT_DirectCurrent;
					info.s_priCurrentType="=";

					info.i_secUnit=AU_Ampere;
					info.s_secUnit="A";
					info.i_secCurrentType=ACT_AlternatingCurrent;
					info.s_secCurrentType="~";
				}
				else if (qdInfo->I_SubState_ACDC==3)
				{
					info.i_priUnit=AU_Ampere;
					info.s_priUnit="A";
					info.i_priCurrentType=ACT_DirectandAlternatingCurrent;
					info.s_priCurrentType="≃";

					info.i_secUnit=AU_None;
					info.s_secUnit="";
					info.i_secCurrentType=ACT_NoCurrentType;
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
				info.i_priCurrentType=ACT_NoCurrentType;
				info.s_priCurrentType="";

				info.i_priUnit=AU_Hz;
				info.s_priUnit="Hz";
			break;
			case 2:	//%
				info.i_priUnit=AU_Percent;
				info.s_priUnit="%";
				info.i_priCurrentType=ACT_DirectCurrent;
				info.s_priCurrentType="";

				info.i_secUnit=AU_Hz;
				info.s_secUnit="Hz";
				info.i_secCurrentType=ACT_NoCurrentType;
				info.s_secCurrentType="";

			break;
			case 3: //ms
				info.i_priUnit=AU_Seconds;
				info.s_priUnit="s";
				info.i_priCurrentType=ACT_DirectCurrent;
				info.s_priCurrentType="";

				info.i_secUnit=AU_Hz;
				info.s_secUnit="Hz";
				info.i_secCurrentType=ACT_NoCurrentType;
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
				info.i_priUnit=AU_Percent;
				info.s_priUnit="%";
			}

			//Does the multimeter log new data atm?
			info.b_Logging=qdInfo->I_MEMclr != 3 && qdInfo->I_S_Log &&qdInfo->I_CurrentView != 6 ;

			//Are we in ViewMode? If no data CurrentView==6 else MEMclr==3 //TODO (change into enum symbols)
			info.b_ViewMem=qdInfo->I_CurrentView == 6 || qdInfo->I_MEMclr == 3;

			//Check if mode switch is stuck between two positions
			info.b_ModeSwitchERR=qdInfo->I_MeasureMode == 0 && qdInfo->I_CurrentView == 1;


			//Get Mode Switch Position
			if(!info.b_ViewMem && !info.b_ModeSwitchERR)
			{
				switch(qdInfo->I_MeasureMode)
				{
				case 1:
					info.i_ModeSwitchPos=MS_VAC;
					break;
				case 2:
					info.i_ModeSwitchPos=MS_mVAC;
					break;
				case 3: case 5:
					info.i_ModeSwitchPos=MS_VDC;
					break;
				case 4: case 6:
					info.i_ModeSwitchPos=MS_mVDC;
					break;
				case 9: case 11: case 10:
					info.i_ModeSwitchPos=MS_OhmSiemens;
					break;
				case 12: case 13:
					info.i_ModeSwitchPos=MS_CapDiode;
					break;
				case 27: case 26:
					info.i_ModeSwitchPos=MS_Temp;
					break;
				case 14: case 15:
					info.i_ModeSwitchPos=MS_AACmAAC;
					break;
				case 16:
					info.i_ModeSwitchPos=MS_uAAC;
					break;
				case 17: case 18: case 21: case 20:
					info.i_ModeSwitchPos=MS_ADCmADC;
					break;
				case 19: case 22:
					info.i_ModeSwitchPos=MS_uADC;
					break;
				default:
					info.i_ModeSwitchPos=MS_Unknown;
				}
			}
			else
			{
				if(info.b_ViewMem) info.i_ModeSwitchPos=MS_VIEWMEM;
				if(info.b_ModeSwitchERR) info.i_ModeSwitchPos=MS_STUCKBETW2POS;
			}
			return info;
	}






	std::string Fluke189DataResponseAnalyzerWrapper::valueErrorToString(Fluke189::ValueError number)
	{
		std::string ErrorString;
		switch(number)
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

		case Fluke189::VE_NOT_APPLICABLE:
			ErrorString = "";
		break;

		case Fluke189::VE_NO_ERROR:
			ErrorString="";

		default:
			ErrorString = "UNKNOWN_ERR";
		break;
		}

		return ErrorString;
	}

	/*
	 * Fluke189ResponseAnalyzerWrapperQD0 functions
	 */
	bool Fluke189DataResponseAnalyzerWrapperQD0::hasErrorPRIdisplay(bool reading2)
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		if(reading2)
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorPV0==1;
		}
		else
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorPV1==1;
		}
	}

	bool Fluke189DataResponseAnalyzerWrapperQD0::hasErrorSECdisplay(bool reading2)
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		if(reading2)
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorSV0==1;
		}
		else
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorSV1==1;
		}
	}


	Fluke189::ValueError Fluke189DataResponseAnalyzerWrapperQD0::get_PRIdisplayError(bool reading2)
	{
		if(!hasErrorPRIdisplay(reading2))
		{
			return Fluke189::VE_NO_ERROR;
		}

		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		if(reading2)
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorNoPV0;
		}
		else
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorNoPV1;
		}

	}


	Fluke189::ValueError Fluke189DataResponseAnalyzerWrapperQD0::get_SECdisplayError(bool reading2)
	{
		if(!hasErrorSECdisplay(reading2))
		{
			return Fluke189::VE_NO_ERROR;
		}

		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		if(reading2)
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorNoSV0;
		}
		else
		{
			return (Fluke189::ValueError)container->Data()->I_ErrorNoSV1;
		}
	}

	Fluke189DataResponseAnalyzerWrapper::ModeSwitchSetting Fluke189DataResponseAnalyzerWrapperQD0::get_ModeSwitchSetting()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo((Fluke::Fluke189::qdInfo_t*) &(container->Data()->I_QDInfo)).i_ModeSwitchPos;
	}

	Fluke189DataResponseAnalyzerWrapper::Unit Fluke189DataResponseAnalyzerWrapperQD0::get_primaryUnit()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo((Fluke::Fluke189::qdInfo_t*) &(container->Data()->I_QDInfo)).i_priUnit;
	}

	Fluke189DataResponseAnalyzerWrapper::Unit Fluke189DataResponseAnalyzerWrapperQD0::get_secondaryUnit()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo((Fluke::Fluke189::qdInfo_t*) &(container->Data()->I_QDInfo)).i_secUnit;
	}

	Fluke189DataResponseAnalyzerWrapper::CurrentType Fluke189DataResponseAnalyzerWrapperQD0::get_primaryCurrentType()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo((Fluke::Fluke189::qdInfo_t*) &(container->Data()->I_QDInfo)).i_priCurrentType;
	}

	Fluke189DataResponseAnalyzerWrapper::CurrentType Fluke189DataResponseAnalyzerWrapperQD0::get_secondaryCurrentType()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo((Fluke::Fluke189::qdInfo_t*) &(container->Data()->I_QDInfo)).i_secCurrentType;
	}

	Fluke189DataResponseAnalyzerWrapper::Etch Fluke189DataResponseAnalyzerWrapperQD0::get_EtchInfo()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		if(container->Data()->I_QDInfo.I_RisingEtch) return AE_RISING_ETCH;
		if(container->Data()->I_QDInfo.I_FallingEtch) return AE_FALLING_ETCH;
		return AE_NOT_APPLICABLE;
	}











































////////////////////////////////////OLD STUFF // TODO [REMOVE] AFTER COPY

//	/*
//	 * ResponseAnalyserWrapper Functions
//	 */
//	Fluke189::ValueError Fluke189ResponseAnalyserWrapper::get_PRIdisplayError(bool reading2)  //TODO ADD NO ERROR!!!
//	{
//		if(!hasErrorPRIdisplay(reading2))
//		{
//			return Fluke189::VE_NO_ERROR;
//		}
//
//
//		switch(this->currentAnalyser->currentResponseContainerType)
//		{
//		case Fluke189ResponseAnalyser::QD0:
//				Fluke189::RCT_QD0* container0;
//				container0=(Fluke189::RCT_QD0*)this->currentAnalyser->container;
//				if(reading2)
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorNoPV0;
//				}
//				else
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorNoPV1;
//				}
//			break;
//
//		case Fluke189ResponseAnalyser::QD4:
//				Fluke189::RCT_QD4* container4;
//				container4=(Fluke189::RCT_QD4*)this->currentAnalyser->container;
//				if(reading2)
//				{
//					return (Fluke189::ValueError)container4->Data()->dsets[this->datasetnumber].I_ErrorNoPV0;
//				}
//				else
//				{
//					return (Fluke189::ValueError)container4->Data()->dsets[this->datasetnumber].I_ErrorNoPV1;
//				}
//			break;
//
//		default:
//			std::cerr<<"This function is not applicable for that kind of response"<<std::endl;
//			return Fluke189::VE_NOT_APPLICABLE;
//		}
//	}
//
//
//	Fluke189::ValueError Fluke189ResponseAnalyserWrapper::get_SECdisplayError(bool reading2)
//	{
//		if(!hasErrorSECdisplay(reading2))
//		{
//			return Fluke189::VE_NO_ERROR;
//		}
//
//		switch(this->currentAnalyser->currentResponseContainerType)
//		{
//		case Fluke189ResponseAnalyser::QD0:
//				Fluke189::RCT_QD0* container0;
//				container0=(Fluke189::RCT_QD0*)this->currentAnalyser->container;
//				if(reading2)
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorNoSV0;
//				}
//				else
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorNoSV1;
//				}
//			break;
//
//		case Fluke189ResponseAnalyser::QD4:
//				Fluke189::RCT_QD4* container4;
//				container4=(Fluke189::RCT_QD4*)this->currentAnalyser->container;
//				if(reading2)std::cerr<<"For QD4 there is no second value of the secondary display, ignoring..."<<std::endl;
//				return (Fluke189::ValueError)container4->Data()->dsets[this->datasetnumber].I_ErrorNoSV;
//			break;
//
//		default:
//			std::cerr<<"This function is not applicable for that kind of response"<<std::endl;
//			return Fluke189::VE_NOT_APPLICABLE;
//		}
//	}
//
//
//	Fluke189::Analyse_ModeSwitchSetting Fluke189ResponseAnalyserWrapper::get_ModeSwitchSetting()
//	{
//		if(this->currentAnalyser->currentResponseContainerType==Fluke189ResponseAnalyser::QD0)
//		{
//
//		}
//		std::cerr<<"This function is not applicable for that kind of response"<<std::endl;
//		return Fluke189::AMS_NOT_APPLICABLE;
//	}
//
//	bool Fluke189ResponseAnalyserWrapper::hasErrorPRIdisplay(bool reading2)
//	{
//		switch(this->currentAnalyser->currentResponseContainerType)
//		{
//		case Fluke189ResponseAnalyser::QD0:
//				Fluke189::RCT_QD0* container0;
//				container0=(Fluke189::RCT_QD0*)this->currentAnalyser->container;
//				if(reading2)
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorPV0==1;
//				}
//				else
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorPV1==1;
//				}
//			break;
//
//		case Fluke189ResponseAnalyser::QD4:
//				Fluke189::RCT_QD4* container4;
//				container4=(Fluke189::RCT_QD4*)this->currentAnalyser->container;
//				if(reading2)
//				{
//					return (Fluke189::ValueError)container4->Data()->dsets[this->datasetnumber].I_ErrorPV0==1;
//				}
//				else
//				{
//					return (Fluke189::ValueError)container4->Data()->dsets[this->datasetnumber].I_ErrorPV1==1;
//				}
//			break;
//
//		default:
//			std::cerr<<"This function is not applicable for that kind of response"<<std::endl;
//			return -1;
//		}
//	}
//
//	bool Fluke189ResponseAnalyserWrapper::hasErrorSECdisplay(bool reading2)
//	{
//		switch(this->currentAnalyser->currentResponseContainerType)
//		{
//		case Fluke189ResponseAnalyser::QD0:
//				Fluke189::RCT_QD0* container0;
//				container0=(Fluke189::RCT_QD0*)this->currentAnalyser->container;
//				if(reading2)
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorSV0==1;
//				}
//				else
//				{
//					return (Fluke189::ValueError)container0->Data()->I_ErrorSV1==1;
//				}
//			break;
//
//		case Fluke189ResponseAnalyser::QD4:
//				Fluke189::RCT_QD4* container4;
//				container4=(Fluke189::RCT_QD4*)this->currentAnalyser->container;
//				if(reading2)std::cerr<<"For QD4 there is no second value of the secondary display, ignoring..."<<std::endl;
//				return (Fluke189::ValueError)container4->Data()->dsets[this->datasetnumber].I_ErrorSV==1;
//			break;
//
//		default:
//			std::cerr<<"This function is not applicable for that kind of response"<<std::endl;
//			return -1;
//		}
//	}
//
//	std::string Fluke189ResponseAnalyserWrapper::valueErrorToString(Fluke189::ValueError number)
//	{
// 		std::string ErrorString;
//		switch(number)
//		{
//		case Fluke189::VE_Display_OFFLINE:
//			ErrorString = "-OFF-";
//		break;
//
//		case Fluke189::VE_LEADS_CONNECTION_WRONG:
//			ErrorString = "LEADS";
//		break;
//
//		case Fluke189::VE_OL_OUTOFRANGE_NOCON:
//			ErrorString = "OL";
//		break;
//
//		case Fluke189::VE_OPEN__NOTHING_CONNECTED:
//			ErrorString = "OPEN";
//		break;
//
//		case Fluke189::VE_FUSE:
//			ErrorString = "FUSE";
//		break;
//
//		case Fluke189::VE_NOT_APPLICABLE:
//			ErrorString = "";
//		break;
//
//		case Fluke189::VE_NO_ERROR:
//			ErrorString="";
//
//		default:
//			ErrorString = "UNKNOWN_ERR";
//		break;
//		}
//
//		return ErrorString;
//	}









/*Namespace End*/}

