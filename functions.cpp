#include <stdlib.h> 
#include <getopt.h>
#include <fstream>

#include "functions.hpp"

void parse_opt(int* e, int* v, int argc, char** argv) {
	while(1) {
		int opt = getopt(argc, argv, "e:vh");
		if(opt < 0) break;
		switch(opt) {
			case 'e': 
				*e = atoi(optarg);
				break;
			case 'v':
				*v = 1;
				break;
			case 'h':
			default:
				fprintf (stdout, " Usage: %s\n", argv[0]);
				fprintf (stdout, "  -e,	<int>		Precision for the pick.	Default value: 9\n");
				fprintf (stdout, "  -v,			Verbose mode.		Default: off\n");
				fprintf (stdout, "  -h, 			Display help.\n");
				exit(1);
		}
	}
}
