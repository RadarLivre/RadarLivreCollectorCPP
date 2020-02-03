#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_db.h"
#include "adsb_createLog.h"

/*==============================================
FUNCTION: DB_open
INPUT: a char vector
OUTPUT: a pointer for a sqlite3 structure
DESCRIPTION: this function receives a database name
and tries to open the database with that name. In
success, the function returns the database handler
pointer. Otherwise, it returns NULL.
================================================*/
sqlite3 * DB_open(char *db_name){
    sqlite3 *db_handler = NULL;
    int status = -1;
    
    status = sqlite3_open((const char *)db_name, &db_handler);
    
    if(status == SQLITE_OK){
        //()printf("Database successfully opened!\n");
        return db_handler;
    }else{
        //()printf("Database not opened: %s\n", sqlite3_errmsg(db_handler));
        LOG_add("DB_open", "database couldn't be opened!");

        return NULL;
    }
}

/*==============================================
FUNCTION: DB_close
INPUT: a sqlite3 pointer and two char vectors
OUTPUT: void
DESCRIPTION: this function receives the database
handler and attempts to close the database.
In addition, it frees both char vectors: error
message and sql query.
================================================*/
void DB_close(sqlite3 **db_handler, char**errmsg, char**sqlText){

    if(sqlite3_close_v2(*db_handler) != SQLITE_OK){
        //()printf("It couldn't close the database!\n");
        LOG_add("DB_saveADSBInfo", "Database couldn't be closed");
    }else{
        //()printf("Database successfully closed!\n");
    }

    sqlite3_free(*errmsg);
    *errmsg = NULL;
    sqlite3_free(*sqlText);
    *sqlText = NULL;
    *db_handler = NULL;
}

/*==============================================
FUNCTION: DB_saveADSBInfo
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to save its information into the radar_
livre_adsbinfo table. In success, the status returned
contains 0 (SQLITE_OK). Otherwise, contains the value
of the constant that represents the error.
================================================*/
int DB_saveADSBInfo(adsbMsg *msg){
    int status = -1;    //-1 we need assuming that nothings was saved.
    char *sqlText = NULL, *errmsg = NULL;
    sqlite3 *db_handler = NULL;
    
    //It's recommended to call this function prior to any other from sqlite (https://www.sqlite.org/c3ref/initialize.html)
    if(sqlite3_initialize() != SQLITE_OK){
        //()printf("It wasn't possible to initialize SQLITE!\n");
        LOG_add("DB_saveADSBInfo", "It wasn't possible to initialize SQLITE");

        return DATABASE_ERROR;
    }

    db_handler = DB_open(DATABASE);
    
    if(db_handler == NULL){
        sqlite3_shutdown();
        return DATABASE_ERROR;  //database couldn't be opened.
    }

    sqlText = sqlite3_mprintf(
        "INSERT INTO radarlivre_api_adsbinfo(collectorKey,modeSCode,callsign,latitude,longitude," \
        "altitude,verticalVelocity,horizontalVelocity,groundTrackHeading,timestamp,timestampSent," \
        "messageDataId,messageDataPositionEven,messageDataPositionOdd,messageDataVelocity)"\
        "VALUES(\"%q\", \"%q\", \"%q\", %f, %f, %d, %d, %f, %f, %lf, %lf, \"%q\", \"%q\", \"%q\", \"%q\");", 
        msg->COLLECTOR_ID, msg->ICAO, msg->callsign, msg->Latitude, msg->Longitude, msg->Altitude,
        msg->verticalVelocity, msg->horizontalVelocity, msg->groundTrackHeading, getCurrentTime(), 
        getCurrentTime(), msg->messageID, msg->oeMSG[0], msg->oeMSG[1], msg->messageVEL
    );
    
    if(sqlText == NULL){
        //()printf("SQLITE query couldn't be created!\n");
        LOG_add("DB_saveADSBInfo", "SQLITE query couldn't be create");
        DB_close(&db_handler, &errmsg, &sqlText);
        sqlite3_shutdown();

        return  DATABASE_ERROR;
    }

    status = sqlite3_exec(db_handler, sqlText, NULL, NULL, &errmsg); //It tries to execute the sql statement
    
    if(status ==  SQLITE_OK){
        //printf("Data was saved successfully!\n");
        LOG_add("DB_saveADSBInfo", "Data was saved successfully");
    }else{
        //()printf("Data couldn't be saved: %s\n", errmsg);
        LOG_add("DB_saveADSBInfo", "Data couldn't be saved");
    }
 
    DB_close(&db_handler, &errmsg, &sqlText);
    sqlite3_shutdown(); //It's recommended to call this function at the end of a sqlite process

    return status;
}

