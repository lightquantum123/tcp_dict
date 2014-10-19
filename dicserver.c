#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include "db.h"
#include "transcode.h"
#include "dic.h"

void ser_register(int confd, char *usrname, unsigned int len 
		, unsigned char * data)
{
	switch(db_signup(usrname, data))
	{
		case 0:
			write(confd
				,trans_encode(usrname
						,RPL_REGISTER
						,0
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		case 1:
			write(confd
				,trans_encode(usrname
						,RPL_REGISTER
						,ERR_USR_EXIST
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		case -1:
			write(confd
				,trans_encode(usrname
						,RPL_REGISTER
						,ERR_UNKNOWN
						,0
						,NULL)
				,sizeof(struct XProtocal));

			break;
	}
	return;
}

void ser_login(int confd, char *usrname, unsigned int len 
		, unsigned char * data)
{
	switch(db_login(usrname, data))
	{
		case 0:
			write(confd
				,trans_encode(usrname
						,RPL_LOGIN
						,0
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		case 1:
			write(confd
				,trans_encode(usrname
						,RPL_LOGIN
						,ERR_USR_NOEXIST
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		case -1:
			write(confd
				,trans_encode(usrname
						,RPL_LOGIN
						,ERR_UNKNOWN
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
	}
	return;
}

void ser_queryword(int confd, char *usrname, unsigned int len
		,unsigned char * data)
{
	char expbuf[MAXLEN_DATA];
	switch(db_islogin(usrname))
	{
		case 0:
		//no active user
			write(confd
				,trans_encode(usrname
						,RPL_QUERYWORD
						,ERR_USR_NA
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		case 1:
		//user is active 
			db_history_put(usrname,data);
			if(dic_getexp(data, expbuf) == 0)
			{
				write(confd
					,trans_encode(usrname
							,RPL_QUERYWORD
							,0
						       ,strlen(expbuf)+1
							,expbuf)
					,sizeof(struct XProtocal));
			}else
			{
				write(confd
					,trans_encode(usrname
							,RPL_QUERYWORD
							,ERR_WORD_NA
							,0
							,NULL)
					,sizeof(struct XProtocal));

			}
			break;
		default:
			write(confd
				,trans_encode(usrname
						,RPL_QUERYWORD
						,ERR_UNKNOWN
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
	}
	return;
}

struct callback_par
{
	int fd;
	char * un;
};

int ser_history_method(
	void *arg, 
	int n_column, 
	char **column_value, 
	char **column_name
	)
{
	
	struct callback_par* pfd = (struct callback_par*)arg;
	int confd = pfd->fd;
	char *usrname = pfd->un;
	char data[MAXLEN_DATA];
	sprintf(data,"%s%s",column_value[1], column_value[2]);
	write(confd
		,trans_encode(usrname
				,RPL_HISTORY
				,0
				,strlen(data)+1
				,data)
		,sizeof(struct XProtocal));
	return 0;
}

void ser_history(int confd, char *usrname, unsigned int len
		, unsigned char * data)
{
	struct callback_par parameter;
	switch(db_islogin(usrname))
	{
		case 0:
		//no active user
			write(confd
				,trans_encode(usrname
						,RPL_HISTORY
						,ERR_USR_NA
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		case 1:
		//user is active 
			parameter.fd = confd;
			parameter.un = usrname;
			db_history_get(usrname, ser_history_method, &parameter);
			write(confd
				,trans_encode(usrname
						,RPL_HISTORY
						,ERR_HISTORY_END
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
		default:
			write(confd
				,trans_encode(usrname
						,RPL_HISTORY
						,ERR_UNKNOWN
						,0
						,NULL)
				,sizeof(struct XProtocal));
			break;
	}
	return;

}

void reaper(int signo)
{
	while(waitpid(-1,NULL,WNOHANG) >0);
}

int main(int argc, char * argv[])
{
	int listenfd;
	char buf[sizeof(struct XProtocal)];
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8888);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listenfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if(listen(listenfd, 50))
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	int confd = 0;
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(myaddr);
	pid_t pid;
	int recvsize = 0;
	
	signal(SIGCHLD, reaper);

	db_initializer();

	while(1)
	{
		if((confd = accept(listenfd
			, (struct sockaddr *)&peeraddr, &peerlen)) == -1)
		{
			perror("bind");
			exit(EXIT_FAILURE);
		}
		
		pid = fork();
		
		if(pid < 0)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}else if(pid >0)
		{
			//parent
			close(confd);
		}else 
		{
			//child
			char usrname[MAXLEN_USRNAME];
			unsigned int len = 0;
			unsigned short retval = 0;
			unsigned char data[MAXLEN_DATA];

			close(listenfd);
			while(1)
			{
				recvsize = read(confd, buf, sizeof(buf));
				switch(trans_decode(buf
					,usrname
					,NULL,&len,data))
				{
					case REQ_REGISTER:
						ser_register(confd,usrname,len,data);
						break;
					case REQ_LOGIN:
						ser_login(confd,usrname,len,data);
						break;
					case REQ_QUERYWORD:
						ser_queryword(confd,usrname,len,data);
						break;
					case REQ_HISTORY:
						ser_history(confd,usrname,len,data);
						break;
					case REQ_EXIT:
						close(confd);
						db_logout(usrname);
						printf("connection ended\n");
						exit(EXIT_SUCCESS);

						break;
					default:
						printf("unknown req\n");
						write(confd
							,trans_encode(usrname,REQ_LOGIN,ERR_UNKNOWN, 0,NULL)
							, sizeof(struct XProtocal));
						break;
				}

				if(recvsize == -1)
				{
					perror("recvfrom");
					exit(EXIT_FAILURE);
				}
				if(recvsize == 0)
				{
					break;
				}

			}
			close(confd);
			printf("connection ended\n");
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}

