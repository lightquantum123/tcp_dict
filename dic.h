/*
 * dic.h
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */


//The parameter word should hold the string of 
//the querying word. The function will store the
//respective explaination to the exp string.
//The memory for both parameters should allocated by the caller.
//This function will return 0 on success, and will return -1 
//when it fails.
int dic_getexp(char * word,char *exp);
