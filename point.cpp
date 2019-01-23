#include <iostream>
#include "point.h"

using namespace std;

point::point(){
	window_size=0;

}



void point::setPoint(int index, std::vector <std::pair<double,double>> &arr, std::vector<point> &points)
{
	smoothArray(points,index,arr);	
	time=arr[index].first;
	getDerivativeData(index,points,arr);
	if(index+1>=window_size)
	{
		getSlope(index,points,arr);
		getAvg(index,points);
		getMinimum(index,points);
		getMaximum(index,points);
	}
	
	else  
	{  
		windowSlope=0;
		getAvg(index,points);
		minimum=256;
		maximum=0;
		minimumIndex=-1;
		maximumIndex=-1;
	}
}
void point :: smoothArray(std::vector<point> &points,int index, std::vector<std::pair<double,double>> &arr)
{	
	
	 value=0;

/* Unite conditions
	if(index < globals["PREV_NEIGHBOURS"] -1)
	{
		
		for(int i=0;i<=index;i++)
		{
			
			value+= arr[i].second;
		}
		
		value= value/(index+1);


	}
*/ 

	//else if (index==globals["PREV_NEIGHBOURS"]-1)
	if (index<=globals["PREV_NEIGHBOURS"]-1)
	{
		
		for(int i=0;i<=index;i++)
		{
			value+=arr[i].second;
		}

		value= value/(index+1);
	}
	else
	{
		int end=index-globals["PREV_NEIGHBOURS"];
		value=points[index-1].value-arr[end].second/globals["PREV_NEIGHBOURS"] +arr[index].second/globals["PREV_NEIGHBOURS"];
		
	}


}
void point :: getAvg(int index, std::vector<point> &points)
{
	if(index+1>window_size)
	{
		int end=index-window_size;
		avg=points[index-1].avg+(value-points[end].value)/window_size;
	}
	else
	{
		avg=0;

		for(int i=0;i<=index;i++)
		{
			//avg+=value;
			// this value hasn't been written to "points" yet
			if (i==index)
				avg+=value;
			else
				avg+=points[i].value;
		}

		avg=avg/(index+1);
	}

}
void point :: getMinimum(int index,std::vector<point> &points)
{
	minimum=256;
	
	if((points[index-1].minimumIndex==0) || (index==window_size-1))
	{
		int j=0;
		
		if(value<minimum)
		{
			minimum=value;
			minimumIndex=window_size-1;
		}
			
		for(int i=index-window_size+1;i<index;i++)
		{
			if(points[i].value<minimum)
			{
				minimum=points[i].value;
				minimumIndex=j;
			}
			j++;
		}
	}

else
{
	
    minimum= min(value,points[index-1].minimum);
    if(minimum==value)
	{
        minimumIndex=window_size-1;
	}
    else
	{
        minimumIndex=points[index-1].minimumIndex-1;
	}
}
	
}
void point :: getMaximum(int index,std::vector<point> &points)
{
	maximum=0;
	if((points[index-1].maximumIndex==0) || (index==window_size-1))
	{
		
		if(value>maximum)
		{
			maximum=value;
			maximumIndex=window_size-1;
		}

		int j=0;
		for(int i=index-window_size+1;i<index;i++)
		{
			if(points[i].value>maximum)
			{
				maximum=points[i].value;
				maximumIndex=j;
			}
			j++;
		}
	}
else
{
    maximum= max(value,points[index-1].maximum);
    if(maximum==value)
        maximumIndex=window_size-1;
    else
        maximumIndex=points[index-1].maximumIndex-1;
	}
	
}	 
void point :: getSlope(int index,std::vector<point> &points,std::vector <std::pair<double,double>> &arr)
{
	int end=index-window_size+1;

		//cout<<"value: "<< value<<" points[end].value: "<< points[end].value<<endl<<"arr[index].first: "<<arr[index].first<<"points[end].time: "<<points[end].time<<endl;
		
	
	windowSlope=(value-points[end].value)/(arr[index].first-points[end].time);
}
void point :: getDerivativeData(int index,std::vector<point> &points,std::vector <std::pair<double,double>> &arr)
{
	
	if(index==0)
	{
		 deravativeData_derivative=0;deravativeData_windowTendency=2;deravativeData_pointTendencyZero=2;
		 deravativeData_pointTendencyMargin=2;deravativeData_posPrecent=0;deravativeData_negPrecent=0;deravativeData_zeroPrecent=0;deravativeData_signShifts=0;deravativeData_currentSign=0;
	}

	else
	{

		deravativeData_derivative=((value-points[index-1].value)/(arr[index].first-points[index-1].time)); //yoav comment: removed epsilon, we have a division by zero problem 
		windowSlopesPrecent( index, points );
		getSignShifts( index,points );
	}

}
int point :: getWindowEnd(int index)
{
	int windowEnd;
	 if(index-window_size>0)
	 {
          windowEnd=index-window_size+1;
	 }
     else
	 {
         windowEnd=0;
	 }
	 return windowEnd;
}
void point :: getSignShifts(int index, std::vector<point> &points)
{
	int cSign=0;
	int	windowEnd=getWindowEnd(index);
	int sShifts=points[index-1].deravativeData_signShifts;
    
	if(index>	window_size)
	{
       // removing the window end
        if(points[windowEnd-1].deravativeData_currentSign != 0 && points[windowEnd].deravativeData_currentSign != 0 &&
			points[windowEnd-1].deravativeData_currentSign != points[windowEnd].deravativeData_currentSign)
		{
		   sShifts -= 1;
		}
	}

        //adding the window start
    if(deravativeData_pointTendencyMargin==points[index-1].deravativeData_currentSign || deravativeData_pointTendencyMargin==0)
	{
		cSign=points[index-1].deravativeData_currentSign;
	}
           
	else
	{
		if(points[index-1].deravativeData_currentSign != 0)
		{
			sShifts=sShifts+1;
		}

		cSign=deravativeData_pointTendencyMargin;
	}
       deravativeData_currentSign=cSign;
	   deravativeData_signShifts=sShifts;

}
bool point :: isSaw(int index,std::vector<point> &points)
{
	bool flag;
	    if((
			( points[index].deravativeData_signShifts>globals["MIN_SIGN_SHIFTS_FOR_SAW"]) &&
		   (max(points[index].deravativeData_posPrecent / points[index].deravativeData_negPrecent, points[index].deravativeData_negPrecent/points[index].deravativeData_posPrecent)>globals["MIN_POS_NEG_RATIO_FOR_SAW_STATES"])&&
		   (max(points[index].deravativeData_posPrecent / points[index].deravativeData_negPrecent, points[index].deravativeData_negPrecent/points[index].deravativeData_posPrecent)<globals["MAX_POS_NEG_RATIO_FOR_SAW_STATES"]))&&
		   (abs(points[index].maximum-points[index].minimum) > globals["MIN_MAX_DIFF_FOR_SAW"]) &&
		   (abs(points[index].maximum-points[index].minimum) < globals["MAX_MAX_DIFF_FOR_SAW"]) 
		  )
		{
			 flag=true;
		}
		else
		{
			 flag=false;
		}
    
    if (( points[index].deravativeData_posPrecent < globals["MIN_SLOPE_PRECENT_FOR_SAW"]) && ( points[index].deravativeData_negPrecent < globals["MIN_SLOPE_PRECENT_FOR_SAW"]) && ( points[index].deravativeData_zeroPrecent > globals["MIN_ZERO_PRECENT_FOR_NON_SAW"]))
	{
        flag=false;
	}

	return flag;
    
}
void point :: windowSlopesPrecent( int index,std::vector<point> &points )
	{

		
    double EPSILON=0.01*1/window_size;//1e-3;
    
  
    deravativeData_posPrecent=0;
    deravativeData_negPrecent=0;
    deravativeData_zeroPrecent=0;

    if(deravativeData_derivative>globals["POS_SLOPE"])
        deravativeData_pointTendencyMargin=1;    
    else if(deravativeData_derivative<globals["NEG_SLOPE"])
        deravativeData_pointTendencyMargin=-1;
    else
        deravativeData_pointTendencyMargin=0;
    
    
    if(deravativeData_derivative>0)
	{
        deravativeData_pointTendencyZero=1;    
	}
    else if(deravativeData_derivative<0)
	{
        deravativeData_pointTendencyZero=-1;
	}
    else deravativeData_pointTendencyZero=0;
  
    if(index+1==window_size)
	{
		for(int i=0;i<index;i++)
		{
			if(points[i].deravativeData_pointTendencyZero==1)
				deravativeData_posPrecent++;
			else if(points[i].deravativeData_pointTendencyZero==-1)
				deravativeData_negPrecent++;
			else if(points[i].deravativeData_pointTendencyZero==0)
				deravativeData_zeroPrecent++;
		}
		
		//process separately the last datum since it's not yet present in "points" structure
		if(deravativeData_pointTendencyZero==1)
			deravativeData_posPrecent++;
		else if(deravativeData_pointTendencyZero==-1)
			deravativeData_negPrecent++;
		else if(deravativeData_pointTendencyZero==0)
			deravativeData_zeroPrecent++;
	}


    if(index+1>window_size)
	{
		deravativeData_posPrecent = window_size * points[index-1].deravativeData_posPrecent;
		deravativeData_negPrecent = window_size * points[index-1].deravativeData_negPrecent;
		deravativeData_zeroPrecent = window_size *points[index-1].deravativeData_zeroPrecent;

		if(deravativeData_pointTendencyZero==1)
			deravativeData_posPrecent++;
		else if(deravativeData_pointTendencyZero==-1)
			deravativeData_negPrecent++;
		else if(deravativeData_pointTendencyZero==0)
			deravativeData_zeroPrecent++;


		//removing oldest
        if(points[index-window_size].deravativeData_pointTendencyZero==1)
            deravativeData_posPrecent--;
        else if (points[index-window_size].deravativeData_pointTendencyZero==-1)
            deravativeData_negPrecent--;
        else if (points[index-window_size].deravativeData_pointTendencyZero==0)
            deravativeData_zeroPrecent--;
	}

	deravativeData_posPrecent/=window_size;
	deravativeData_negPrecent/=window_size;
	deravativeData_zeroPrecent/=window_size;

    double winner=0;
    if ((deravativeData_posPrecent>globals["MIN_PERCENTAGE"]) || ((deravativeData_posPrecent>globals["MIN_PERCENTAGE"]/15) && (deravativeData_posPrecent/deravativeData_negPrecent >globals["MAX_POS_NEG_RATIO_TO_SET_ZERO"]))) 
	{

            deravativeData_windowTendency=1;
            winner=deravativeData_posPrecent;
	}
    
    if ((deravativeData_negPrecent>globals["MIN_PERCENTAGE"])  || ((deravativeData_negPrecent>globals["MIN_PERCENTAGE"]/15) && (deravativeData_negPrecent/deravativeData_posPrecent >globals["MAX_POS_NEG_RATIO_TO_SET_ZERO"])) && (deravativeData_negPrecent>winner) )
	{

            deravativeData_windowTendency=-1;
            winner=deravativeData_negPrecent;
	}
     if ((abs(deravativeData_posPrecent-deravativeData_negPrecent) < globals["POS_NEG_DIFF_THRESH"]) && (max(deravativeData_posPrecent/deravativeData_negPrecent, deravativeData_negPrecent/deravativeData_posPrecent)  < globals["MAX_POS_NEG_RATIO_TO_SET_ZERO"]) )
         winner=0;
     
     if (winner==0)
         deravativeData_windowTendency=0;
     
     if (abs(deravativeData_posPrecent)<EPSILON)
         deravativeData_posPrecent=0;
     
     if (abs(deravativeData_negPrecent)<EPSILON)
         deravativeData_negPrecent=0;
     
     if (abs(deravativeData_zeroPrecent)<EPSILON)
         deravativeData_zeroPrecent=0;
     
	}
