#if(0)

#warning This is just a file to store old stuff which needs to be reimplemented do not compile!

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




/**
 * @todo Remove this old stuff after implementing in analyzer class
 */

//Function for getting error string of a value error number
std::string getFluke189ValueErrorString(unsigned int DisplayErrorNo);  //integrated

//Function for extracting additional Data out of a SerialResponse of command QD 0
//Fluke::Fluke189::analysedInfo_t Fluke189AnalyseQdInfo(Fluke::Fluke189::qdInfo_t* qdInfo);


//Struct to store values with their unit
typedef struct fluke189Value_t
{
	fluke189Value_t()
	{
		strUnit=' ';
	}
	/** Value from Fluke 189 (integer)*/
	signed   int intValue;
	unsigned int intDecimal;	//Place of Decimal Point
	signed   int intPrefix;		//Prefix
	std::string  strUnit;		//Unit: V, A, S, s etc...
	std::string	 strSymbolsAfter;  //Symbols to be placed after value and unit
	std::string  strSymbolsBefore; //Symbols to be placed before value
} fluke189Value;

//This function decides if operandSmall is smaller than operandBig
bool fluke189ValueSmallerThan(fluke189Value_t operandSmall,fluke189Value_t operandBig);
//This function generates a string out of a fluke189Value
std::string fluke189ValueToString(fluke189Value_t value);

//This function uses pointers to variables to calculate minimum, maximum and average value
void fluke189ValueMinMaxAverage(fluke189Value_t current, fluke189Value_t& min, fluke189Value_t& max, fluke189Value_t& avg, long long& stat_average, long& stat_datanumber, bool& reset,std::string& stat_unit);



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



#endif




