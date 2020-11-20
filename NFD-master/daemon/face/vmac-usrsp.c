#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <syslog.h>
#include <pthread.h>
#include <setjmp.h>
#include <sched.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include "vmac-usrsp.h"
#include "uthash.h"

void *recvvmac(void* tid)
{
	uint64_t  enc;
	uint16_t type;
	uint16_t seq,intnamelen;
	char *buffer;
	struct control rxc;
	struct hash *s;
	char *intname;
	int send=1;
	while(1)
	{
	send=1;
	recvmsg(sock_fd,&msg2,0);
	buffer=malloc(nlh2->nlmsg_len-100);
	memcpy(&rxc,NLMSG_DATA(nlh2),sizeof(struct control));
	memcpy(&buffer[0],NLMSG_DATA(nlh2)+sizeof(struct control),nlh2->nlmsg_len-100);
	type = (*(uint16_t*)(rxc.type));
	seq = (*(uint16_t*)(rxc.seq));
	enc = (*(uint64_t*)(rxc.enc));
	if(type==1||type==5)
	{
		HASH_FIND(hh,names,&enc,sizeof(uint64_t),s);
		if((type==5&&s==NULL)||s==NULL)send=0;
		else 
		{
			intname = malloc(strlen(s->name));
			memcpy(intname, s->name, strlen(s->name));
			intnamelen = strlen(s->name);
		}
		
	}
	else 
	{
		intname = NULL;
		intnamelen = 0;
		seq = 0;
	}
	if(send == 1)(*cb)(type, enc, buffer, nlh2->nlmsg_len-100, seq, intname, intnamelen);
	}
}
int vmac_register(void (*cf))
{
	msgy[0]='a';
	cb=cf;
	struct sched_param params;
	sock_fd=socket(PF_NETLINK,SOCK_RAW,VMAC_USER);
	size=strlen(msgy)+100;
	memset(&src_addr,0,sizeof(src_addr));
	src_addr.nl_family=AF_NETLINK;
	src_addr.nl_pid = getpid();
	bind(sock_fd,(struct sockaddr*)&src_addr, sizeof(src_addr));
	memset(&dest_addr,0,sizeof(dest_addr));
	dest_addr.nl_family= AF_NETLINK;
	dest_addr.nl_pid=0;
	dest_addr.nl_groups=0;
	nlh=(struct nlmsghdr*)malloc(MAX_PAYLOAD);
	nlh2=(struct nlmsghdr*)malloc(MAX_PAYLOAD);
	memset(nlh,0,MAX_PAYLOAD);
	memset(nlh2,0,MAX_PAYLOAD);
	nlh2->nlmsg_len=MAX_PAYLOAD;
	nlh->nlmsg_len=size;
	nlh->nlmsg_pid=getpid();
	nlh->nlmsg_flags=0;
	nlh->nlmsg_type=4;
	iov2.iov_base=(void*)nlh2;
	iov2.iov_len=nlh2->nlmsg_len;
	msg2.msg_name=(void*)&dest_addr;
	msg2.msg_namelen=sizeof(dest_addr);
	msg2.msg_iov=&iov2;
	msg2.msg_iovlen=1;
	iov.iov_base=(void*)nlh;
	iov.iov_len=nlh->nlmsg_len;
	msg.msg_name=(void*)&dest_addr;
	msg.msg_namelen=sizeof(dest_addr);
	msg.msg_iov=&iov;
	msg.msg_iovlen=1;
	params.sched_priority=sched_get_priority_max(SCHED_FIFO);
	pthread_setschedparam(thread, SCHED_FIFO, &params);
	pthread_create(&thread,NULL,recvvmac,(void*)0);
	nlh->nlmsg_type=255;
	memset(msgy,0,1024);
	digest64=0;
	memcpy(NLMSG_DATA(nlh),&digest64,8);
	memcpy(NLMSG_DATA(nlh)+8,msgy,strlen(msgy));
	size=strlen(msgy)+100;
	sendmsg(sock_fd,&msg,0);  
	enable_frame_adaptation();
	return 1;
}

void setfixed_rate(uint16_t rate)
{
	fixed_rate=rate;
}
void disable_frame_adaptation()
{
	fixed_rate=254;
}
void enable_frame_adaptation()
{
	fixed_rate=255;
}
void send_vmac(uint16_t type,uint16_t rate,uint16_t seq, char* buf, uint16_t len, char* InterestName, uint16_t name_len)
{
	struct control txc;
	struct hash *s;
	char* name;
	uint16_t ratesh;
	digest64=siphash24(InterestName, name_len, key);
	nlh->nlmsg_type=type;
	
	HASH_FIND(hh,names,&digest64,sizeof(uint64_t),s);
	if(s==NULL&&type==0)
	{
		name=malloc(name_len);
		memcpy(name,InterestName,name_len);
		s=(struct hash*)malloc(sizeof(struct hash));
		s->id=digest64;
		s->name=name;
		HASH_ADD(hh,names,id,sizeof(uint64_t),s);
	}
	//printf("Hashing %s with len =%d", InterestName,name_len);
	//printf("TYPE IS =%u",type);
	memcpy(&txc.enc[0],&digest64,8);
	memcpy(&txc.seq[0],&seq,2);
	if(fixed_rate==255)ratesh=255;
	else if(fixed_rate==254)ratesh=rate;
	else ratesh=fixed_rate;
	memcpy(&txc.rate[0],&ratesh,2);
	memcpy(&txc.type[0],&type,2);
	memcpy(NLMSG_DATA(nlh),&txc,sizeof(struct control));
	if(len!=0)memcpy(NLMSG_DATA(nlh)+sizeof(struct control),buf,len);
	iov.iov_len=nlh->nlmsg_len=len+100;
	sendmsg(sock_fd,&msg,0);
}
void add_name(char*InterestName, uint16_t name_len)
{
	struct hash *s;
	char *name;
	HASH_FIND(hh, names, &digest64, sizeof(uint64_t),s);
	digest64=siphash24(InterestName, name_len, key);
	HASH_FIND(hh, names, &digest64, sizeof(uint64_t),s);
	
	if(s == NULL)
	{
		name = malloc(name_len);
		memcpy(name, InterestName, name_len);
		s = (struct hash*)malloc(sizeof(struct hash));
		s->id = digest64;
		s->name = name;
		HASH_ADD(hh, names, id, sizeof(uint64_t),s);
	}
}

void del_name(char *InterestName, uint16_t name_len)
{
	struct hash *s;
	char *name;
	HASH_FIND(hh, names, &digest64, sizeof(uint64_t), s);
	digest64 = siphash24(InterestName, name_len, key);
	HASH_FIND(hh, names, &digest64, sizeof(uint64_t), s);
	if(s != NULL)
	{
		HASH_DEL(names, s);
		free(s);
	}
}
