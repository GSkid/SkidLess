// #Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

// #Defines


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
	//char something[6][10];
	double data[6];
	
	FILE *fp;
	
	fp = popen("python RFpython_test.py","r");
	if(fp == NULL)
	{
		printf("Failed to run command.\n");
		exit(1);
	}
	
	int tmp = 0;
	while(fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		sscanf(buffer, "%lf", &data[tmp]);
		//fprintf(stdout, "Fin: %lf", data[tmp]);
		++tmp;
	}
	
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





