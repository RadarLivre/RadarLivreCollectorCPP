#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include "adsb_systemStats.h"
#include "adsb_time.h"

//Monography tests functions

/*==============================================
FUNCTION: CPU_usage
INPUT: void
OUTPUT: it returns a float
DESCRIPTION: this function returns the use of the
cpu since the boot time, read from the /proc/stat
file. To get the percentage, the returned number
needs to be multiplied by 100.
================================================*/
float CPU_usage(){
    int total = 0, idle = 0, cpu = 0, i = 0;
    float pidle = 0;
    char line[105];
    
    FILE *p = fopen("/proc/stat","r");
    fscanf(p, "%[^\n]", line);
    
    strtok(line, " ");
    for(i = 0; i < 7; i ++){
        if(i == 3){
            idle = atoi(strtok(NULL, " "));
            total+=idle;
            continue;
        }
        
        total += atoi(strtok(NULL, " "));
    }

    fclose(p);
    pidle = (float)idle/total;

    return 1.0 - pidle;
}

/*==============================================
FUNCTION: MEM_usage
INPUT: void
OUTPUT: it returns a float
DESCRIPTION: this function returns the use of memory,
which is read from the /proc/meminfo file. To get the
percentage, the returned number needs to be multiplied
by 100.
================================================*/
float MEM_usage(){
    int total = 0, mfree = 0, mbuf = 0, mcache = 0, mused = 0, i = 0;
    size_t  len = 0;
    float pidle = 0;
    char *line;
    
    FILE *p = fopen("/proc/meminfo","r");
    
    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        total = atoi(strtok(NULL, " "));
        //printf("Total: %d\n", total);
    }

    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        mfree = atoi(strtok(NULL, " "));
        //printf("MemFree: %d\n", mfree);
    }

    getline(&line,&len,p); //This line must be commented in the orange pi because meminfo is different.
    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        mbuf = atoi(strtok(NULL, " "));
        //printf("Buffers: %d\n", mbuf);
    }

    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        mcache = atoi(strtok(NULL, " "));
        //printf("Caches: %d\n", mcache);
    }

    fclose(p);
    return total - (mfree + mbuf + mcache); //KB
}

/*==============================================
FUNCTION: saveSystemStats
INPUT: void
OUTPUT: void
DESCRIPTION: this functions saves the cpu and memory
usages in two different files, every n seconds.
================================================*/
void* saveSystemStats(){

    while(1){
        FILE *fcpu = fopen(CPU_FILE, "a");
        if(fcpu == NULL){
            printf("It was not possible to open the cpu stats file!\n");
            continue;
        }
        FILE *fmem = fopen(MEM_FILE,"a");
        if(fmem == NULL){
            printf("It was not possible to open the mem stats file!\n");
            continue;
        }

        long long int now = getCurrentTimeMilli();
        fprintf(fcpu,"%d,%d,%lf,%lld\n",COLLECTOR,HARDWARE_C,CPU_usage(),now);
        fprintf(fmem,"%d,%d,%lf,%lld\n",COLLECTOR,HARDWARE_C,MEM_usage(),now);

        fclose(fcpu);
        fclose(fmem);

        sleep(STATS_TIMEOUT);
    }
}

/*==============================================
FUNCTION: saveDecodingTime
INPUT: a double value
OUTPUT: void
DESCRIPTION: this functions receives the time spent
to decode the message, verify whether it is complete, 
save in the database, read from the database and 
save in the list to be send; and saves it in a file.
================================================*/
void saveDecodingTime(double decTime){
    FILE *fdec = fopen(DECTIME_FILE, "a");
    if(fdec == NULL){
        printf("It was not possible to open the decoding time file!\n");
        return;
    }

    fprintf(fdec,"%d,%d,%lf,%lld\n", COLLECTOR, HARDWARE_C, decTime,getCurrentTimeMilli());

    fclose(fdec);
}

/*==============================================
FUNCTION: saveReceivedMessage
INPUT: a vector of chars
OUTPUT: void
DESCRIPTION: this functions receives a message 
and saves it in a file.
================================================*/
void saveReceivedMessage(char *msg, char *path){
    FILE *fmsg = fopen(path, "a");
     if(fmsg == NULL){
        printf("It was not possible to open the received messages file!\n");
        return;
    }

    fprintf(fmsg,"%d,%d,%s,%lld\n", COLLECTOR, HARDWARE_C, msg,getCurrentTimeMilli());
    fclose(fmsg);
}