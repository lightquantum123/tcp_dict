
int db_initializer();

int db_signin(char * username, char * passwd)

int db_login(char * username, char * passwd);

int db_logout(char * username);

int db_close();

int db_history_put(char * username,char * word)

int db_history_get(char * username,  int (*callback)(void*,int,char**,char**))
