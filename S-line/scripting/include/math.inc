/* Version 1.1 (31.01.2004) */

#if defined _math_included
  #endinput
#endif
#define _math_included

#include <core>
#include <fixed>
#include <string>
#include <adminlib>

#define LGCOUNT 15

stock fixtostr(fixed:fNumber,sNumber[],iMaxLength){
	new iNumber,iFrac;
	new iSign=1;
	new sFrac[4];

	if(fNumber<0.000){
		iNumber=fround(fNumber,fround_ceil);
	}
	else{
		iNumber=fround(fNumber,fround_floor);
	}
	iFrac=ffract(fNumber);
	if(iFrac<0){
		iFrac*=-1;
		iSign=-1;
	}
	numtostr(iFrac,sFrac);
	switch(strlen(sFrac)){
		case 2:
			snprintf(sNumber,iMaxLength,"%d.0%s",iNumber,sFrac);
		case 1:
			snprintf(sNumber,iMaxLength,"%d.00%s",iNumber,sFrac);
		default:
			snprintf(sNumber,iMaxLength,"%d.%s",iNumber,sFrac);
	}
	if(iSign==-1 && iNumber==0){
		snprintf(sNumber,iMaxLength,"-%s",sNumber);
	}
	return PLUGIN_CONTINUE;
}

stock fixed:strtofix(sNumber[]){
	new fixed:fNumber;

	if(strtrim(sNumber,"-",0)>0){
		fNumber=fmul(fixedstr(sNumber),-1.000);
	}
	else{
		fNumber=fixedstr(sNumber);
	}

	return fNumber;
}

stock matherror(iError){
	new log_msg[MAX_TEXT_LENGTH];

	switch(iError){
		case 1:
			strcpy(log_msg,"No valid value for function input!",MAX_TEXT_LENGTH);
		case 2:
			strcpy(log_msg,"Number too high for allowed range!",MAX_TEXT_LENGTH);
		case 3:
			strcpy(log_msg,"Number too low for allowed range!",MAX_TEXT_LENGTH);
		default:
			return PLUGIN_CONTINUE;
	}
	log(log_msg);
	return PLUGIN_CONTINUE;
}

stock fixed:f_log10(fixed:fValue,&iError=0){
	new fixed:fLG[LGCOUNT]={2147482.000,10000.000,100.000,10.000,3.162,1.778,1.334,1.155,1.075,1.037,1.018,1.009,1.005,1.002,1.000};
	new fixed:fLGFract[LGCOUNT-1]={4.000,2.000,1.000,0.500,0.250,0.125,0.063,0.031,0.016,0.008,0.004,0.002,0.001,0.000};
	new fixed:fLocalValue=0.000;
	new fixed:fLower=0.000;

	if(fValue<=0.000){
		iError=1;
		return 0.000;
	}
	if(fValue==2147482.000){
		iError=2;
		return 6.332;
	}
	if(fValue==0.001){
		return -3.000;
	}

	for(new i=1;i<4;i++){
		if(fValue>=1.000){
			break;
		}
		fValue=fmul(fValue,10.000);
		fLower=fLower+1.000;
	}

	for(new i=0;i<LGCOUNT-1;i++){
		if(fValue<fLG[i]){
			if(fValue>=fLG[i+1]){
				fLocalValue=fLocalValue+fLGFract[i];
				fValue=fdiv(fValue,fLG[i+1]);
			}
		}
		else if(fValue==fLG[i]){
			break;
		}
	}
	iError=0;

	return fLocalValue-fLower;
}

