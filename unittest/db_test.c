#include <stdio.h>
int main()
{
	db_initializer();
	//should return signed in
	switch(db_signin("a","12345"))
	{
		case 0:
			printf("signed in\n");
		case 1:
			printf("already exist\n");
		case -1:
			printf("error\n");
	}
	//should return already exist
	switch(db_signin("a","12345"))
	{
		case 0:
			printf("signed in\n");
		case 1:
			printf("already exist\n");
		case -1:
			printf("error\n");
	}

	switch(db_login(usrname, data))
	{
		case 0:
			printf("loged in\n");
		case 1:
			printf("");
	}
}
