#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db.h"

#define MAX 100


static sqlite3 *globaldb = NULL;

static int show_sql_result(
	void *arg, 
	int n_column, 
	char **column_value, 
	char **column_name
	)
{
    int i = 0;

    for(i = 0;i < n_column; i++) {
        printf("%s|", column_name[i]);
    }
    printf("\b \n");

    for(i = 0;i < n_column; i++) {
        printf("%s|", column_value[i]);
    }
    printf("\b \n");

    return 0;
}

static int exec_sql(char *sql_string, sqlite3 *db)
{
    char *errmsg;

    if( SQLITE_OK != sqlite3_exec(
    			db,	/* handler to the db connection */
    			sql_string, 	/* SQL statements */
    			show_sql_result,/* callback */ 
    			NULL, 		/* param to the callback */
    			&errmsg)) {	/* error message */
        fprintf(stderr, "sqlite3_exec(%s): %s.\n", sql_string, errmsg);
        sqlite3_free(errmsg);
        return 1;
    }

    return 0;
}

static int db_check_exist_method(
	void *arg, 
	int n_column, 
	char **column_value, 
	char **column_name
	)
{
	int * p = (int *) arg;
	*p++;
}


static int db_check_exist(char * username, sqlite3 *db)
{
	char cmd[256];
	int result = 0;
	sprintf(cmd,"SELECT name FROM loginlist WHERE name='%s';"
		,username);

	char *errmsg;

	if( SQLITE_OK != sqlite3_exec(
    			db,	/* handler to the db connection */
    			cmd, 	/* SQL statements */
    			db_check_exist_method,/* callback */ 
    			&result, 	/* param to the callback */
    			&errmsg)) {	/* error message */
        fprintf(stderr, "sqlite3_exec(%s): %s.\n", cmd, errmsg);
        sqlite3_free(errmsg);
        return -1;
	}
	return result;
}


static int db_check_passwd(char * username, char * passwd,sqlite3 *db)
{
	char cmd[256];
	int result = 0;
	sprintf(cmd,"SELECT name,passwd FROM loginlist WHERE "
		"name='%s' AND passwd='%s' AND state='N';"
		,username, passwd);

	char *errmsg;

	if( SQLITE_OK != sqlite3_exec(
    			db,	/* handler to the db connection */
    			cmd, 	/* SQL statements */
    			db_check_exist_method,/* callback */ 
    			&result, 	/* param to the callback */
    			&errmsg)) {	/* error message */
        fprintf(stderr, "sqlite3_exec(%s): %s.\n", cmd, errmsg);
        sqlite3_free(errmsg);
        return -1;
	}

	return result;
}

int db_initializer()
{
	int result;
	result = sqlite3_open("server.db", &globaldb);
	if (result != SQLITE_OK) 
	{
 		if (NULL != globaldb) 
		{
			fprintf(stderr, "sqlite3_open : %s.\n",
	     			 sqlite3_errmsg(globaldb));
		}else 
		{
			printf("error : failed to allocate memory for sqlite3!\n");
		}
		exit(EXIT_FAILURE);
	}
	return 0;
}

int db_signin(char * username, char * passwd)
{
	int exist = 0;
	char cmd[256];
	if((exist = db_check_exist(username, globaldb)) == 0)
	{
		//user does not exist
		//create new user
		sprintf(cmd,"insert into loginlist"
			" values('%s', '%s', 'N')", username, passwd);
		exec_sql(cmd, globaldb);
		return 0;
	}else
	{
		if(exist == -1)
		{
			return -1;
		}else
		{
			printf("user already exist\n");
			return 1;
		}
	}
}

int db_login(char * username, char * passwd)
{
	int exist = 0;
	char cmd[256];
	if((exist = db_check_passwd(username, passwd, globaldb)) == 1)
	{
		//user does not exist
		//create new user
		sprintf(cmd,"update loginlist set state='A'"
			" where name='%s' AND passwd='%s'", username, passwd);
		exec_sql(cmd, globaldb);
		return 0;
	}else
	{
		if(exist == -1)
		{
			return -1;
		}else
		{
			printf("no such user\n");
			return 1;
		}
	}
}


int db_islogin(char * username)
{
	char cmd[256];
	int result = 0;
	sprintf(cmd,"SELECT name,passwd FROM loginlist WHERE "
		"name='%s' AND state='A';"
		,username, passwd);

	char *errmsg;

	if( SQLITE_OK != sqlite3_exec(
    			db,	/* handler to the db connection */
    			cmd, 	/* SQL statements */
    			db_check_exist_method,/* callback */ 
    			&result, 	/* param to the callback */
    			&errmsg)) {	/* error message */
        fprintf(stderr, "sqlite3_exec(%s): %s.\n", cmd, errmsg);
        sqlite3_free(errmsg);
        return -1;
	}

	return result;
}

int db_logout(char * username)
{
	int exist = 0;
	char cmd[256];
	if((exist = db_check_exist(username, globaldb)) == 1)
	{
		//user does not exist
		//create new user
		sprintf(cmd,"update loginlist set state='N'"
			" where name='%s'", username);
		exec_sql(cmd, globaldb);
		return 0;
	}else
	{
		if(exist == -1)
		{
			return -1;
		}else
		{
			printf("no such user\n");
			return 1;
		}
	}
}

int db_close()
{
	int result;
	result = sqlite3_close(globaldb);
	if(result != SQLITE_OK) 
	{
		fprintf(stderr, "sqlite3_close : %s.\n",
        		 sqlite3_errmsg(globaldb));
        	exit(EXIT_FAILURE);
    	}
	return 0;
}

int db_history_put(char * username,char * word)
{
	struct tm * p = NULL;
	time_t t = time(NULL);
	int i;
	char cmd[256];
	
	char timestr[128];
	p = localtime(&t);
	sprintf(timestr,"%4d-%d-%d %2d:%2d:%2d\n", p->tm_year + 1900, 
			p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
			p->tm_sec);

	sprintf(cmd,"insert into history"
			" values('%s', '%s', '%s')", username, timestr,word);
	return exec_sql(cmd, globaldb);
}

int db_history_get(char * username,  int (*callback)(void*,int,char**,char**))
{
	char cmd[256];
	sprintf(cmd,"SELECT * FROM history WHERE "
		"name='%s';"
		,username);

	char *errmsg;

	if( SQLITE_OK != sqlite3_exec(
    			globaldb,	/* handler to the db connection */
    			cmd, 	/* SQL statements */
    			callback,/* callback */ 
    			NULL, 	/* param to the callback */
    			&errmsg)) {	/* error message */
        fprintf(stderr, "sqlite3_exec(%s): %s.\n", cmd, errmsg);
        sqlite3_free(errmsg);
        return -1;
	}

	return 0;

}

