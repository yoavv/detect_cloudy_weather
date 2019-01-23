#include "pointArray.h"
using namespace std;

pointArray :: pointArray()
{
	small_window_size = 0;
	large_window_size = 0;

	remove_timestamps_flag = false;
}


void pointArray :: writeToFile()
{
  ofstream myfile ("example.csv");
  if (myfile.is_open())
  {
    myfile << "time,value,state\n";
    for(int row = 0; row < arr.size() ; row ++)
	{

			myfile << arr.at(row).first << "," ;
			myfile << arr.at(row).second << "," ;
			myfile<<"\n";
    }
	
    myfile.close();
  }
  else cout << "Unable to open file";


}



void pointArray :: excelToVector(char* filename)
	{
	ifstream file(filename);
    string row, value;
	double a[2];
	int hour, minute, second, millisecond;
	int a_day=0;
	double prev_time_val = 0;
	double one_sec_val = 1/(24*3600);

	
    for(int i=0 ;file.good(); i++)
    {
        getline(file, row); //gets the row

        istringstream tmp(row); //saves the row as the string
        for(int width=0 ; width <point::globals["NUM_COLS"]  ; width++) 
        {
            getline(tmp, value, ','); //extracts the comma for the values
			if (width==0)
			{
				//strcpy(time_str,value.c_str());
				sscanf(value.c_str(), "%d:%d:%d.%d", &hour, &minute, &second, &millisecond);
				a[width]= (a_day + hour*3600 + minute*60 + second + (double)millisecond/1000) / (24*3600);
				if (prev_time_val - a[width] > one_sec_val) // to avoid misprecisions on double on the same consecutive time_stamps
				{
					a_day=1;
					a[width]+=1;
				}
				prev_time_val=a[width];
			}
			else
				a[width]=atof(value.c_str());		
        }
		
		//addToArr(a[0],a[1]);  //TEMP - uncomment after testing
		process(false, a[1],a[0]); //TEMP - remove after testing

		//Output the progress to console
		if (!(i%10000))
			cout << i << endl;
    }

	//point::frame_rate=(point::globals["FRAME_NUM"]-1)/(24*3600*(arr.at(point::globals["FRAME_NUM"]).first - (arr.at(0).first)));
	//For testing
	point::frame_rate= 7.234717363284847;

	setWindowSize(false);
	cout<<"small window size:"<<small_window_size<<", frame rate:"<<point::frame_rate<<endl;
	cout<<"large window size:"<<large_window_size<<", frame rate:"<<point::frame_rate<<endl;
	
	
}

