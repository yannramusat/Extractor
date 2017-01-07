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

int main(int argc, char **argv) {
	map <string, vector<int> > occurences;
	
	string src = "data/marchCrisis";
	int e = 9;
	int v = 0;
	parse_opt(&e, &v, argc, argv);

	/* Parsing source file */ 
	cout << "Parsing source file.." << endl;
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
							// DICTIONNAIRE POUR CODER LES FREQUENCES
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
	
	/* Find and solve peaks */
	map<string, vector<int> >::iterator p;
	for(p = occurences.begin(); p != occurences.end(); p++) {
		bool ok = false;
		int sum = 0;
		for(int i = 0; i < 31; i++) {
			sum += p->second[i];
		}
		sum /= 31;
		for(int i = 0; i < 31; i++) {
			if(p->second[i] > e*sum+e*e) ok = true;
		}
		if(ok) {
			/* Print occurences day per day */
			cout << p->first;
			for(int i = 0; i < 31; i++) {
				cout << " "<< p->second[i];
			}
			cout << endl;

			/* Construct the co-occurence graph if this word has a peak */
			map <string, map <string, int> > graph;
			//map <pair<string, string>, int> graph;
			ifstream tweets_src(src.c_str());
			int cortweets = 0;
			int nb_links = 0;
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
								if(word.compare(p->first) == 0) {
									//cout << "A tweet is found related with this location" << endl;
									cortweets++;
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
									//cout << keywords.size() << endl;
									//for(int i = 0; i < keywords.size(); i++) cout << keywords[i] << endl;
									for(int i = 0; i < keywords.size(); i++) {
										for(int j = i+1; j < keywords.size(); j++) {
											if(keywords[i].compare(keywords[j]) != 0){
												/*pair<string, string> couple;
												couple = make_pair(keywords[i], keywords[j]);
												graph[couple]++;
												couple = make_pair(keywords[j], keywords[i]);
												graph[couple]++;*/
												graph[keywords[i]][keywords[j]]++;
												graph[keywords[j]][keywords[i]]++;
												nb_links++;
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
			/*map <pair<string, string>, int>::iterator q;
			for(q = graph.begin(); q != graph.end(); q++) {
				cout << "Edge of weight " << q->second << " between " << q->first.first << " and " << q->first.second << endl;
			}*/
			map <string, map <string, int> >::iterator q;
			int max_degree = 0;
			for(q = graph.begin(); q != graph.end(); q++) {
				map <string, int>::iterator r;
				int degree = 0;
				for(r = q->second.begin(); r != q->second.end(); r++) {
					//cout << "Edge of weight " << r->second << " between " << q->first << " and " << r->first << endl;
					degree += r->second;
				}
				if(degree > max_degree) max_degree = degree;
			}	
			cout << "Correlated tweets: " << cortweets << " Total edge weight: " << nb_links << " Keywords: " << graph.size()  << " Max degree: " << max_degree << endl;	

			/* Graph mining */
			double r_g = nb_links / graph.size();
			//double r_bg = r_g;
			map <string, map <string, int> > best_graph = graph;
			
			// buckets to do it in linear time
			vector<set<string> > deltas;
			deltas.resize(max_degree+1);
			for(q = graph.begin(); q != graph.end(); q++) {
				map <string, int>::iterator r;
				int degree = 0;
				for(r = q->second.begin(); r != q->second.end(); r++) {
					//cout << "Edge of weight " << r->second << " between " << q->first << " and " << r->first << endl;
					degree += r->second;
				}
				deltas[degree].insert(q->first);
			}
			/*for(int i = 0; i < deltas.size(); i++) {
				if(deltas[i].size() != 0) {
					cout << "Degree " << i << ": ";
					list<string>::iterator j;
					for(j = deltas[i].begin(); j != deltas[i].end(); j++) {
						cout << *j << " ";
					}
					cout << endl;
				}
			}*/

			// peel iteratively the graph
			int min_degree = 0;
			while(graph.size() > 5) {
				// find a min
				while(deltas[min_degree].size() == 0) min_degree++;
				//string node = deltas[min_degree].back();
				//deltas[min_degree].pop_back();
				set <string>::iterator itest = deltas[min_degree].begin();
				string node = *itest;
				deltas[min_degree].erase(deltas[min_degree].find(node));
				//cout << "We want to delete: " << node << " of degree: " << min_degree << endl;
				// update graph
				nb_links -= min_degree;				
				map <string, int>::iterator r;
				for(r = graph[node].begin(); r != graph[node].end(); r++) {
					//cout << "	We consider neighboor: "<< r->first << endl;
					// update deltas
						int degree1 = 0;
						map <string, int>::iterator s;
						for(s = graph[r->first].begin(); s != graph[r->first].end(); s++) {
							degree1 += s->second;
						}
						int degree2 = degree1-(r->second);
						string neigh = r->first;
						// suppr le voisin de la liste degree1
						/*list<string>::iterator itd;
						for(itd = deltas[degree1].begin(); itd != deltas[degree1].end(); ++itd) {
							cout<<"		search "<<*itd<<" in degree "<<degree1<<endl;
							if(neigh.compare(*itd) == 0) { cout << "		succeed" << endl; deltas[degree1].erase(itd);}
							cout<<"		test_done"<<endl;
						}*/
						deltas[degree1].erase(neigh);
						//cout << "		Deleted in: " << degree1 << endl;
						deltas[degree2].insert(neigh);
						//cout << "		Added in: " << degree2 << endl;
						if(degree2 < min_degree) min_degree = degree2;
					graph[r->first].erase(node);
				}
				graph.erase(node);
				// update best_graph
				if(graph.size() != 0) {
					if(nb_links / graph.size() > r_g) {
						r_g = nb_links / graph.size();
						best_graph = graph;
					}
				}
			}

			/* Printing keywords of the densest subgraph */
			map <string, map <string, int> >::iterator it;
			for(it = best_graph.begin(); it != best_graph.end(); it++) {
				cout << it->first << endl;
			}
		}
	}
    return 0;
}
