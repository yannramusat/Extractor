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
#include "functions.hpp"
using namespace std;

/* Core functions */
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

/* General functions */

/* Specific functions */
void parse_source(map <string, vector<int> >& occurences, string src) {
	ifstream tweets_src(src.c_str());
	bool notsee = true;
	int begin = 0;
	if(tweets_src) {
		while(!tweets_src.eof()) {
			int timestamp;
			tweets_src >> timestamp;
			if(notsee) {
				begin = timestamp; 
				notsee = false;
			}
			if(timestamp > 0) { // handle last line
				string contain;
				getline(tweets_src, contain);
				//cout << ">>> Date: " << timestamp << " <<<" << endl;	
				istringstream words(contain);
				while(!words.eof()) {
					string word;
					words >> word;
					/* Studying word by word */
					// highlight locations
					size_t found = word.find("b-geo-loc_");
					if (found!=std::string::npos) {
						word = word.substr(10, string::npos);
						// TODO SUPPR HASHTAG
						//cout << "Location: " << word << endl;
						// update frequencies
						occurences[word].resize(31,0);
						int day = (timestamp-begin)/86400;
						occurences[word][day]++;
					}
					//cout << word << endl;
				}
				timestamp = -1;	// handle last line
			}
		}	
		tweets_src.close();
    } else {
		cout << "Cannot open the source." << endl;
	}
}

void construct_cograph(map <string, map <string, int> >& graph, string location, int* cortweets, int* nb_links, string src) {
	ifstream tweets_src(src.c_str());
	bool notsee = true;
	int begin = 0;
	if(tweets_src) {
		while(!tweets_src.eof()) {
			int timestamp;
			tweets_src >> timestamp;
			if(notsee) {
				begin = timestamp; 
				notsee = false;
			}
			if(timestamp > 0) { // handle last line
				string contain;
				getline(tweets_src, contain);
				//cout << ">>> Date: " << timestamp << " <<<" << endl;	
				istringstream words(contain);
				while(!words.eof()) {
					string word;
					words >> word;
					/* Verifying if it's a correlated tweet*/
					// highlight locations
					size_t found = word.find("b-geo-loc_");
					if (found!=std::string::npos) {
						word = word.substr(10, string::npos);
						// TODO SUPPR HASHTAG
						if(word.compare(location) == 0) {
							//cout << "A tweet is found related with this location" << endl;
							(*cortweets)++;
							/* Analyzing the correlated tweet */
							istringstream wordss(contain);
							vector<string> keywords;
							string tmp;
							while(wordss >> tmp) {
								size_t found = tmp.find("/"); // delete anotations
								if(found==std::string::npos) {
									found = tmp.find("_"); // delete tags
									if(found==std::string::npos) {
										keywords.push_back(tmp);
									}
								}
							}
							for(int i = 0; i < keywords.size(); i++) {
								for(int j = i+1; j < keywords.size(); j++) {
									if(keywords[i].compare(keywords[j]) != 0){
										graph[keywords[i]][keywords[j]]++;
										graph[keywords[j]][keywords[i]]++;
										(*nb_links)++;
									}
								}
							}
						}
					}
				}
				timestamp = -1;	// handle last line
			}
		}	
		tweets_src.close();
	} else {
		cout << "Cannot open the source." << endl;
	}
}
