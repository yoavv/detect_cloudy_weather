#pragma once
#include "globals.h"
//#include "pointArray.h"

enum state{unknown,early_sunrise,late_sunrise,day,clouds,early_sunset,late_sunset,night};
class point{

public:


	double value,time,avg,minimum,minimumIndex,maximum,maximumIndex,windowSlope,deravativeData_derivative,deravativeData_posPrecent,deravativeData_negPrecent;
	double deravativeData_zeroPrecent,deravativeData_windowTendency;
	int deravativeData_currentSign,deravativeData_signShifts;
	int deravativeData_pointTendencyZero,deravativeData_pointTendencyMargin;

	static std::map<std::string, double> globals;
	static double frame_rate;
	static int clouds_count;
	int window_size;

	point();
	state currentState(std::vector<point> &points_small,std::vector<point> &points_large, int index);
	void setPoint(int index, std::vector <std::pair<double,double>> &arr, std::vector<point> &points);
	void setWindowSize(const int window);
	~point();


private:
	void getAvg(int index, std::vector<point> &points);
	void getDerivativeData(int index,std::vector<point> &points,std::vector <std::pair<double,double>> &arr);
	void getMaximum(int index,std::vector<point> &points);
	void getMinimum(int index,std::vector<point> &points);
	bool isSaw(int index,std::vector<point> &points);
	void getSignShifts(int index, std::vector<point> &points);
	void getSlope(int index,std::vector<point> &points,std::vector <std::pair<double,double>> &arr);
	int getWindowEnd(int index);
	void  windowSlopesPrecent( int index, std::vector<point> &points );
	void smoothArray(std::vector<point> &points,int index, std::vector<std::pair<double,double>> &arr);
	
};