stock fixed:f_ln(fixed:fValue,&iError=0){
	new fixed:fFactor=2.303;
	new fixed:fCheck;
	new fixed:fMultiply;
	new fixed:fKeepResult;

	fCheck=fValue;

	fValue=f_log10(fValue,iError);
	if(iError>0){
		return fValue;
	}

	fValue=fmul(fValue,fFactor);
	fKeepResult=fValue;
	fKeepResult=f_powere(fKeepResult,iError);

	if(iError>0){
		return fKeepResult;
	}

	if(fKeepResult==fCheck){
		return fValue;
	}
	if(fKeepResult>fCheck){
		fMultiply=-0.001;
	}
	else if(fKeepResult<fCheck){
		fMultiply=0.001;
	}
	for(new i=0;i<1000;i++){
		fKeepResult=f_powere(fValue+fmul(fixed(i),fMultiply),iError);
		if(iError>0){
			return fKeepResult;
		}
		if(fKeepResult==fCheck){
			return fValue+fmul(fixed(i-1),fMultiply);
		}
	}

	return fValue;
}

stock fixed:f_logab(fixed:fBase,fixed:fValue,&iError=0){

	fBase=f_log10(fBase,iError);
	if(iError>0){
		return fBase;
	}
	fValue=f_log10(fValue,iError);
	if(iError>0){
		return fValue;
	}

	return fdiv(fValue,fBase);
}

stock fixed:f_faculty(fixed:fValue,&iError=0){
	new iValue=1;

	if(ffract(fValue)==0.000){
		if(fValue>0.000){
			if(fValue<10.000){
				iValue=fround(fValue);
				fValue=1.000;
				for(new i=2;i<=iValue;i++){
					fValue=fmul(fValue,fixed(i));
				}
				return fValue;
			}
			iError=2;
			return 2147482.000;
		}
	}
	iError=1;

	return -1.000;
}

stock fixed:f_powere(fixed:fExponent,&iError=0){
	new fixed:fEulerArray[3][10]={{1.000,1.105,1.221,1.350,1.492,1.649,1.822,2.014,2.226,2.460},{1.000,1.010,1.020,1.030,1.041,1.051,1.062,1.073,1.083,1.094},{1.000,1.001,1.002,1.003,1.004,1.005,1.006,1.007,1.008,1.009}};
	new fixed:fSignE=1.000;
	new fixed:fEuler;
	new fixed:fLocalResult=1.000;
	new iKeepExponent=0;

	fEuler=f_euler();
 
	if(fExponent==0.000){
		return 1.000;
	}

	if(fExponent>14.579){
		iError=2;
		return 2147482.000;
	}

	if(fExponent<0.000){
		fSignE=-1.000;
		fExponent=fExponent*fSignE;
	}

	iKeepExponent=fround(fExponent,fround_floor);
	if(iKeepExponent!=0){
		for(new i=0;i<iKeepExponent;i++){
			fLocalResult=fmul(fEuler,fLocalResult);
		}
	}

	for(new i=0;i<3;i++){
		fExponent=fExponent-fixed(iKeepExponent);		
		fExponent=fmul(fExponent,10.000);
		iKeepExponent=fround(fExponent,fround_floor);
		fLocalResult=fmul(fLocalResult,fEulerArray[i][iKeepExponent]);
	}

	if(fSignE==-1.000){
		fLocalResult=fdiv(1.000,fLocalResult);
	}

	return fLocalResult;
}

stock fixed:f_power(fixed:fBasis,fixed:fExponent,&iError=0){
	new fixed:fKeepValue;
	new iKeep;
	new iSign=0;

	iKeep=fround(fExponent,fround_floor);

	if(fBasis<0.000){
		if(fixed(iKeep)!=fExponent){
			iError=1;
			return -1.000;
		}
	}

	if(fExponent==0.500){
		return f_sqrt(fBasis,iError);
	}

	fKeepValue=fBasis;

	if(fixed(iKeep)==fExponent){
		if(fExponent<0.000){
			fExponent=fmul(fExponent,-1.000);
			iKeep=iKeep*-1;
			iSign=1;
		}
		for(new i=1;i<iKeep;i++){
			fBasis=fmul(fBasis,fKeepValue);
		}
		if(iSign){
			fBasis=fdiv(1.000,fBasis);
		}
		return fBasis;
	}

	fKeepValue=f_ln(fKeepValue,iError);
	if(iError>0){
		return -1.000;
	}

	return f_powere(fmul(fExponent,fKeepValue),iError);
}

