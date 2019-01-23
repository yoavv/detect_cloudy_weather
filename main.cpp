
#include "globals.h"
#include "point.h"
#include "pointArray.h"
#include <ctime>
using namespace std;
std::map<std::string, double>  getGlobals()
{
	string str;
	std::ifstream file("globals.xml");
	std::map<std::string, double> globals;
	for (int i = 0; file.good(); i++)
	{
		getline(file, str);


		unsigned first = str.find(">");
		unsigned last = str.find("</key>");
		string var = str.substr(first+1, last - first-1);


		getline(file, str);
		unsigned firsts = str.find(">");
		unsigned lasts = str.find("</value>");
		string val = str.substr(firsts+1, lasts - firsts-1);

		double value=atof(val.c_str());
		globals[var]=value;
		
	}
	return globals;
}
double point ::frame_rate=7;
int point :: clouds_count=0;
//int point :: window_size=0;
int pointArray ::frame_count=0;
int pointArray ::deleted_count=0;
std::map<std::string, double> point ::  globals=getGlobals();

void main()
	
{

	char * filename="C:\\cam8_out.csv";	
	pointArray p1= pointArray();
	p1.process(filename);
	/*
	for(int i=0;i<100000;i++)
		{

			
			p1.process(25);
			
		}
		*/
	//p1.logData();

}