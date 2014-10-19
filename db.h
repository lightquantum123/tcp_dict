/*
 * db.h
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */
int show_sql_result(
	void *arg, 
	int n_column, 
	char **column_value, 
	char **column_name
	);

int db_initializer();

int db_signup(char * username, char * passwd);

int db_login(char * username, char * passwd);

int db_logout(char * username);

int db_close();

int db_history_put(char * username,char * word);

int db_history_get(char * username,  int (*callback)(void*,int,char**,char**), void *p);
