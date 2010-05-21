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
	std::vector<Fluke189::SimpleListElement__QD4_BIN__t>Fluke189::CMD_QD4_CreateSimpleList_BIN()
	{
		RCT_QD4 rct = this->CMD_QD4(1);
		std::vector<Fluke189::SimpleListElement__QD4_BIN__t> list;
		//TODO Add error reporting ... etc....
		SimpleListElement__QD4_BIN__t currentelement;

		for(unsigned int i=0;i<rct.Data()->DataCount;i++)
		{
			//Special Modes
			currentelement.Auto=rct.Data()->dsets[i].I_S_Auto;
			currentelement.Hold=rct.Data()->dsets[i].I_S_Hold;
			currentelement.Log=rct.Data()->dsets[i].I_S_Log;

			//MIN MAX AVG
			currentelement.Avg=rct.Data()->dsets[i].I_S_Avg;
			currentelement.Max=rct.Data()->dsets[i].I_S_Max;
			currentelement.Min=rct.Data()->dsets[i].I_S_Min;


			//Errors
			currentelement.Leads=rct.Data()->dsets[i].I_ErrorNoPV0==LEADS_CONNECTION_WRONG ||
								 rct.Data()->dsets[i].I_ErrorNoPV1==LEADS_CONNECTION_WRONG;

			currentelement.Fuse=rct.Data()->dsets[i].I_ErrorSV==FUSE;
			currentelement.Open=rct.Data()->dsets[i].I_ErrorNoPV0==OPEN__NOTHING_CONNECTED ||
								rct.Data()->dsets[i].I_ErrorNoPV1==OPEN__NOTHING_CONNECTED;

			currentelement.Error=rct.Data()->dsets[i].I_ErrorPV0 ||
								 rct.Data()->dsets[i].I_ErrorPV1 ||
								 rct.Data()->dsets[i].I_ErrorSV;


			//Low Battery Warning
			currentelement.LowBat=rct.Data()->dsets[i].I_LowBattery;

			//Range
			switch(rct.Data()->dsets[i].I_RangeMode)
			{
			case RANGE_1: 		currentelement.Range=1; 	break;
			case RANGE_10: 		currentelement.Range=10;	break;
			case RANGE_100: 	currentelement.Range=100; 	break;
			case RANGE_1000: 	currentelement.Range=1000; 	break;
			case RANGE_5: 		currentelement.Range=5; 	break;
			case RANGE_50: 		currentelement.Range=50; 	break;
			case RANGE_500: 	currentelement.Range=500; 	break;
			case RANGE_5000: 	currentelement.Range=5000; 	break;
			}

			switch(rct.Data()->dsets[i].I_MeasureMode)
			{
			case MM_AC_Ampere:
				currentelement.PriUnit=SL_AC_Ampere;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_AC_Volt:
				currentelement.PriUnit=SL_AC_Volts;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_AC_mVolt:
				currentelement.PriUnit=SL_AC_Volts;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_AC_uAmpere:
				currentelement.PriUnit=SL_AC_Ampere;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_CAP_Farad:
				currentelement.PriUnit=SL_Farad;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_DC_A_Submode:

				break;
			case MM_DC_Ampere:
				currentelement.PriUnit=SL_DC_Ampere;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_DC_V_Submode:

				break;
			case MM_DC_Volt:
				currentelement.PriUnit=SL_DC_Volts;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_DC_mA_Submode:

				break;
			case MM_DC_mV_Submode:

				break;
			case MM_DC_mVolt:
				currentelement.PriUnit=SL_DC_Volts;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_DC_uAmpere:
				currentelement.PriUnit=SL_DC_Ampere;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_Diode_Volt:
				currentelement.PriUnit=SL_DC_Volts;//??
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_POS_BETW_2_MODES:
				currentelement.PriUnit=SL_DISP_OFF;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_R_BEEP_Ohm:
				currentelement.PriUnit=SL_Ohm;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_R_Ohm:
				currentelement.PriUnit=SL_Ohm;
				currentelement.SecUnit=SL_DISP_OFF;
				break;
			case MM_Temp_Celsius:
				currentelement.PriUnit=SL_Celsius;
				currentelement.SecUnit=SL_Celsius;
				break;
			case MM_Temp_Fahrenheit:
				currentelement.PriUnit=SL_Fahrenheit;
				currentelement.SecUnit=SL_Fahrenheit;
				break;

//			default:
				//TODO Error
			}

			//Ampere Substates
			if(rct.Data()->dsets[i].I_MeasureMode==MM_DC_A_Submode ||
		       rct.Data()->dsets[i].I_MeasureMode==MM_DC_mA_Submode)
			{
				switch(rct.Data()->dsets[i].I_SubState_ACDC)
				{
				case 0:
					//TODO Error
					break;
				case 1:
					currentelement.PriUnit=SL_AC_Ampere;
					currentelement.SecUnit=SL_DC_Ampere;
					break;
				case 2:
					currentelement.PriUnit=SL_DC_Ampere;
					currentelement.SecUnit=SL_AC_Ampere;
					break;
				case 3:
					currentelement.PriUnit=SL_ACDC_Ampere;
					currentelement.SecUnit=SL_DISP_OFF;
					break;
				}
			}
			//Volt Substates
			if(rct.Data()->dsets[i].I_MeasureMode==MM_DC_V_Submode ||
			   rct.Data()->dsets[i].I_MeasureMode==MM_DC_mV_Submode)
			{
				switch(rct.Data()->dsets[i].I_SubState_ACDC)
				{
				case 0:
					//TODO Error
					break;
				case 1:
					currentelement.PriUnit=SL_AC_Volts;
					currentelement.SecUnit=SL_DC_Volts;
					break;
				case 2:
					currentelement.PriUnit=SL_DC_Volts;
					currentelement.SecUnit=SL_AC_Volts;
					break;
				case 3:
					currentelement.PriUnit=SL_ACDC_Volts;
					currentelement.SecUnit=SL_DISP_OFF;
					break;
				}
			}


			if(rct.Data()->dsets[i].I_Delta)
			{
				currentelement.Delta=1;
				currentelement.SecUnit=currentelement.PriUnit;
			}

			if(rct.Data()->dsets[i].I_DeltaPercent)
			{
				currentelement.Delta=1;
				currentelement.Percent=1;
				currentelement.PriUnit=SL_Percent;
				currentelement.SecUnit=currentelement.PriUnit;
			}
			if(rct.Data()->dsets[i].I_Unit_AC_dB)
			{
				currentelement.PriUnit=SL_DC_Volts;
				currentelement.SecUnit=SL_dB;
			}

			if(rct.Data()->dsets[i].I_Unit_AC_dB)
			{
				currentelement.PriUnit=SL_dB;
				currentelement.SecUnit=SL_DC_Volts;
			}

			//Rising Etch / Falling Etch
			currentelement.FallingEtch=rct.Data()->dsets[i].I_FallingEtch;
			currentelement.RisingEtch= rct.Data()->dsets[i].I_RisingEtch;



			//Add the current element to the vector list
			list.push_back(currentelement);
		}

		return list;
	}

}