/*==============================================
FUNCTION: DB_saveAirline
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to save its information into the radar_
livre_airline table. In success, the status returned
contains 0 (SQLITE_OK). Otherwise, contains the value
of the constant that represents the error.
================================================*/
int DB_saveAirline(adsbMsg *msg){
    int status = -1; // we need assuming that nothings was saved.
    char *sqlText = NULL, *errmsg = NULL;
    sqlite3 *db_handler = NULL;

    //It's recommended to call this function prior to any other from sqlite (https://www.sqlite.org/c3ref/initialize.html)
    if(sqlite3_initialize() != SQLITE_OK){
        //()printf("It wasn't possible to initialize SQLITE!\n");
        LOG_add("DB_saveAirline", "It wasn't possible initialize SQLITE");
        return DATABASE_ERROR;
    }

    db_handler = DB_open(DATABASE);

    if(db_handler == NULL){
        sqlite3_shutdown();
        return DATABASE_ERROR;  //database couldn't be opened.
    }

    sqlText = sqlite3_mprintf(
        "INSERT INTO radarlivre_api_airline(icao, callsign) VALUES (\"%q\", \"%q\");", 
        msg->ICAO, msg->callsign
    );

    if(sqlText == NULL){
        //printf("SQLITE query couldn't be created!\n");
        LOG_add("DB_saveAirline", "SQLITE query couldn't be create");
        DB_close(&db_handler, &errmsg, &sqlText);
        sqlite3_shutdown();

        return  DATABASE_ERROR;
    }
    status = sqlite3_exec(db_handler, sqlText, NULL, NULL, &errmsg); //It tries to execute the sql statement

    if(status ==  SQLITE_OK){
        //printf("Data was saved successfully!\n");
        LOG_add("DB_saveAirline", "Data was saved successfully");
    }else{
        //printf("Data couldn't be saved: %s\n", errmsg);
        LOG_add("DB_saveAirline", "Data couldn't be saved");
    }

    DB_close(&db_handler, &errmsg, &sqlText);
    sqlite3_shutdown(); //It's recommended to call this function at the end of a sqlite process

    return status;
}

/*==============================================
FUNCTION: DB_saveData
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to save its information into the radar_
livre_adsbinfo and radarlivre_api_airline tables.
In success, the status returned contains 0 (SQLITE_OK).
Otherwise, contains the value of the constant that
represents the error.
================================================*/
int DB_saveData(adsbMsg *msg){
    int status1 = -1, status2 = -1, tries = 3;

    while((status1 != 0) && (tries > 0)){
         status1 = DB_saveADSBInfo(msg);
         tries--;
    }

    // tries = 3;
    // while((status2 != 0) && (tries > 0)){
    //      status2 = DB_saveAirline(msg);
    //      tries--;
    // }

    // if(status1 != 0){
    //     return status1;
    // }
    // if(status2 != 0){
    //     return status2;
    // }

    return status1; //It's because status1 and status2 are equal to zero
}