stock fixed:f_euler(){
	return 2.718;
}

stock fixed:f_pi(){
	return 3.142;
}

stock fixed:f_sqrt(fixed:fNum,&iError=0,fixed:fNumStart=1.000) { 
	new fixed:fNum2=fNum;
	new fixed:fOne=1.000;
	
	if(f_abs(fNum)!=fNum){
		iError=1;		
		return -1.000;
	}

	if(fNumStart==fNum2){
		fNumStart=1.000;
	}

	if(fNum<1.000){
		fNum2=fdiv(fOne,fNum);
	}

	for(new i=0;i<100;i++){
		fNum2=fdiv(fNum2+fNumStart,2.000); 
		fNumStart=fdiv(fNum,fNum2);
		if(fNum2 <= fNumStart){
			break;
		}
	}

	return fNum2; 
}

stock fixed:f_abs(fixed:fNum) { 
	if(fNum >= 0.000) { 
		return fNum; 
	}
	return fmul(fNum,-1.000); 
}

stock fixed:f_max(fixed:fNum,fixed:fNum2){
	if(fNum >= fNum2) { 
		return fNum; 
	}
	return fNum2; 
}

stock fixed:f_min(fixed:fNum,fixed:fNum2){
	if(fNum < fNum2) { 
		return fNum; 
	}
	return fNum2; 
}

stock distance(x1,x2,y1=0,y2=0,z1=0,z2=0){ 
	new fixed:fNumStart=-1.000;
	new fixed:fSum=0.000;
	new iDx,iDy,iDz;
	new iError=0;

	iDx=(x1-x2)*(x1-x2);
	iDy=(y1-y2)*(y1-y2);
	iDz=(z1-z2)*(z1-z2);

	fNumStart=fixed(max(max(iDx,iDy),iDz));
	fSum=fixed(iDx+iDy+iDz);
	if(fSum==0.000){
		return 0;
	}

	fSum=f_sqrt(fSum,iError,fNumStart);

	return fround(fSum);
}

stock fixed:f_sin(fixed:fNum){ 
	new iSign=1;
	new fixed:fDummy=0.000;
	new fixed:fDummy2=f_pi();

	if(fNum<0.000){
		fNum=fmul(fNum,-1.000);
		iSign=-1;
	}

	fDummy2=fmul(fDummy2,2.000);
	fDummy=fdiv(fNum,fDummy2);
	if(fDummy>1.000){
		fDummy=fixed(fround(fDummy,fround_floor));
		fNum=fNum-fmul(fDummy,fDummy2);
	}
	fDummy2=f_pi();
	if(fNum>fDummy2){
		iSign=iSign*-1;
		fNum=fNum-fDummy2;
	}

	fDummy=fNum;
	fDummy2=fNum;
	for(new i=3;i<=11;i=i+2){
		fDummy=fmul(fdiv(fmul(fDummy,fmul(fNum,fNum)),fmul(fixed(i),fixed(i-1))),-1.000);
		fDummy2=fDummy2+fDummy;
	}

	return fmul(fDummy2,fixed(iSign)); 
}

stock fixed:f_cos(fixed:fNum){
	return f_sin(fNum+fdiv(f_pi(),2.000));
}

stock fixed:f_tan(fixed:fNum){
	return fdiv(f_sin(fNum),f_cos(fNum));
}

stock fixed:f_cot(fixed:fNum){
	return fdiv(f_cos(fNum),f_sin(fNum));
}

