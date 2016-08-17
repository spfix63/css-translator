/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include "CodeGen.h"
#include <stdio.h>

extern FILE *yyin;

int main(int argc, char *argv[]) 
{
	if (argc != 4)
	{
		printf("Usage: %s inputFn headerFn codeFn\n", argv[0]);
		return -1;
	}
	if (!strcmp(argv[2], argv[3]))
	{
		printf("Usage: %s inputFn headerFn codeFn\nOutput file names musn't match.\n", argv[0]);
		return -1;
	}
	CG_init(argv[2], argv[3]);

	yyin = fopen(argv[1], "r");
	if (yyin) 
		yyparse();
	else
	{
		printf("error: Couldn't open file %s\n", argv[1]); 
		fclose (yyin);
		return -1;
	}
	fclose (yyin);
	
	if (CG_flush() == -1)
		printf("No output was generated because of an error.");
	else
		printf("Program completed successfully.");
	CG_destroy();
	return 0;
}