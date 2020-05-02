// #Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>


struct Forecast{
	int precipProb;
	int temperature;
	int humidity;
	int pressure;
	int windSpeed;
	int windBearing;
};


int main(int argc, char *argv[])
{
	struct Forecast Forecast1;
	char buffer[10];
	double data[6];
	
	FILE *fp;
	
	/* 	calls python-wrapped shell script.
		In this example, "python RFpython_test.py" is the command used in the shell.
	 	The RFpython_test.py file was in the current directory */
	fp = popen("python RFpython_test.py","r");
	
	// error checking
	if(fp == NULL)
	{
		printf("Failed to run command.\n");
		exit(1);
	}
	
	int tmp = 0;
	
	// loop that extracts the outputted data from the shell and places it in an array
	while(fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		sscanf(buffer, "%lf", &data[tmp]);
		++tmp;
	}
	
	// moves the extracted data from the array to the struct
	Forecast1.precipProb = round(data[0]);
	printf("Forecast1.precipProb = %d.\n", Forecast1.precipProb);
	Forecast1.temperature = round(data[1]);
	printf("Forecast1.temperature = %d.\n", Forecast1.temperature);
	Forecast1.humidity = round(data[2]);
	printf("Forecast1.humidity = %d.\n", Forecast1.humidity);
	Forecast1.pressure = round(data[3]);
	printf("Forecast1.pressure = %d.\n", Forecast1.pressure);
	Forecast1.windSpeed = round(data[4]);
	printf("Forecast1.windSpeed = %d.\n", Forecast1.windSpeed);
	Forecast1.windBearing = round(data[5]);
	printf("Forecast1.windBearing = %d.\n", Forecast1.windBearing);
	
	
	pclose(fp);
	
	return 0;
}





