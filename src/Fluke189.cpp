/*
 * Fluke189.cpp
 *
 *  Created on: 06.01.2010
 *      Author: cyborg-x1
 */

#include "Fluke189.hpp"

using namespace LibSerial;
namespace Fluke {

	//////////////////////////
    ////Fluke189 Functions////
    //////////////////////////
	Fluke189::Fluke189(std::string filename)
	{
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

	//////////////////////////////////
	////ResponseAnalyzer Functions////
	//////////////////////////////////

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


    /////////////////////////////////////////////////
	////Fluke189ResponseAnalyzerWrapper Functions////
	/////////////////////////////////////////////////

	Fluke189DataResponseAnalyzerWrapper::analyzedInfo_t Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo(Fluke::Fluke189::qdInfo_t* qdInfo)
	{
		Fluke189DataResponseAnalyzerWrapper::analyzedInfo_t info;

			//Primary Unit and Secondary Unit
			switch(qdInfo->I_MeasureMode)
			{
			//Off or Empty MemoryView
			case 0:
				info.i_priUnit=AU_None;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			// V~  /  dB + V~  / V~ + dB
			case 1: case 2:
				if(qdInfo->I_Unit_dB_AC)
				{
					info.i_priUnit=(qdInfo->I_dBRef_V_nm)?AU_dB_V:AU_dBm;
					info.i_priCurrentType=ACT_NoCurrentType;


					info.i_secUnit=AU_Volts;
					info.i_secCurrentType=ACT_AlternatingCurrent;

				}
				else if(qdInfo->I_Unit_AC_dB)
				{
					info.i_priUnit=AU_Volts;
					info.i_priCurrentType=ACT_AlternatingCurrent;


					info.i_secUnit=(qdInfo->I_dBRef_V_nm)?AU_dB_V:AU_dBm;
					info.i_secCurrentType=ACT_NoCurrentType;

				}
				else
				{
					info.i_priUnit=AU_Volts;
					info.i_priCurrentType=ACT_AlternatingCurrent;


					info.i_secUnit=AU_None;
					info.i_secCurrentType=ACT_NoCurrentType;

				}
			break;

			// V=
			case 3: case 4:
				info.i_priUnit=AU_Volts;
				info.i_priCurrentType=ACT_DirectCurrent;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			// V= Substates
			case 5: case 6:
				if(qdInfo->I_SubState_ACDC==1)
				{
					info.i_priUnit=AU_Volts;
					info.i_priCurrentType=ACT_AlternatingCurrent;


					info.i_secUnit=AU_Volts;
					info.i_secCurrentType=ACT_DirectCurrent;

				}
				else if (qdInfo->I_SubState_ACDC==2)
				{
					info.i_priUnit=AU_Volts;
					info.i_priCurrentType=ACT_DirectCurrent;


					info.i_secUnit=AU_Volts;
					info.i_secCurrentType=ACT_AlternatingCurrent;

				}
				else if (qdInfo->I_SubState_ACDC==3)
				{
					info.i_priUnit=AU_Volts;
					info.i_priCurrentType=ACT_DirectandAlternatingCurrent;


					info.i_secUnit=AU_None;
					info.i_secCurrentType=ACT_NoCurrentType;

				}
			break;

			//Ohm
			case 9: case 11:
				info.i_priUnit=AU_Ohm;
				info.i_priCurrentType=ACT_NoCurrentType;

				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			//nS
			case 10:
				info.i_priUnit=AU_Siemens;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			//Cap
			case 12:
				info.i_priUnit=AU_Farad;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			//Diode
			case 13:
				info.i_priUnit=AU_Volts;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			case 26:
				info.i_priUnit=AU_Celsius;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_secUnit=AU_Celsius;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;


			case 27:
				info.i_priUnit=AU_Fahrenheit;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_secUnit=AU_Fahrenheit;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			case 14: case 15: case 16:
				info.i_priUnit=AU_Ampere;
				info.i_priCurrentType=ACT_AlternatingCurrent;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;


			case 17: case 18: case 19:
				info.i_priUnit=AU_Ampere;
				info.i_priCurrentType=ACT_DirectCurrent;


				info.i_secUnit=AU_None;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;

			//A= Substates
			case 20: case 22: case 21:
				if(qdInfo->I_SubState_ACDC==1)
				{
					info.i_priUnit=AU_Ampere;
					info.i_priCurrentType=ACT_AlternatingCurrent;


					info.i_secUnit=AU_Ampere;
					info.i_secCurrentType=ACT_DirectCurrent;

				}
				else if (qdInfo->I_SubState_ACDC==2)
				{
					info.i_priUnit=AU_Ampere;
					info.i_priCurrentType=ACT_DirectCurrent;


					info.i_secUnit=AU_Ampere;
					info.i_secCurrentType=ACT_AlternatingCurrent;

				}
				else if (qdInfo->I_SubState_ACDC==3)
				{
					info.i_priUnit=AU_Ampere;
					info.i_priCurrentType=ACT_DirectandAlternatingCurrent;


					info.i_secUnit=AU_None;
					info.i_secCurrentType=ACT_NoCurrentType;

				}

			break;


			}
			//Hz % ms
			switch(qdInfo->I_Hz_Percent_ms)
			{
			case 1: //Hz
				info.i_secUnit=info.i_priUnit;

				info.i_secCurrentType=info.i_priCurrentType;
				info.i_priCurrentType=ACT_NoCurrentType;


				info.i_priUnit=AU_Hz;

			break;
			case 2:	//%
				info.i_priUnit=AU_Percent;
				info.i_priCurrentType=ACT_DirectCurrent;


				info.i_secUnit=AU_Hz;
				info.i_secCurrentType=ACT_NoCurrentType;


			break;
			case 3: //ms
				info.i_priUnit=AU_Seconds;
				info.i_priCurrentType=ACT_DirectCurrent;


				info.i_secUnit=AU_Hz;
				info.i_secCurrentType=ACT_NoCurrentType;

			break;
			}

			//Min Max Avg Mode
			if(qdInfo->I_MinMaxMode)
			{
				//Secondary unit becomes primary unit(showing current value)
				info.i_secUnit=info.i_priUnit;

				info.i_secCurrentType=info.i_priCurrentType;

			}

			//Delta and Delta Percent
			if(qdInfo->I_Delta)
			{
				std::cout<<"(1)";
				//Secondary unit becomes primary unit (showing start value)
				info.i_secUnit=info.i_priUnit;

				info.i_secCurrentType=info.i_priCurrentType;

			}
			else if(qdInfo->I_DeltaPercent)
			{
				//secondary unit becomes primary and primary changes to percent
				info.i_secUnit=info.i_priUnit;

				info.i_secCurrentType=info.i_priCurrentType;

				info.i_priUnit=AU_Percent;


			}

			//If any hold mode is enabled and the secondary display was off before it changes to the current value
			//while the primary value stays at the hold value.
			//But if there is already anything displayed in the secondary display it does not change to the current value
			if(qdInfo->I_S_Hold && info.i_secUnit==AU_None)
			{
				info.i_secUnit=info.i_priUnit;

				info.i_secCurrentType=info.i_priCurrentType;

			}



			//Does the multimeter log new data atm?
			info.b_Logging=qdInfo->I_MEMclr != 3 && qdInfo->I_S_Log &&qdInfo->I_CurrentView != 6 ;

			//Are we in ViewMode? If no data CurrentView==6 else MEMclr==3
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

	std::string Fluke189DataResponseAnalyzerWrapper::valueErrorToString(DispError number)
	{
		std::string ErrorString;
		switch(number)
		{
		case Fluke189DataResponseAnalyzerWrapper::ERR_DISPLAY_OFF:
			ErrorString = "-OFF-";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_LEADS:
			ErrorString = "LEADS";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_OUTOFRANGE_POSITIVE:
			ErrorString = "OL";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_OPEN:
			ErrorString = "OPEN";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_FUSE:
			ErrorString = "FUSE";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_UNKNOWN:
			ErrorString = "UNKNOWN ERR";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_NONE:
			ErrorString="";
		break;

		case Fluke189DataResponseAnalyzerWrapper::ERR_OUTOFRANGE_NEGATIVE:
			ErrorString="-0L";
		break;

		default:
			ErrorString = "-?-UERROR--";
		break;
		}

		return ErrorString;
	}

	Fluke189DataResponseAnalyzerWrapper::DispError Fluke189DataResponseAnalyzerWrapper::valueExtractError(int value)
	{

		bool negative=false;
		if(value < -99999 || value > 99999 )
		{
			if(value < 0) negative=true;
			value &= ~((1<<31) | (1<<30) | (1<<29) | (1<<28));
			switch(value)
			{
			case 1: //DISPLAY OFFLINE
					return ERR_DISPLAY_OFF;
			case 22: //OPEN
					return ERR_OPEN;
			case 29: //LEADS
					return ERR_LEADS;
			case 33: //OUT OF RANGE
					return ERR_OUTOFRANGE_POSITIVE;
			case 0:
				if(negative)
				{
					return ERR_OUTOFRANGE_NEGATIVE;
				}
				else
				{
					return ERR_UNKNOWN;
				}
			case 27: //FUSE ERROR
					return ERR_FUSE;
			}
			return ERR_UNKNOWN;
		}
		else
		{
			return ERR_NONE;
		}
	}

	std::string Fluke189DataResponseAnalyzerWrapper::currentTypeToString(CurrentType type, bool symbol)
	{
		std::string typestr;
		if(!symbol)
		{
			switch(type)
			{
			case ACT_AlternatingCurrent:
				typestr="AC";
				break;
			case ACT_DirectCurrent:
				typestr="DC";
				break;
			case ACT_DirectandAlternatingCurrent:
				typestr="AC+DC";
				break;
			case ACT_NoCurrentType:
				typestr="";
				break;
			default:
				throw std::runtime_error("Error: Unknown CurrentType!!!");
			}
		}
		else
		{
			switch(type)
			{
			case ACT_AlternatingCurrent:
				typestr="~";
				break;
			case ACT_DirectCurrent:
				typestr="=";
				break;
			case ACT_DirectandAlternatingCurrent:
				typestr="≃";
				break;
			case ACT_NoCurrentType:
				typestr="";
				break;
			default:
				throw std::runtime_error("Error: Unknown CurrentType!!!");
			}
		}
		return typestr;
	}

	std::string Fluke189DataResponseAnalyzerWrapper::unitToString(Unit unit, bool symbol)
	{
		std::string unitstr;
		if(symbol)
		{
			switch(unit)
			{
			case AU_None:
				unitstr="";
				break;
			case AU_Ampere:
				unitstr="A";
				break;
			case AU_Celsius:
				unitstr="°C";
				break;
			case AU_Fahrenheit:
				unitstr="°F";
				break;
			case AU_Farad:
				unitstr="F";
				break;
			case AU_Hz:
				unitstr="Hz";
				break;
			case AU_Ohm:
				unitstr="Ω";
				break;
			case AU_Percent:
				unitstr="%";
				break;
			case AU_Siemens:
				unitstr="S";
				break;
			case AU_Volts:
				unitstr="V";
				break;
			case AU_dB_V:
				unitstr="dBV";
				break;
			case AU_dBm:
				unitstr="dBm";
				break;
			case AU_Seconds:
				unitstr="s";
				break;

			}
		}
		else
		{
			switch(unit)
			{
			case AU_None:
				unitstr="";
				break;
			case AU_Ampere:
				unitstr="Ampere";
				break;
			case AU_Celsius:
				unitstr="Celsius";
				break;
			case AU_Fahrenheit:
				unitstr="Fahrenheit";
				break;
			case AU_Farad:
				unitstr="Farad";
				break;
			case AU_Hz:
				unitstr="Herz";
				break;
			case AU_Ohm:
				unitstr="Ohm";
				break;
			case AU_Percent:
				unitstr="Percent";
				break;
			case AU_Siemens:
				unitstr="Siemens";
				break;
			case AU_Volts:
				unitstr="Volts";
				break;
			case AU_dB_V:
				unitstr="Dezibel (V)";
				break;
			case AU_dBm:
				unitstr="Dezibel (m)";
				break;
			case AU_Seconds:
				unitstr="Seconds";
				break;
			}
		}
		return unitstr;
	}

	std::string Fluke189DataResponseAnalyzerWrapper::etchToString(Etch etch, bool symbol)
	{
		std::string EtchString;
		if(symbol)
		{
			switch(etch)
			{
			case AE_RISING_ETCH:
				EtchString="⏌";
				break;
			case AE_FALLING_ETCH:
				EtchString="⏋";
				break;
			case AE_NOT_APPLICABLE:
				EtchString="";
				break;
			}
		}
		else
		{
			switch(etch)
			{
			case AE_RISING_ETCH:
				EtchString="Rising Etch";
				break;
			case AE_FALLING_ETCH:
				EtchString="Falling Etch";
				break;
			case AE_NOT_APPLICABLE:
				EtchString="";
				break;
			}
		}
	}
	////////////////////////////////////////////////////
	////Fluke189ResponseAnalyzerWrapperQD0 Functions////
	////////////////////////////////////////////////////

	Fluke189DataResponseAnalyzerWrapper::analyzedInfo_t Fluke189DataResponseAnalyzerWrapperQD0::getAnalyzedInfoStruct()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::analyzeQdInfo((Fluke::Fluke189::qdInfo_t*) &(container->Data()->I_QDInfo));
	}

	bool Fluke189DataResponseAnalyzerWrapperQD0::hasErrorPRIdisplay()
	{
		return(!!get_PRIdisplayError());
	}

	bool Fluke189DataResponseAnalyzerWrapperQD0::hasErrorSECdisplay()
	{
		return(!!get_SECdisplayError());
	}

	Fluke189DataResponseAnalyzerWrapper::DispError Fluke189DataResponseAnalyzerWrapperQD0::get_PRIdisplayError()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::valueExtractError(container->Data()->I_priValue0);
	}

