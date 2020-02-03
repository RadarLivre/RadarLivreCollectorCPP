#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "adsb_auxiliars.h"
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_decoding.h"
#include "adsb_serial.h"
#include "adsb_db.h"
#include "adsb_createLog.h"
#include "adsb_network.h"
#include "adsb_systemStats.h"

adsbMsg *messagesList = NULL;

volatile int timer_flag = 0;

static void timerHandler(int sig, siginfo_t *si, void *uc){
    timer_flag = 1;
}

void suddenStop(){

 	printf("\nCtrl+C...\nClosing Collector...\n");

 	LIST_removeAll(&messagesList);
 
    printf("Collector closed!\n");
    LOG_add("suddenStop", "Collector was closed");

 	exit(1);
}

int main(){
    int serialPort = 0, syndrome = 0;
    char buffer[29]; buffer[0] = '\0';
    adsbMsg *node = NULL;
    adsbMsg *nodePost = NULL;

    signal(2, suddenStop);

    //Starting Serial
    serialPort = SERIAL_start();

    //Starting Timer
    TIMER_setSignalHandler(timerHandler, SIG);
    timer_t timerid = TIMER_create(CLOCKID, SIG);
    TIMER_setTimeout(TIMEOUT, timerid);

    //Initializing semaphore
    SEM_init();

    pthread_t thread;
    //pthread_t thread_stats; (monography tests)

    //To count the time spent to decode and save the messages 
    //clock_t startStat, endStat; (monography tests)

    //This thread sends a hello to the server and sends the messages of the list
    int sendHello = pthread_create(&thread, NULL, NET_dataUpload, NULL);					//Cria uma thread responsável apenas por mandar um Hello do coletor para o servidor, a cada 1 min.
    if (sendHello){
	 	//printf("ERROR; return code from pthread_create() is %d\n", sendHello);
        LOG_add("adsb_collector","Send hello thread could not be created");
 		exit(-1);
 	}

    //(monography tests)
    /*int readStats = pthread_create(&thread_stats, NULL, saveSystemStats, NULL);					//Cria uma thread responsável apenas por mandar um Hello do coletor para o servidor, a cada 1 min.
    if (readStats){
	 	//printf("ERROR; return code from pthread_create() is %d\n", readStats);
        LOG_add("adsb_collector","read Stats thread could not be created");
 		exit(-1);
 	}*/

    while(1){   //Polling method

        SERIAL_communicate(&serialPort, buffer);

        //This is used because the timer interrupt makes the buffer empty,
        //and we don't want to proccess an empty buffer.
        if(strlen(buffer) == 0){
            continue;
        }

        //saves all the messages received 
        //saveReceivedMessage(buffer, ALL_MSG_FILE); (monography tests)

        //If CRC returns 1, the message is correct. Otherwise, we don't do anything with the message.
        if(CRC_tryMsg(buffer, &syndrome)){

            //startStat = clock(); (monography tests)

            messagesList = decodeMessage(buffer, messagesList, &node);

            if(isNodeComplete(node) != NULL){
                if(DB_saveData(node) != 0){
                    //()printf("The aircraft information couldn't be saved!\n");
                }else{
                    //()printf("Aircraft %s information saved succesfully!\n", node->ICAO);

                    if(DB_readData(&nodePost) != 0){
                        //printf("We coudn't read the aircraft information\n");
                        LOG_add("adsb_collector","We coudn't read the aircraft information");
                    }else{
                   
                        NET_addBuffer((void *)nodePost);

                        clearMinimalInfo(node);
                    }
                    
                }
            }else{
                //()printf("Information is not complete!\n");
            }

            //(monography tests)
            /*endStat = clock();
            saveDecodingTime(((double)(endStat - startStat))/CLOCKS_PER_SEC);*/
        }
        
        node = NULL;
        nodePost = NULL;
		memset(buffer, 0x0, 29);

        // //It cleans the old nodes in the messages list
        if(timer_flag){
            messagesList = LIST_delOldNodes(messagesList);
            timer_flag = 0;
        }

        usleep(1000); //Wait 1 ms
    }
    return 0;
}