/*==============================================
FUNCTION: DB_callback
INPUT: a void pointer, an integer and two char 
pointer to pointer
OUTPUT: an integer
DESCRIPTION: this function has the structure shown 
below. If executed within a sql_exec(), the callback 
function will be called for each record (line) returned
by the select statement. 
[https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm]


typedef int (*sqlite3_callback)(
   void*,     Data provided in the 4th argument of sqlite3_exec() 
   int,       The number of columns in row 
   char**,    An array of strings representing fields in the row 
   char**     An array of strings representing column names 
);

================================================*/
static int DB_callback(void *data, int nCols, char** fields, char** colNames){
    //Initializing a new adsMsg node to save the information
    adsbMsg** node = (adsbMsg **)data;
    // *node = (adsbMsg*)realloc(*node, sizeof(adsbMsg));
    *node = (adsbMsg*)malloc(sizeof(adsbMsg));
	
    (*node)->COLLECTOR_ID[0] = '\0';
	(*node)->ICAO[0] = '\0';
	(*node)->callsign[0] = '\0';
	(*node)->oeTimestamp[0] = 0;
	(*node)->oeTimestamp[1] = 0;
	(*node)->lastTime = 0;
	(*node)->uptadeTime = getCurrentTime();
	(*node)->Latitude = 0;
	(*node)->Longitude = 0;
	(*node)->Altitude = 0;
	(*node)->horizontalVelocity = 0;
	(*node)->verticalVelocity = 0;
	(*node)->groundTrackHeading = 0;
    (*node)->messageID[0] = '\0';
    (*node)->oeMSG[0][0] = '\0';
	(*node)->oeMSG[1][0] = '\0';
	(*node)->messageVEL[0] = '\0';

    strcpy((*node)->COLLECTOR_ID, fields[1]);
    (*node)->COLLECTOR_ID[strlen(fields[1])] = '\0';
    strcpy((*node)->ICAO, fields[2]);
    (*node)->ICAO[strlen(fields[2])] = '\0';
    strcpy((*node)->callsign, fields[3]);
    (*node)->callsign[strlen(fields[3])] = '\0';

    (*node)->Latitude = atof(fields[4]);
    (*node)->Longitude = atof(fields[5]);
    (*node)->Altitude = atoi(fields[6]);
    (*node)->verticalVelocity = atoi(fields[7]);
    (*node)->horizontalVelocity = atof(fields[8]);
    (*node)->groundTrackHeading = atof(fields[9]);

    (*node)->oeTimestamp[0] = atof(fields[10]);
    (*node)->oeTimestamp[1] = atof(fields[11]);

    strcpy((*node)->messageID, fields[12]);
    (*node)->messageID[strlen(fields[12])] = '\0';
    strcpy((*node)->oeMSG[0], fields[13]);
    (*node)->oeMSG[0][strlen(fields[13])] = '\0';
    strcpy((*node)->oeMSG[1], fields[14]);
    (*node)->oeMSG[1][strlen(fields[14])] = '\0';
    strcpy((*node)->messageVEL, fields[15]);
    (*node)->messageVEL[strlen(fields[15])] = '\0';

    (*node)->next = NULL;

   return 0;
}


/*==============================================
FUNCTION: DB_readData
INPUT: an adsbMsg pointer
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to read the last message saved in the database
into this node.
================================================*/
int DB_readData(adsbMsg **node){
    int status = -1; 
    char *sqlText = NULL, *errmsg = NULL;
    sqlite3 *db_handler = NULL;

    //It's recommended to call this function prior to any other from sqlite (https://www.sqlite.org/c3ref/initialize.html)
    if(sqlite3_initialize() != SQLITE_OK){
        //()printf("It wasn't possible to initialize SQLITE!\n");
        LOG_add("DB_readData", "It wasn't possible to initialize SQLITE");
        return DATABASE_ERROR;
    }

    db_handler = DB_open(DATABASE);

    if(db_handler == NULL){
        sqlite3_shutdown();
        return DATABASE_ERROR;  //database couldn't be opened.
    }

    sqlText = sqlite3_mprintf("SELECT * from radarlivre_api_adsbinfo ORDER BY id DESC LIMIT 1");

    status = sqlite3_exec(db_handler, sqlText, DB_callback, node, &errmsg); //It tries to execute the sql statement
    
    if(status ==  SQLITE_OK){
        //printf("Data was read successfully!\n");
        LOG_add("DB_readData", "Data was read successfully");
    }else{
        //printf("Data couldn't be read: %s\n", errmsg);
        LOG_add("DB_readData", "Data couldn't be read");
    }

    DB_close(&db_handler, &errmsg, &sqlText);
    sqlite3_shutdown(); //It's recommended to call this function at the end of a sqlite process

    return status;
}