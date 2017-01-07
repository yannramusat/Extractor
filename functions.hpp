#include <stdlib.h> 
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <sstream>  
#include <map>
#include <string>
#include <vector>
#include <list>
#include <set>
using namespace std;

/* Core functions */
void parse_opt(int* e, int* v, int argc, char** argv);
/* General functions */
/* Specific functions */
void parse_source(map <string, vector<int> >& occurences, string src);
void construct_cograph(map <string, map <string, int> >& graph, string location, int* cortweets, int* nb_links, string src);