void pointArray :: addToArr(double a0,double a1)
{
	arr.push_back(make_pair(a0,a1));
}
void pointArray :: process(char* filename)//file
{
	excelToVector(filename);



	
/* TEMP - uncoment after testing	
	for(int i=0;i<arr.size();i++)
	{
		point p_small;
		point p_large;

		p_small.setWindowSize(small_window_size);
		p_large.setWindowSize(large_window_size);

		p_small.setPoint( i, arr, points_small);
		p_large.setPoint( i, arr, points_large);

		points_small.push_back(p_small);
		points_large.push_back(p_large);

		states.push_back(points_small[i].currentState(points_small, points_large, i));
		
		//Output the progress to console
		if (!(i%1000))
			cout << "Processing frame:" << i << endl;

		
	}
*/	
	
}
state pointArray :: process(bool if_dynamic_frame_rate, double value, double time_stamp=-1)
{


	if(frame_count>0)
	{
		int i=(point::frame_rate*point::globals["WINDOW_SIZE_UPDATE_TIME_IN_SEC"]);
		if(frame_count % i==0)
		{
			point::frame_rate = (i/(points_large.at(points_large.size()-1).time - points_large.at(points_large.size()-i).time)/point::globals["WINDOW_SIZE_UPDATE_TIME_IN_SEC"]);
				setWindowSize(false);
		}
	}

	double t2 = (time_stamp<0) ? getTime() : time_stamp;
	
	time_stamp_buffer.push(t2);
	if (time_stamp_buffer.size() > 1)
	{

		if (frame_count+1 == point::globals["FRAME_NUM"])
			remove_timestamps_flag = true;
		if (remove_timestamps_flag)
			time_stamp_buffer.pop();

		double t1 = time_stamp_buffer.front();
		
		point::frame_rate=(time_stamp_buffer.size())/(24*3600*(t2-t1));

		//treat infinity
		if (point::frame_rate > 1000)
			point::frame_rate = 1000;

		//cout << "frame rate:" << point::frame_rate << endl;
		if (!if_dynamic_frame_rate)
			point::frame_rate= 7.234717363284847;

		//setWindowSize(false);
	}
	else
		setWindowSize(true);
	
	
/*
	setWindowSize(true);
	
	if(frame_count<=point::globals["FRAME_NUM"])
	{
		if (frame_count==0)
		{
			setWindowSize(true);
			t1 = (time_stamp<0) ? getTime() : time_stamp;
			cout<<t1<<endl;
		}
			
		if(frame_count==point::globals["FRAME_NUM"])
		{
			t2 = (time_stamp<0) ? getTime() : time_stamp;
			cout<<t2<<endl;
			point::frame_rate=(point::globals["FRAME_NUM"]-1)/(t2-t1);
			//For testing
			//point::frame_rate= 7.234717363284847;

			cout<<t2-t1<<endl;

			cout<<point::frame_rate<<endl;
			setWindowSize(false);
		}
		
		//frame_count++;
		//return unknown;
	}
*/
	
	if (time_stamp<0)
		addToArr(getTime(),value);
	else
		addToArr(time_stamp,value);
		
	point p_small;
	point p_large;

	p_small.setWindowSize(small_window_size);
	p_large.setWindowSize(large_window_size);
		
	//int index =frame_count - point::globals["FRAME_NUM"] - 1;

	int index =frame_count-deleted_count;
	
	p_small.setPoint( index, arr, points_small);
	p_large.setPoint( index, arr, points_large);

	points_small.push_back(p_small);
	points_large.push_back(p_large);

	state result = points_small[index].currentState(points_small, points_large,index);

	states.push_back(result);
	frame_count++;
	if(frame_count-deleted_count>point::globals["MAX_FRAME_RATE"]*1.5*795+10000+2)
	{
		
		cout<<"frame_count:"<<frame_count<<" deleted_count :"<<deleted_count<<" large_window_size :"<<large_window_size<<endl;
		writePartTofile();
		
		points_small.erase(points_small.begin(),points_small.begin()+10000);
		points_large.erase(points_large.begin(),points_large.begin()+10000);
		arr.erase(arr.begin(),arr.begin()+10000);
		states.erase(states.begin(),states.begin()+10000);
		deleted_count+=10000;
	}
	return result;
}
void pointArray :: vectorToFile()
{
  ofstream myfile ("example.csv");
  if (myfile.is_open())
  {
    myfile << "time,value,state\n";
	int i;

	for(int i=0;i<arr.size();i++)
		myfile<<arr.at(i).first<<","<<arr.at(i).second<<","<<states[i]<<endl;
	
	
    myfile.close();
  }
  else cout << "Unable to open file";

}
void pointArray :: pointsTofile(std::vector<point> &points, const int id)
{
	char file_name[20];
	sprintf(file_name, "%s%d%s", "points",id,".csv");

	//  ofstream myfile ("points.csv");
	  ofstream myfile (file_name);
  if (myfile.is_open())
  {
    myfile << std::setprecision(15)<<"value," 
<<"time,"
<<"avg,"
<<"minimum,"
<<"minimumIndex,"
<<"maximum,"
<<"maximumIndex,"
<<"windowSlope,"
<<"deravativeData_derivative,"
<<"deravativeData_posPrecent,"
<<"deravativeData_negPrecent,"
<<"deravativeData_zeroPrecent,"
<<"deravativeData_windowTendency,"
<<"deravativeData_pointTendencyZero,"
<<"deravativeData_pointTendencyMargin,"
<<"deravativeData_signShifts,"
<<"deravativeData_currentSign,"



			<<endl;


	for(int i=0;i<arr.size();i++){
		std:myfile <<  points[i].value << ","
		<<points[i].time<<","
		<<points[i].avg<<","
		<<points[i].minimum<<","
		<<points[i].minimumIndex<<","
		<<points[i].maximum<<","
		<<points[i].maximumIndex<<","
		<<points[i].windowSlope<<","
		<<points[i].deravativeData_derivative<<","
		<<points[i].deravativeData_posPrecent<<","
		<<points[i].deravativeData_negPrecent<<","
		<<points[i].deravativeData_zeroPrecent<<","
		<<points[i].deravativeData_windowTendency<<","
		<<points[i].deravativeData_pointTendencyZero<<","
		<<points[i].deravativeData_pointTendencyMargin<<","
		<<points[i].deravativeData_signShifts<<","
		<<points[i].deravativeData_currentSign<<","
		
		<<endl;
	}
	
	
    myfile.close();
  }
  else cout << "Unable to open file";
}
void pointArray :: logData()
{
	cout << "Writing points to files." << endl;
	pointsTofile(points_small,1);	
	pointsTofile(points_large,2);	
	
	cout << "Writing states to file." << endl;
	vectorToFile();
}
void pointArray :: setWindowSize(const bool init)
{
	if (init)
	{
		small_window_size=1.5*point::globals["FRAME_NUM"];
		large_window_size=1.5*point::globals["FRAME_NUM"];
	}
	else
	{
		small_window_size  =ceil(1.5*795*point::frame_rate/5);
		large_window_size  =ceil(1.5*795*point::frame_rate);	
	}
	
}
double pointArray :: getTime()
{
	SYSTEMTIME time;

	GetSystemTime(&time);

	double mili= (((((time.wDay*24)+time.wHour)*60+time.wMinute)*60 +time.wSecond) *1000) + time.wMilliseconds;
	return double(mili/1000);
}
void pointArray:: writePartTofile()
{
	std::ofstream myfile;
	myfile.open("example.csv",std::ios_base::app);
	for(int i=0;i<10000;i++)
	{
		myfile <<  points_large[i].value << ","
		<<points_large[i].time<<","
		<<points_large[i].avg<<","
		<<points_large[i].minimum<<","
		<<points_large[i].minimumIndex<<","
		<<points_large[i].maximum<<","
		<<points_large[i].maximumIndex<<","
		<<points_large[i].windowSlope<<","
		<<points_large[i].deravativeData_derivative<<","
		<<points_large[i].deravativeData_posPrecent<<","
		<<points_large[i].deravativeData_negPrecent<<","
		<<points_large[i].deravativeData_zeroPrecent<<","
		<<points_large[i].deravativeData_windowTendency<<","
		<<points_large[i].deravativeData_pointTendencyZero<<","
		<<points_large[i].deravativeData_pointTendencyMargin<<","
		<<points_large[i].deravativeData_signShifts<<","
		<<points_large[i].deravativeData_currentSign<<","
		<<states[i]<<","
		<<endl;
	}


}
pointArray::~pointArray(void)
{

}