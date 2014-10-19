/*
 * db_test.c
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */
#include <stdio.h>
#include "../db.h"



int main()
{
	db_initializer();
	//should show signed in
	printf("db_signup(a,12345)\n");
	switch(db_signup("a","12345"))
	{
		case 0:
			printf("db_signed in\n");
			break;
		case 1:
			printf("db_already exist\n");
			break;
		case -1:
			printf("db_error\n");
			break;
	}
	//should show already exist
	printf("db_signup(a,12345)\n");
	switch(db_signup("a","12345"))
	{
		case 0:
			printf("db_signed in\n");
			break;
		case 1:
			printf("db_already exist\n");
			break;
		case -1:
			printf("db_error\n"); 
			break;
	}

	//should show logined in
	printf("db_login(a,12345)\n");
	switch(db_login("a", "12345"))
	{
		case 0:
			printf("db_loged in\n");
			break;
		case 1:
			printf("db_no such user\n");
			break;
		case -1:
			printf("db_error\n");
			break;
	}
	
	//should show no such user
	printf("db_login(a,12345)\n");
	switch(db_login("a", "12345"))
	{
		case 0:
			printf("db_loged in\n");
			break;
		case 1:
			printf("db_no such user\n");
			break;
		case -1:
			printf("db_error\n");
			break;
	}
	//should show loged in
	printf("db_islogin(a)\n");
	switch(db_islogin("a"))
	{
		case 1:
			printf("db_loged in\n");
			break;
		case 0:
			printf("db_not loged in\n");
			break;
		case -1:
			printf("db_error\n");
			break;
	}
	//should
	printf("db_logout(a)\n");
	switch(db_logout("a"))
	{
		case 1:
			printf("db_loged out\n");
			break;
		case 0:
			printf("db_not loged in\n");
			break;
		case -1:
			printf("db_error\n");
			break;
	}
	//should show not loged in
	printf("db_islogin(a)\n");
	switch(db_islogin("a"))
	{
		case 1:
			printf("db_loged in\n");
			break;
		case 0:
			printf("db_not loged in\n");
			break;
		case -1:
			printf("db_error\n");
			break;
	}
	db_history_put("a","word");
	db_history_get("a",show_sql_result,NULL);
	db_close();
}
