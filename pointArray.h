#pragma once
#include "point.h"
#include "globals.h"
#include <queue>

class pointArray
{
private:
	int small_window_size, large_window_size;
public:

	std::vector<point> points_small; //smaller window
	std::vector<point> points_large; //larger window
	std::vector<state> states;
	std::vector<std::pair<double,double>> arr;
	std::queue<double> time_stamp_buffer;
	bool remove_timestamps_flag;
	static int frame_count;
	static int deleted_count;
	double t1,t2;


	pointArray();
	pointArray(char* filename);
	void writeToFile();
	void process(char* filename);
	state process(bool if_dynamic_frame_rate, double value,  double time_stamp);
	void  excelToVector(char* filename);
	void vectorToFile();
	void  addToArr(double a0,double a1);
	void pointsTofile(std::vector<point> &points, const int id);
	void logData();
	void setWindowSize(const bool init);
	double getTime();
	void writePartTofile();
	void reset(){frame_count=0;deleted_count=0;}
	~pointArray(void);
};

