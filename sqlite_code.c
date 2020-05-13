
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

void insert_stuff(sqlite3 *mDb, int soil_moisture, int pressure, int light, int temp, int output, int timeStamp, int nodeID)
{
	char* errorMessage;
	sqlite3_exec(mDb, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
 
	char buffer[] = "INSERT INTO DATA (SOIL_MOISTURE,BARAMETRIC_PRESSURE,Ambient_Light,Ambient_Temp,Digital_Output,Time_Stamp,Node_ID) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(mDb, buffer, strlen(buffer), &stmt, NULL);
 
	char *id;
	//char id[] = "001";
	//sqlite3_bind_text(stmt, 1, id, strlen(id), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 1, soil_moisture);
	sqlite3_bind_double(stmt, 2, pressure);
	sqlite3_bind_double(stmt, 3, light);
	sqlite3_bind_double(stmt, 4, temp);
	sqlite3_bind_int(stmt, 5, output);
	sqlite3_bind_double(stmt, 6, timeStamp);
	sqlite3_bind_int(stmt, 7, nodeID);

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
	int result, soil_moisture, pressure, light, temp, output, timeStamp, nodeID;
	char a[10], b[10], c[10], d[10], e[10], f[10], g[10];
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
		result = sscanf(line, "%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^',']", a, b, c, d, e, f, g);
     	soil_moisture  = atoi(a);
		pressure  = atoi(b);
		light  = atoi(c);
		temp  = atoi(d);
		output  = atoi(e);
		timeStamp  = atoi(f);
		nodeID  = atoi(g);
     	//printf("%d\n %d\n %d\n %d\n %d\n %d\n %d\n", i, j, k, l, m, n, o);
     	insert_stuff(db, soil_moisture, pressure, light, temp, output, timeStamp, nodeID);
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
