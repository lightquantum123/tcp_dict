/*
 * transcode.c
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <inttypes.h>
#include "transcode.h"


unsigned short trans_decode(const char* buf, char *usrname
		,unsigned short * retval,unsigned int * len
		,unsigned char *data)
{
	if(buf == NULL)
		return -1;

	unsigned short cmd;
	int i = 0;
	const struct XProtocal *pxp;
	pxp = (struct XProtocal *)buf;
	cmd = ntohs(pxp->cmd_type);
	if(retval != NULL)
		*retval = ntohs(pxp->ret_val);
	if(len != NULL)
		*len = ntohl(pxp->data_len);
	if(usrname != NULL)
		strcpy(usrname,pxp->usrname);
	if(data != NULL)
	{
		for(i=0; i<*len;++i)
		{
			data[i] = pxp->data[i];
		}
	}
	return cmd;
}

char * trans_encode(char * usrname, unsigned short cmd
		, unsigned short retval
		, unsigned int len
		,unsigned char *data)
{
	int i = 0;
	static struct XProtocal buf;
	if(usrname != NULL)
	{
		strcpy(buf.usrname,usrname);
	}
	buf.cmd_type = htons(cmd);
	buf.ret_val = htons(retval);
	buf.data_len = htonl(len);
	if(data != NULL)
	{
		for(i=0; i<len;++i)
		{
			buf.data[i] = data[i] ;
		}
	}
	
	return (char *)&buf;
}

