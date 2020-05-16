
/*
pi@raspberry:~ $ sudo apt-get install sqlite3 // installs sqlite

pi@raspberry:~ $ sudo apt-get install libsqlite3-dev installs sqlite dev libraries

pi@raspberry:~ $ sqlite3 sensordata.db // creates database

compile cmd: gcc -o sqlite_code sqlite_code.c -lsqlite3 -std=c99

/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <string.h> 
#include <math.h>

#define CSVFILENAME "sensordata_example.csv"

void insert_stuff(sqlite3 *mDb, double soil_moisture, int light, int temp, double pressure, double precip_prob, int output, int nodeID, double battery_lvl, int hose1, int hose2, int hose3)
{
	char* errorMessage;
	sqlite3_exec(mDb, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
 
	char buffer[] = "INSERT INTO DATA (Soil_Moisture,Ambient_Light,Ambient_Temp,Barometric_Pressure,Precip_Prob,Digital_Output,Node_ID,Battery_Level,Hose_1,Hose_2,Hose_3) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(mDb, buffer, strlen(buffer), &stmt, NULL);
 
	char *id;
	//char id[] = "001";
	//sqlite3_bind_text(stmt, 1, id, strlen(id), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 1, soil_moisture);
	sqlite3_bind_int(stmt, 2, light);
	sqlite3_bind_int(stmt, 3, temp);
	sqlite3_bind_double(stmt, 4, pressure);
	sqlite3_bind_double(stmt, 5, precip_prob);
	sqlite3_bind_int(stmt, 6, output);
	sqlite3_bind_int(stmt, 7, nodeID);
	sqlite3_bind_double(stmt, 8, battery_lvl);
	sqlite3_bind_int(stmt, 9, hose1);
	sqlite3_bind_int(stmt, 10, hose2);
	sqlite3_bind_int(stmt, 11, hose3);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
    	printf("Commit Failed!\n");
		printf("Error: %s.\n",sqlite3_errmsg(mDb));
	}
	else
	{
		printf("Commit Successful.\n");
	}

	sqlite3_reset(stmt);
 
	sqlite3_exec(mDb, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
	if (errorMessage != NULL)
    {
        printf("Error: %s\n",errorMessage);
    }
	sqlite3_finalize(stmt);
}

  void processCSV(sqlite3 *db)
  {
	int result, light, temp, output, nodeID, hose1, hose2, hose3;
	double soil_moisture, pressure, precip_prob, battery_lvl;
	char a[10], b[10], c[10], d[10], e[10], f[10], g[10], h[10], i[10], j[10], k[10];
  	char line[256];
  	FILE *fp;
	fp = fopen(CSVFILENAME,"r");

    if(fp == NULL)
    {
		fprintf(stderr,"File not found.\n");
	}
    fgets(line,sizeof(line)-1,fp);
    while(fgets(line,sizeof(line)-1,fp) != NULL)
    {
		result = sscanf(line, "%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^',']", a, b, c, d, e, f, g, h, i, j, k);
     		soil_moisture  = atof(a);
		light  = atoi(b);
		temp  = atoi(c);
		pressure  = atof(d);
		precip_prob  = atof(e);
		output  = atoi(f);
		nodeID  = atoi(g);
	    	battery_lvl  = atof(h);
		hose1  = atoi(i);
		hose2  = atoi(j);
		hose3  = atoi(k);
     	//printf("%d\n %d\n %d\n %d\n %d\n %d\n %d\n", i, j, k, l, m, n, o);
     	insert_stuff(db, soil_moisture, light, temp, pressure, precip_prob, output, nodeID, battery_lvl, hose1, hose2, hose3));
	}

	fclose(fp);
  }


static int callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	int i;
	for(i = 0; i<argc; i++) 
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int createTable(sqlite3 *db)
{
	int rc;
	char *sql;
	char *zErrMsg = 0;
	/* Open database */
	//rc = sqlite3_open("sensordata.db", &db);
   
	/*if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	} else {
		fprintf(stdout, "Opened database successfully\n");
	}*/
	
	/* Create SQL statement */
	sql = "CREATE TABLE DATA(\n" 
	"ID INTEGER PRIMARY KEY AUTOINCREMENT,\n" 
	"SOIL_MOISTURE REAL,\n" 
	"BARAMETRIC_PRESSURE REAL,\n" 
	"AMBIENT_LIGHT REAL,\n" 
	"AMBIENT_TEMP REAL,\n" 
	"DIGITAL_OUTPUT INT,\n" 
	"TIME_STAMP REAL,\n" 
	"NODE_ID INT);"; 

	fprintf(stdout,"sql: %s \n",sql);

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	/* Close database */
	//rc = sqlite3_close(db);	   
	
	return 0;
}

int main(int argc, char* argv[]) 
{
	sqlite3 *db;
	int rc;

	// creates and opens database
	rc = sqlite3_open("sensordata.db", &db);
   
	if( rc ) 
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		//return(0);
	} 
	else 
	{
		fprintf(stdout, "Opened database successfully\n");
	}

	// creates the table in the database
	createTable(db);

	// takes in the data from the csv file (ignores the first line of headers), and places the data into the table
	// this should be in the embedded loop
	processCSV(db);

	/* Close database */
	rc = sqlite3_close(db);
}
