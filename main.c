#define _DEFAULT_SOURCE
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "filefind.h"

int main(int argc, char *argv[]) { 
	if (argc != 2) {
		printf("Expected only 1 filename");
		return 1;
	}
	init_find_file(argv[1]);
	return 0; 
}
