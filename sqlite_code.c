
/*
pi@raspberry:~ $ sudo apt-get install sqlite3 // installs sqlite packages

pi@raspberry:~ $ sudo apt-get installlibsqlite3-dev // installs sqlite dev packages

pi@raspberry:~ $ sqlite3 sensordata.db // creates database


compile cmd: gcc -o sqlite_code sqlite_code.c -lsqlite3 -std=c99

/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

printf("A \n");

   /* Open database */
   rc = sqlite3_open("sensordata.db", &db);
   
   printf("B \n");
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }

   printf("C \n");

   /* Create SQL statement */
	sql = "CREATE TABLE DATA(" \
	"ID INTEGER PRIMARY KEY," \
	"SOIL_MOISTURE REAL," \
	"BARAMETRIC_PRESSURE REAL," \
	"AMBIENT_LIGHT REAL," \
	"AMBIENT_TEMP REAL," \
	"DIGITAL_OUTPUT INTEGER," \
	"TIME_STAMP REAL," \
	"NODE_ID INTEGER);";

   printf("D \n");

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   printf("E \n");
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   
   printf("H \n");
      
   /* Create SQL statement */
   sql = ".mode csv";
   
   printf("I \n");
	
   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
   
   printf("K \n");
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }

   printf("L \n");
   
   /* Close database */
   rc = sqlite3_close(db);
   
   printf("M \n");
   
   int i = 0;
   
   printf("N \n");
	   
	for(i = 0; i < 3; i++)
	{
		/* Open database */
		rc = sqlite3_open("sensordata.db", &db);
		
		printf("O %d \n", i);
		
		/* Create SQL statement */
		sql = ".import sensordata.csv DATA";
		
		printf("P %d \n", i);
		
		/* Execute SQL statement */
		rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
		
		printf("Q %d \n", i);
		
		if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		}
		
		printf("R %d \n", i);

		/* Close database */
		rc = sqlite3_close(db);
	}
	
	printf("End of Program.");
}


