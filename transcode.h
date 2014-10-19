#include <inttypes.h>

#define MAXLEN_USRNAME 8
#define MAXLEN_DATA 304
#define REQ_REGISTER (unsigned short)0x0001
#define RPL_REGISTER (unsigned short)0x8001
#define REQ_LOGIN (unsigned short)0x0002
#define RPL_LOGIN (unsigned short)0x8002
#define REQ_QUERYWORD (unsigned short)0x0003
#define RPL_QUERYWORD (unsigned short)0x8003
#define REQ_HISTORY (unsigned short)0x0004
#define RPL_HISTORY (unsigned short)0x8004
#define REQ_EXIT (unsigned short)0x0005

#define ERR_UNKNOWN (unsigned short)0x0001
#define ERR_USR_EXIST (unsigned short)0x0002
#define ERR_USR_NOEXIST (unsigned short)0x0003
#define ERR_USR_NA (unsigned short)0x0004
#define ERR_WORD_NA (unsigned short)0x0005
#define ERR_HISTORY_END (unsigned short)0x0006

struct XProtocal
{
	char usrname[MAXLEN_USRNAME];
	uint16_t cmd_type;
	uint16_t ret_val;
	uint32_t data_len;
	unsigned char data[MAXLEN_DATA];
};

unsigned short trans_decode(const char* buf, char *usrname 
		,unsigned short * retval,unsigned int * len
		,unsigned char *data);

char * trans_encode(char * usrname, unsigned short cmd 
		, unsigned short retval
		, unsigned int len
		,unsigned char *data);
