/*
 * dicclient.c
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "transcode.h"

static char usrname[MAXLEN_USRNAME];

void signup(int sockfd, char *cmd)
{
	char *p = cmd;
	char usr[MAXLEN_USRNAME];
	char passwd[MAXLEN_DATA];
	char netbuf[sizeof(struct XProtocal)];
	unsigned short retval = 0;
	int i = 0;

	p += 6;

	while(*p == ' ')
	{
		++p;
	}

	i = 0;
	while(*p != ' ')
	{
		usr[i] = *p;
		++i;
		++p;
	}
	usr[i] = '\0';

	while(*p == ' ')
	{
		++p;
	}
	
	strcpy(passwd, p);
	passwd[strlen(passwd)-1] = '\0';
	write(sockfd
		,trans_encode(usr
				,REQ_REGISTER
				,0
				,strlen(passwd)+1
				,passwd)
		,sizeof(struct XProtocal));
	
	read(sockfd, netbuf, sizeof(netbuf));

	if(trans_decode(netbuf
		,usrname
		,&retval, 0, NULL) == RPL_REGISTER)
	{
		switch(retval)
		{
			case ERR_USR_EXIST:
				printf("user already exist\n");
				break;
			case ERR_UNKNOWN:
				printf("server error\n");
				break;
			case 0:
				printf("sign up successfully\n");
				break;
		}
	}
}

void login(int sockfd, char *cmd)
{
	char *p = cmd;
	char usr[MAXLEN_USRNAME];
	char passwd[MAXLEN_DATA];
	char netbuf[sizeof(struct XProtocal)];
	unsigned short retval = 0;
	int i = 0;

	p += 5;

	while(*p == ' ')
	{
		++p;
	}

	i = 0;
	while(*p != ' ')
	{
		usr[i] = *p;
		++i;
		++p;
	}
	usr[i] = '\0';

	while(*p == ' ')
	{
		++p;
	}
	
	strcpy(passwd, p);
	passwd[strlen(passwd)-1] = '\0';
	write(sockfd
		,trans_encode(usr
				,REQ_LOGIN
				,0
				,strlen(passwd)+1
				,passwd)
		,sizeof(struct XProtocal));
	
	read(sockfd, netbuf, sizeof(netbuf));

	if(trans_decode(netbuf
		,usrname
		,&retval, 0, NULL) == RPL_LOGIN)
	{
		switch(retval)
		{
			case ERR_USR_NOEXIST:
				printf("invalid username "
					"or password\n");
				break;
			case ERR_UNKNOWN:
				printf("server error\n");
				break;
			case 0:
				printf("log in successfully\n");
				strcpy(usrname,usr);
				break;
		}
	}
}

void search(int sockfd, char *cmd)
{
	char *p = cmd;
	char word[32];
	char data[MAXLEN_DATA];
	char netbuf[sizeof(struct XProtocal)];
	unsigned short retval = 0;
	unsigned int len = 0;

	p += 6;
	while(*p == ' ')
	{
		++p;
	}
	strcpy(word,p);
	word[strlen(word)-1] = '\0';
	write(sockfd
		,trans_encode(usrname
				,REQ_QUERYWORD
				,0
				,strlen(word)+1
				,word)
		,sizeof(struct XProtocal));
	read(sockfd, netbuf, sizeof(netbuf));
	if(trans_decode(netbuf
		,usrname
		,&retval, &len, data) == RPL_QUERYWORD)
	{
		switch(retval)
		{
			case ERR_USR_NA:
				printf("illegal user, "
					"please log in again\n");
				break;
			case ERR_UNKNOWN:
				printf("server error\n");
				break;
			case ERR_WORD_NA:
				printf("cannot found\n");
				break;
			case 0:
				printf("%s\n",data);
				break;
		}
	}
}

void history(int sockfd, char *cmd)
{	
	char data[MAXLEN_DATA];
	char netbuf[sizeof(struct XProtocal)];
	unsigned short retval = 0;
	unsigned int len = 0;
	int fetching = 1;
	write(sockfd
		,trans_encode(usrname
				,REQ_HISTORY
				,0
				,0
				,NULL)
		,sizeof(struct XProtocal));
	while(fetching)
	{
		read(sockfd, netbuf, sizeof(netbuf));
		if(trans_decode(netbuf
			,usrname
			,&retval, &len, data) == RPL_HISTORY)
		{
			switch(retval)
			{
				case ERR_USR_NA:
					printf("illegal user, "
					"please log in again\n");
					fetching = 0;
					break;
				case ERR_UNKNOWN:
					printf("server error\n");
					fetching = 0;
					break;
				case 0:
					printf("%s\n",data);
					break;
				case ERR_HISTORY_END:
					fetching = 0;
					break;
			}
		}else
			fetching = 0;
	}
}

void logout(int sockfd, char *cmd)
{
	write(sockfd
		,trans_encode(usrname
				,REQ_EXIT
				,0
				,0
				,NULL)
		,sizeof(struct XProtocal));
	
	usrname[0] = '\0';
}

void help()
{
	printf("legal commands are:\n");
	printf("	signup\n");
	printf("	signin\n");
	printf("	search\n");
	printf("	history\n");
	printf("	logout\n");
}

int main(int argc, char * argv[])
{
	int sockfd;
	char buf[256];
	char nbuf[256];

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	socklen_t serverlen = sizeof(serveraddr);
	int recvsize = 0;
	char *p = NULL;
	usrname[0] = '\0';

	do
	{
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("socket");
			exit(EXIT_FAILURE);
		}
		if(connect(sockfd,(struct sockaddr *)&serveraddr
			, serverlen) < 0)
		{
			perror("connect");
			exit(EXIT_FAILURE);
		}
		p = fgets(buf, sizeof(buf), stdin);

		if(strncmp(buf,"search",6) == 0)
		{
			search(sockfd, buf);
		}else if(strncmp(buf,"history",7) == 0)
		{
			history(sockfd, buf);
		}else if(strncmp(buf,"login",5) == 0)
		{
			login(sockfd, buf);
		}else if(strncmp(buf,"signup",6) == 0)
		{
			signup(sockfd, buf);
		}else if(strncmp(buf,"logout",6) == 0)
		{
			logout(sockfd, buf);
		}else if(strncmp(buf,"help",4) == 0)
		{
			help();
		}else if(strncmp(buf,"quit",4) == 0)
		{
			logout(sockfd, buf);
		}else
		{
			printf("illegal command, please read the "
					"help for the right "
					"command\n");
		}

		close(sockfd);
	
	}while(strcmp(buf, "quit\n"));
	return 0;
}
