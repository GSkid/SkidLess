#include<iostream>
#include<cstdio>
#include<vector>

int column_flag = 1;


int main(int argc, char **argv)
{
	FILE *out;
	
	if (argc < 2)
	{
		printf("No output file specified.\n");
		exit(1);
	}
	
	out = fopen(argv[1], "w");
	
	if (out == NULL)
	{
		printf("Unable to open or generate file.\n");
		exit(1);
	}
	
	// this is the imported data vector
	std::vector<int> data = {0, 1, 2, 3, 4, 5, 6};
	
	// prints out main column headers for the data file.
	// will need a conditional here: output if first loop, dont afterward
	if (column_flag == 1)
	{
		fprintf(out, "Soil Moisture:   Barametric Pressure:   Ambient Light:   Ambient Temperature:   Calculated Digital Output:   Time Stamp:   Node ID:   \n");                                                                                                                     
		column_flag = 0;
	}
	
	fprintf(out, "%d                ", data.at(0)); // prints out 0th member of the data vector to the file.
	fprintf(out, "%d                      ", data.at(1)); // prints out 1st member of the data vector to the file.
	fprintf(out, "%d                ", data.at(2)); // prints out 2nd member of the data vector to the file.
	fprintf(out, "%d                      ", data.at(3)); // prints out 3rd member of the data vector to the file.
	fprintf(out, "%d                            ", data.at(4)); // prints out 4th member of the data vector to the file.
	fprintf(out, "%d             ", data.at(5)); // prints out 5th member of the data vector to the file.
	fprintf(out, "%d \n", data.at(6)); // prints out 6th member of the data vector to the file.
	fclose(out);
	return 0;
	
}
