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
void parse_opt(int* e, int* d, int* v, int argc, char** argv);
/* Specific functions */
void parse_source(map <string, vector<int> >& occurences, int d, string src);
void construct_cograph(map <string, map <string, int> >& graph, string location, int* cortweets, int* nb_links, string src);
void initialize_buckets(map <string, map <string, int> >& graph, vector<set<string> >& deltas);
void peel_graph(map <string, map <string, int> >& graph, map <string, map <string, int> >& best_graph, vector<set<string> >& deltas, int* nb_links);
void print_graph(map <string, map <string, int> >& best_graph);