/* Calculation of the integral 1/(1+x^2) in order to get the arctan */
stock fixed:f_arctan(fixed:fNum,&iError=0){ 
	new fixed:fDummy;
	new fixed:fResult;
	new fixed:fRange;
	new iNum;

	if(fNum>1465.428){
		iError=2;
		return 2147482.000;
	}

	if(fNum==0.000){
		return 0.000;
	}

	fDummy=f_abs(fmul(fNum,1000.000));
	iNum=fround(fDummy)-1;
	fRange=f_max(fNum,0.000);

	fResult=fdiv(f_arctan_help(fRange),2.000);

	fDummy=fRange+0.001;
	for(new i=1;i<iNum;i++){
		fResult=fResult+f_arctan_help(fDummy);
		fDummy=fDummy+0.001;
	}

	return fmul(fResult+fdiv(f_arctan_help(fDummy),2.000),fdiv(fNum,fixed(iNum+1))); 
}

stock fixed:f_arctan_help(fixed:fNum){
	return fdiv(1.000,1.000+fmul(fNum,fNum));
}

stock fixed:f_arcsin(fixed:fNum,&iError=0){

	if(fNum>1.000){
		iError=2;
		return 1.571;
	}
	if(fNum<-1.000){
		iError=3;
		return -1.571;
	}
	if(fNum==1.000){
		return 1.571;
	}
	if(fNum==-1.000){
		return -1.571;
	}

	return f_arctan(fdiv(fNum,f_sqrt(1-fmul(fNum,fNum))));
}

stock fixed:f_arccos(fixed:fNum,&iError=0){

	if(fNum>1.000){
		iError=2;
		return 1.571;
	}
	if(fNum<-1.000){
		iError=3;
		return -1.571;
	}
	if(fNum==1.000){
		return 1.571;
	}
	if(fNum==-1.000){
		return -1.571;
	}

	return fdiv(f_pi(),2.000)-f_arcsin(fNum);
}

stock fixed:f_arccot(fixed:fNum){
	return fdiv(f_pi(),2.000)-f_arctan(fNum);
}

stock fixed:f_sinh(fixed:fNum){
	return fdiv(f_powere(fNum)-f_powere(fmul(fNum,-1.000)),2.000);
}

stock fixed:f_cosh(fixed:fNum){
	return fdiv(f_powere(fNum)+f_powere(fmul(fNum,-1.000)),2.000);
}

stock fixed:f_tanh(fixed:fNum){
	return fdiv(f_sinh(fNum),f_cosh(fNum));
}

stock fixed:f_coth(fixed:fNum,&iError=0){
	if(fNum==0.000){
		iError=1;
		return 0.000;
	}
	return fdiv(f_cosh(fNum),f_sinh(fNum));
}

stock fixed:f_arsinh(fixed:fNum,&iError=0){
	if(fNum>1465.428){
		iError=2;
		return 2147482.000;
	}
	return f_ln(fNum+f_sqrt(fmul(fNum,fNum)+1.000));
}

stock fixed:f_arcosh(fixed:fNum,&iError=0){
	if(fNum>1465.428){
		iError=2;
		return 2147482.000;
	}
	if(fNum<1.000){
		iError=3;
		return 0.000;
	}
	return f_ln(fNum+f_sqrt(fmul(fNum,fNum)-1.000));
}

stock fixed:f_artanh(fixed:fNum,&iError=0){
	if(fNum>=1.000){
		iError=2;
		return 0.000;
	}
	if(fNum<=-1.000){
		iError=3;
		return 0.000;
	}
	return fdiv(f_ln(fdiv(1.000+fNum,1.000-fNum)),2.000);
}

stock fixed:f_arcoth(fixed:fNum,&iError=0){
	if(fNum<=1.000){
		if(fNum>=-1.000){
			iError=1;
			return 0.000;
		}
	}
	return fdiv(f_ln(fdiv(1.000+fNum,fNum-1.000)),2.000);
}

stock fixed:f_degtorad(fixed:fNum){
	return fmul(fdiv(fNum,180.000),f_pi());
}

stock fixed:f_radtodeg(fixed:fNum){
	return fdiv(fmul(180.000,fNum),f_pi());
}

