/*
 * db.h
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */

//the callback function that will show records on screen
int show_sql_result(
	void *arg, 
	int n_column, 
	char **column_value, 
	char **column_name
	);



//open database
int db_initializer();



/*
signup particular user
The function will return 0 on success, will 
return 1 if the user already exist, and will return
-1 if an error of database happens.
*/ 
int db_signup(char * username, char * passwd);



/*
log in particular user
The function will return 0 on success, will 
return 1 if the user or the password is incorrect,
and will return
-1 if an error of database happens. 
*/
int db_login(char * username, char * passwd);

/*
log out particular user
The function will return 0 on success, will 
return 1 if the user does not exist or not currently active,
and will return
-1 if an error of database happens. 
*/
int db_logout(char * username);

/*
close database
The function will return 0 on success, and return -1 if 
an error happens.
*/
int db_close();

/*
store a search history to the database
The function will return 0 on success, and return -1 if 
an error happens.
*/
int db_history_put(char * username,char * word);

/*
call the callback function for every record of history of the
user 
The function will return 0 on success, and return -1 if 
an error happens.
*/
int db_history_get(char * username,  int (*callback)(void*,int,char**,char**), void *p);
