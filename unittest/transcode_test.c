#include <stdio.h>
#include "../transcode.h"

int main()
{
	char *p;
	char name[MAXLEN_USRNAME];
	char data[MAXLEN_DATA];
	unsigned short retval;
	unsigned int len;
	p = trans_encode("aa",RPL_REGISTER,0,8,"abcdefg");
	if(trans_decode(p,name,&retval,&len,data) == RPL_REGISTER)
	{
		printf("%s: %d  %s\n",name,retval,data);
	}
}
