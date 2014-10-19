/*
 * dic_test.c
 *
 *  Created on: Oct 19, 2014
 *      Author: Ziyuan Jiang
 */
#include <stdio.h>
#include <string.h>
#include "../dic.h"

int main()
{
	char exp[304];
	char word[304];
	strcpy(word, "periodic");
	printf("%d\n",dic_getexp(word,exp));
	puts(exp);
}
