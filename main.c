#include <stdlib.h>

int main(int argc, char **argv)
{
	
	if (scanner(argc, argv) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (!parser()){
		puts("*****************************");
		puts("*** Compilation Completed ***");
		puts("*****************************\n");
		vm();
	}
	// BE SURE TO COMMENT OUT BEFORE SUBMITTING
	system("PAUSE");
}