state point :: currentState(std::vector<point> &points_small,std::vector<point> &points_large, int index)
{
	state s;
if(index<window_size)
    s= unknown;   
else if  ((!isSaw(index,points_large)) || (points_large[index].avg<globals["NIGHT_THRESHOLD"]))
{
    if (points_small[index].maximum-points_small[index].minimum>=globals["MIN_MAX_DIFF_LOW"]) 
            if  ((points_large[index].maximum-points_large[index].minimum > globals["MIN_MAX_DIFF_HIGH"]) || (max(points_large[index].deravativeData_posPrecent / points_large[index].deravativeData_negPrecent, points_large[index].deravativeData_negPrecent/points_large[index].deravativeData_posPrecent)>globals["MIN_POS_NEG_RATIO_FOR_EXP"]))
			{
                if (points_large[index].deravativeData_windowTendency==1) 
                    s= early_sunrise;

                else if (points_large[index].deravativeData_windowTendency==-1) 
                     s= late_sunset;
                else
				{
                    if (points_small[index].avg>globals["DAY_THRESHOLD"])
                        s= clouds ;
                    else
                        s= night;
				}
			}
            else
			{
                if (points_small[index].avg>globals["DAY_THRESHOLD"])
                    s= clouds;
                else
					s= night;
			}
    else
	{
        if(points_small[index].avg>globals["DAY_THRESHOLD"])
            s= day;
        else if(points_small[index].avg<globals["NIGHT_THRESHOLD"])
			s= night;
        else
            s= unknown;
	}
}
else
{
	if(points_large[index].deravativeData_posPrecent > points_large[index].deravativeData_negPrecent) 
		s= late_sunrise;
	else
		s= early_sunset;
}
	if(s==clouds)
	{
		clouds_count=globals["CLOUD_STATE_PERSIST_TIME_IN_SEC"]*frame_rate;
	}
	else
	{
		if(clouds_count>0)
		{
			clouds_count--;
			return clouds;
		}
	}
	return s;

}

void point :: setWindowSize(const int window)
{
	window_size = window;
}

point::~point()
{
}
