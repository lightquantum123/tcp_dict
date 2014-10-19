#include <stdio.h>
#include "dic.h"
#include <string.h>

int dic_getexp(char * word,char *exp)
{
	FILE * fp;
	char *p;
	char expbuf[300];
	fp = fopen("./dict.txt","r");
	if(fp == NULL)
		return -1;
	
	do{
		p = fgets(expbuf,300,fp);
		if(strncmp(p,word,strlen(word)) == 0)
			break;
	}while(p != NULL);
	
	if(p == NULL)
		return -1;
	else
	{
		p += strlen(word);
		while(*p == ' ')
			++p;
		strcpy(exp, p);
		return 0;
	}
}

