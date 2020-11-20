#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <syslog.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include "uthash.h"
//typedef void (*cb)(uint8_t type, uint64_t enc, unsigned char *data, uint16_t data_len, uint16_t seq);
#define VMAC_USER 31
#define MAX_PAYLOAD 2000

/**
 ** ABI Be careful when changing to adjust userspace information as well.
**/
struct control{
	char type[2];
	char rate[2];
	char enc[8];
	char seq[2];
};
struct hash{
	uint64_t id;
	char *name;
	UT_hash_handle hh;
};
struct hash* names=NULL;
struct sockaddr_nl src_addr,dest_addr;
struct nlmsghdr *nlh=NULL;
struct nlmsghdr *nlh2=NULL;
struct iovec iov;
struct iovec iov2;
struct msghdr msg;
struct msghdr msg2;
uint64_t digest64; 
uint16_t fixed_rate;
void (*cb)();
char msgy[2000];
int multiple=0;
int num,size;
int sock_fd;
pthread_t thread;
char key[16] = {0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf};
uint64_t siphash24(const char *in, unsigned long inlen, const char k[16]);
volatile int running=0;
volatile int total;
void enable_frame_adaptation();
void disable_frame_adaptation();
void setfixed_rate(uint16_t rate);
void send_vmac(uint16_t type,uint16_t rate,uint16_t seq, char* buf, uint16_t len, char* InterestName, uint16_t name_len);
void add_name(char*InterestName, uint16_t name_len);
void del_name(char *InterestName, uint16_t name_len);