	Fluke189DataResponseAnalyzerWrapper::DispError Fluke189DataResponseAnalyzerWrapperQD0::get_SECdisplayError()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		return Fluke189DataResponseAnalyzerWrapper::valueExtractError(container->Data()->I_secValue0);
	}

	Fluke189DataResponseAnalyzerWrapper::ModeSwitchSetting Fluke189DataResponseAnalyzerWrapperQD0::get_ModeSwitchSetting()
	{
		return getAnalyzedInfoStruct().i_ModeSwitchPos;
	}

	Fluke189DataResponseAnalyzerWrapper::Unit Fluke189DataResponseAnalyzerWrapperQD0::get_primaryUnit()
	{
		return getAnalyzedInfoStruct().i_priUnit;
	}

	Fluke189DataResponseAnalyzerWrapper::Unit Fluke189DataResponseAnalyzerWrapperQD0::get_secondaryUnit()
	{
		return getAnalyzedInfoStruct().i_secUnit;
	}

	Fluke189DataResponseAnalyzerWrapper::CurrentType Fluke189DataResponseAnalyzerWrapperQD0::get_primaryCurrentType()
	{
		return getAnalyzedInfoStruct().i_priCurrentType;
	}

	Fluke189DataResponseAnalyzerWrapper::CurrentType Fluke189DataResponseAnalyzerWrapperQD0::get_secondaryCurrentType()
	{
		return getAnalyzedInfoStruct().i_secCurrentType;
	}

	Fluke189DataResponseAnalyzerWrapper::Etch Fluke189DataResponseAnalyzerWrapperQD0::get_EtchInfo()
	{
		Fluke189::RCT_QD0* container;
		container=(Fluke189::RCT_QD0*)this->currentContainer;
		if(container->Data()->I_QDInfo.I_RisingEtch) return AE_RISING_ETCH;
		if(container->Data()->I_QDInfo.I_FallingEtch) return AE_FALLING_ETCH;
		return AE_NOT_APPLICABLE;
	}

	std::string Fluke189DataResponseAnalyzerWrapperQD0::getPrimaryUnitSymbol()
	{
		return this->unitToString(getAnalyzedInfoStruct().i_priUnit,1);
	}

	std::string Fluke189DataResponseAnalyzerWrapperQD0::getSecondaryUnitSymbol()
	{
		return this->unitToString(getAnalyzedInfoStruct().i_secUnit,1);
	}

	////////////////////////////////////
	////Fluke189QD0Logging Functions////
	////////////////////////////////////

	void Fluke189QD0Logging::addContainer(Fluke189::RCT_QD0& container)
	 {
		 Fluke189DataResponseAnalyzer dra=Fluke189DataResponseAnalyzer(container);

		 this->pri_error=dra[0]->get_PRIdisplayError();
		 this->sec_error=dra[0]->get_SECdisplayError();

		 this->current_pri.Value=container.Data()->I_priValue0;

		 //If we have Percent for the primary value we need to ignore the Prefix... it could be wrong...
		 this->current_pri.Prefix=(dra[0]->getAnalyzedInfoStruct().i_priUnit != Fluke189DataResponseAnalyzerWrapper::AU_Percent)?container.Data()->I_priSI_Prefix0 : 0;

		 this->current_pri.Decimal=(container.Data()->I_priDecimal0 != 128) ? container.Data()->I_priDecimal0 : 2;
		 this->pri_unit_str=dra[0]->getAnalyzedInfoStruct().i_priUnit;



		 this->current_sec.Value=container.Data()->I_secValue0;
		 this->current_sec.Prefix=container.Data()->I_secSi_Prefix;
		 this->current_sec.Decimal=(container.Data()->I_secDecimal != 128) ? container.Data()->I_secDecimal : 2;
		 this->sec_unit_str=dra[0]->getAnalyzedInfoStruct().i_secUnit;


		 modes_t current_modes;
		 current_modes.deltapercent=container.Data()->I_QDInfo.I_DeltaPercent;
		 current_modes.delta=container.Data()->I_QDInfo.I_Delta;
		 current_modes.minmaxavg=container.Data()->I_QDInfo.I_MinMaxAvg;
		 current_modes.hold=container.Data()->I_QDInfo.I_Hold;
		 current_modes.autohold=container.Data()->I_QDInfo.I_AutoHold;
		 current_modes.etch=dra[0]->get_EtchInfo();





		 if(   dra[0]->get_primaryUnit() != this->pri_unit
			|| current_modes != this->modes
			|| this->pri_current != dra[0]->get_primaryCurrentType()
			|| this->pri_reset
			)
		 {
			 this->pri_unit=dra[0]->get_primaryUnit();
			 this->pri_count=0;
			 this->pri_min=this->current_pri;
			 this->pri_max=this->current_pri;
			 this->pri_current=dra[0]->get_primaryCurrentType();
			 this->pri_reset=true;
		 }

		 if(dra[0]->get_secondaryUnit() != this->sec_unit
		    || current_modes != this->modes
		    || this->sec_current != dra[0]->get_secondaryCurrentType()
		    || this->sec_reset
		    )
		 {
			 this->sec_unit=dra[0]->get_secondaryUnit();

			 this->sec_count=0;
			 this->sec_min=this->current_sec;
			 this->sec_max=this->current_sec;
			 this->sec_current=dra[0]->get_secondaryCurrentType();
			 this->sec_reset=true;
		 }

		 if(current_modes != this->modes)
		 {
			 this->modes = current_modes;
		 }


		 //Process MINIMUM
		 //If current is smaller set it to the new min
		 if(fluke189ValueSmallerThan(this->current_pri, this->pri_min) && !dra[0]->hasErrorPRIdisplay() && !pri_reset)
		 {
			 this->pri_min=this->current_pri;
		 }
		 if(fluke189ValueSmallerThan(this->current_sec, this->sec_min) && !dra[0]->hasErrorSECdisplay() && !sec_reset)
		 {
			 this->sec_min=this->current_sec;
		 }

		 //Process MAXIMUM
		 //If max is smaller than current set current to the new max
		 if(fluke189ValueSmallerThan(this->pri_max, this->current_pri) && !dra[0]->hasErrorPRIdisplay() && !pri_reset)
		 {
			 this->pri_max=this->current_pri;
		 }
		 if(fluke189ValueSmallerThan(this->sec_min, this->current_sec) && !dra[0]->hasErrorSECdisplay() && !sec_reset)
		 {
			 this->sec_min=this->current_sec;
		 }

		 //Process AVERAGE
		 if(!dra[0]->hasErrorPRIdisplay())
		 {
			 this->pri_avg_ll=(this->pri_avg_ll * this->pri_count+this->current_pri.Value*pow(10,13-(this->current_pri.Prefix*(-3)+this->current_pri.Decimal)))/(++this->pri_count);
			 this->pri_count++;
		 }
		 if(!dra[0]->hasErrorSECdisplay())
		 {
			 this->sec_avg_ll=(this->sec_avg_ll * this->sec_count+this->current_sec.Value*pow(10,13-(this->current_sec.Prefix*(-3)+this->current_sec.Decimal)))/(++this->sec_count);
			 this->sec_count++;
		 }

		//Average long long to minmaxvaluestorage
		if(!dra[0]->hasErrorPRIdisplay())
		{
           //change reset to false only when a good measurement has been gotten after reset
			 this->pri_reset=false;
			 int pri_highest_place=0;
			 //Get highest place of average
			 for(int i = 0; ((int)(this->pri_avg_ll / pow(10, 18-i))) == 0;i++ ) pri_highest_place = 18 - i -1;
			 if(pri_highest_place<4)
			 {
				 this->pri_avg.Prefix=-3;
				 this->pri_avg.Decimal=4;
				 this->pri_avg.Value=(int)pri_avg_ll;
			 }
			 else
			 {
				 unsigned int groupsOfthree=(pri_highest_place)/3;
				 this->pri_avg.Prefix=groupsOfthree-4;
				 this->pri_avg.Decimal=5-((pri_highest_place-groupsOfthree*3)+1);
				 this->pri_avg.Value=pri_avg_ll/pow(10, (groupsOfthree*3)-(pri_avg.Decimal-1));
			 }
		}

		if(!dra[0]->hasErrorSECdisplay())
		{
		//change reset to false only when a good measurement has been gotten after reset
			 this->sec_reset=false;
			 int sec_highest_place=0;
			 //Get highest place of average
			 for(int i = 0; ((int)(this->sec_avg_ll / pow(10, 18-i))) == 0;i++ ) sec_highest_place = 18 - i -1;
			 if(sec_highest_place<4)
			 {
				 this->sec_avg.Prefix=-3;
				 this->sec_avg.Decimal=4;
				 this->sec_avg.Value=(int)sec_avg_ll;
			 }
			 else
			 {
				 unsigned int groupsOfthree=(sec_highest_place)/3;
				 this->sec_avg.Prefix=groupsOfthree-4;
				 this->sec_avg.Decimal=5-((sec_highest_place-groupsOfthree*3)+1);
				 this->sec_avg.Value=sec_avg_ll/pow(10, (groupsOfthree*3)-(sec_avg.Decimal-1));
			 }
		}


	 }

	std::string Fluke189QD0Logging::minMaxAvgValueStorageToString(Fluke189Value_t value)
	{
		std::string strvalue, valmem;
		std::stringstream convert_int;

		//Insert space
		strvalue.append(" ");

		//Convert Value into string
		convert_int<<value.Value;
		convert_int>>valmem;

		//if negative remove minus for the next step
		if(value.Value<0)valmem.replace(0,1,"");


		//calculate insert location
		int insertlocation=valmem.length()-value.Decimal;

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
		if(value.Value<0)valmem.insert(0,1,'-');

		//Append to Output
		strvalue.append(valmem);
		//Insert space
		strvalue.append(" ");
		//addPrefix
		switch(value.Prefix)
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

		return strvalue;

	}

	std::string Fluke189QD0Logging::get_Primary_ValueAndUnit_String()
	{
	 std::string ValueString;
	 if(this->modes.delta || this->modes.deltapercent) ValueString.append("Δ");
	 ValueString += this->minMaxAvgValueStorageToString(this->current_pri);
	 ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->pri_unit_str,true));
	 ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->pri_current,true));
	 if(pri_error) ValueString=Fluke189DataResponseAnalyzerWrapper::valueErrorToString(pri_error);
	 return ValueString;
	}

	std::string Fluke189QD0Logging::get_Primary_Max_ValueAndUnit_String()
	{
		 std::string ValueString;
		 if(this->modes.delta || this->modes.deltapercent) ValueString.append("Δ");
		 ValueString += this->minMaxAvgValueStorageToString(this->pri_max);
		 ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->pri_unit_str,true));
		 ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->pri_current,true));
		 if(pri_reset) ValueString = ""; //If we have not had a good measurement after reset do not show the value
		 return ValueString;
	}

	std::string Fluke189QD0Logging::get_Primary_Min_ValueAndUnit_String()
	{
		 std::string ValueString;
		 if(this->modes.delta || this->modes.deltapercent) ValueString.append("Δ");
		 ValueString += this->minMaxAvgValueStorageToString(this->pri_min);
		 ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->pri_unit_str,true));
		 ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->pri_current,true));
		 if(pri_reset) ValueString = ""; //If we have not had a good measurement after reset do not show the value
		 return ValueString;
	}

	std::string Fluke189QD0Logging::get_Primary_Avg_ValueAndUnit_String()
	{
		 std::string ValueString;
		 if(this->modes.delta || this->modes.deltapercent) ValueString.append("Δ");
		 ValueString += this->minMaxAvgValueStorageToString(this->pri_avg);
		 ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->pri_unit_str,true));
		 ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->pri_current,true));
		 if(pri_reset) ValueString = ""; //If we have not had a good measurement after reset do not show the value
		 return ValueString;
	}



	std::string Fluke189QD0Logging::get_Secondary_ValueAndUnit_String()
	{

		std::string ValueString=this->minMaxAvgValueStorageToString(this->current_sec);
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->sec_unit_str,true));
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->sec_current,true));
		if(sec_error) ValueString=Fluke189DataResponseAnalyzerWrapper::valueErrorToString(sec_error);
		return ValueString;
	}

	std::string Fluke189QD0Logging::get_Secondary_Max_ValueAndUnit_String()
	{

		std::string ValueString=this->minMaxAvgValueStorageToString(this->sec_max);
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->sec_unit_str,true));
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->sec_current,true));
		if(sec_reset) ValueString = ""; //If we have not had a good measurement after reset do not show the value
		return ValueString;
	}

	std::string Fluke189QD0Logging::get_Secondary_Min_ValueAndUnit_String()
	{

		std::string ValueString=this->minMaxAvgValueStorageToString(this->sec_min);
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->sec_unit_str,true));
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->sec_current,true));
		if(sec_reset) ValueString = ""; //If we have not had a good measurement after reset do not show the value
		return ValueString;
	}

	std::string Fluke189QD0Logging::get_Secondary_Avg_ValueAndUnit_String()
	{

		std::string ValueString=this->minMaxAvgValueStorageToString(this->sec_avg);
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::unitToString(this->sec_unit_str,true));
		ValueString.append(Fluke189DataResponseAnalyzerWrapper::currentTypeToString(this->sec_current,true));
		if(sec_reset) ValueString = ""; //If we have not had a good measurement after reset do not show the value
		return ValueString;
	}

/*Namespace End*/}

