#ifndef ADSB_SYSTEMSTATS_H
#define ADSB_SYSTEMSTATS_H

/*===============================
These functions are used to read
cpu and memory stats.
=================================*/
//0: Collector C | 1: Collector Python
#define COLLECTOR 0
//0: Orange Pi | 1: Computador pessoal
#define HARDWARE_C 1
#define CPU_FILE    "stats_dir/cpu_usage_c.csv"
#define MEM_FILE    "stats_dir/mem_usage_c.csv"
#define DECTIME_FILE    "stats_dir/decoding_time_c.csv"
#define ALL_MSG_FILE    "stats_dir/all_received_msgs_c.csv"
#define ADSB_MSG_FILE   "stats_dir/adsb_received_msgs_c.csv"
#define DECODED_MSG_FILE    "stats_dir/decoded_msgs_c.csv"
#define NOT_DECODED_ADSB_MSG_FILE    "stats_dir/not_decoded_adsb_msgs_c.csv"
#define STATS_TIMEOUT   5

float CPU_usage();
float MEM_usage();
void* saveSystemStats();
void saveDecodingTime(double decTime);
void saveReceivedMessage(char *msg, char *path);

#endif