#ifndef ADSB_NETWORK_H
#define ADSB_NETWORK_H

/*===============================
These functions are responsible
for deal with the network functions,
which are used for the communication
with a remote server.
=================================*/

#include <curl/curl.h>

#define PUT_URL "www.radarlivre.com:80/api/collector/"
#define POST_URL    "www.radarlivre.com:80/api/adsb_info/"
#define COLLECTOR_URL   "collector_key/"

//60000
#define PUT_WAIT 120
#define POST_WAIT 500

#define BUFFER_SIZE 2048

typedef struct msg adsbMsg;

void CURL_init(CURL **handler);
void CURL_serialize(adsbMsg *msg, char **jsonMsg);
int CURL_transfer(CURL *handler, const char *url);
int CURL_put(CURL *handler, char *url, char *info);
int CURL_post(CURL *handler, char *url);

//Functions used inside a thread
void* NET_putMsg();
void* NET_dataUpload();
void* NET_postMsg();
void *NET_addBuffer(void *msg);
char *NET_readBuffer(char* finalJson);

//Semaphore
int SEM_init();
int SEM_destroy();

#endif