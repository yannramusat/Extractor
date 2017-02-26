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
void parse_opt(int* e, int* d, int* v, int argc, char** argv) {
	while(1) {
		int opt = getopt(argc, argv, "e:d:vh");
		if(opt < 0) break;
		switch(opt) {
			case 'e': 
				*e = atoi(optarg);
				break;
			case 'd':
				*d = atoi(optarg);
				break;
			case 'v':
				*v = 1;
				break;
			case 'h':
			default:
				fprintf (stdout, " Usage: %s\n", argv[0]);
				fprintf (stdout, "  -e,	<int>		Precision for the pick.	Default value: 9\n");
				fprintf (stdout, "  -d,	<int>		Interval per day.	Default value: 1\n");
				fprintf (stdout, "  -v,			Verbose mode.		Default: off\n");
				fprintf (stdout, "  -h, 			Display help.\n");
				exit(1);
		}
	}
}

/* Specific functions */
void parse_source(map <string, vector<int> >& occurences, int d, string src) {
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
						occurences[word].resize(31*d,0);
						int day = (timestamp-begin)*d/86400;
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

int filter(string tmp) {
	int result = 0;
	if(tmp.compare("thoughts") == 0) result = 1;
	if(tmp.compare("tribute") == 0) result = 1;
	if(tmp.compare("lit") == 0) result = 1;
	if(tmp.compare("colors") == 0) result = 1;
	if(tmp.compare("flag") == 0) result = 1;
	if(tmp.compare("spontaneous") == 0) result = 1;
	if(tmp.compare("deeply") == 0) result = 1;
	if(tmp.compare("saddened") == 0) result = 1;
	if(tmp.compare("deplorable") == 0) result = 1;
	if(tmp.compare("love") == 0) result = 1;
	if(tmp.compare("prayers") == 0) result = 1;
	return result;
}

void construct_cograph(map <string, map <string, int> >& graph, string location, int* cortweets, int* nb_links, string src) {
	ifstream tweets_src(src.c_str());
	if(tweets_src) {
		while(!tweets_src.eof()) {
			int timestamp;
			tweets_src >> timestamp;
			if(timestamp > 0) { // handle last line
				string contain;
				getline(tweets_src, contain);
				//cout << ">>> Date: " << timestamp << " <<<" << endl;	
				istringstream words(contain);
				while(!words.eof()) {
					string word;
					words >> word;
					/* Verifying if it's a correlated tweet */
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
										if(filter(tmp) == 0)
											keywords.push_back(tmp);
									}
								}
							}
							for(unsigned int i = 0; i < keywords.size(); i++) {
								for(unsigned int j = i+1; j < keywords.size(); j++) {
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

void initialize_buckets(map <string, map <string, int> >& graph, vector<set<string> >& deltas) {
	map <string, map <string, int> >::iterator q;
	for(q = graph.begin(); q != graph.end(); q++) {
		map <string, int>::iterator r;
		int degree = 0;
		for(r = q->second.begin(); r != q->second.end(); r++)
			degree += r->second;
		deltas[degree].insert(q->first);
	}
}

void peel_graph(map <string, map <string, int> >& graph, map <string, map <string, int> >& best_graph, vector<set<string> >& deltas, int* nb_links) {
	double r_g = *nb_links / graph.size();
	int min_degree = 0;
	while(graph.size() > 0) {
		// find a min
		while(deltas[min_degree].size() == 0) min_degree++;
		set <string>::iterator itest = deltas[min_degree].begin();
		string node = *itest;
		deltas[min_degree].erase(deltas[min_degree].find(node));
		// update graph
		*nb_links -= min_degree;			
		map <string, int>::iterator r;
		for(r = graph[node].begin(); r != graph[node].end(); r++) {
			int degree1 = 0;
			map <string, int>::iterator s;
			for(s = graph[r->first].begin(); s != graph[r->first].end(); s++)
				degree1 += s->second;
			int degree2 = degree1-(r->second);
			string neigh = r->first;
			deltas[degree1].erase(neigh);
			deltas[degree2].insert(neigh);
			if(degree2 < min_degree) min_degree = degree2;
			graph[r->first].erase(node);
		}
		graph.erase(node);
		// update best_graph
		if(graph.size() != 0) {
			if(*nb_links / graph.size() > r_g) {
				r_g = *nb_links / graph.size();
				best_graph = graph;
			}
		}
	}
}

void print_graph(map <string, map <string, int> >& best_graph, ofstream& fichier_sortie) {
	map <string, map <string, int> >::iterator it;
	map <int, string> keywords;
	int seuil = 0;
	for(it = best_graph.begin(); it != best_graph.end(); it++) {
		map <string, int>::iterator r;
		int degree = 0;
		for(r = it->second.begin(); r != it->second.end(); r++) degree += r->second;
		if (degree > seuil && keywords.size() > 4) {
			keywords.erase(keywords.begin());
			keywords[degree] = it->first;
			seuil = degree;
		} else if(degree > seuil) {
			keywords[degree] = it->first;
		}
	}	
	map <int, string>::iterator	ite;
	for(ite = keywords.begin(); ite != keywords.end(); ite++) {
		cout << "  " << ite->second << endl;
		fichier_sortie << " " << ite->second;
	}
	fichier_sortie << endl;
}
