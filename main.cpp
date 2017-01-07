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
	string src = "data/marchCrisis";
	int e = 9;
	int v = 0;
	parse_opt(&e, &v, argc, argv);

	/* Parsing source file */ 
	map <string, vector<int> > occurences;
	cout << "Parsing source file... ";
	cout.flush();
	parse_source(occurences, src);
	cout << "Parsing done." << endl;
	
	/* Find and solve peaks */
	map<string, vector<int> >::iterator p;
	bool fpeaks = false;
	for(p = occurences.begin(); p != occurences.end(); p++) {
		/* Verify if there's a peak */
		bool ok = false;
		int sum = 0;
		for(int i = 0; i < 31; i++) {
			sum += p->second[i];
		}
		sum /= 31;
		for(int i = 0; i < 31; i++) {
			if(p->second[i] > e*sum+e*e) { ok = true; fpeaks = true; }
		}

		/* Start mining if yes */
		if(ok) {
			/* Print occurences day per day */
			cout << "================================================================" << endl;
			cout << "Peak found for location <<<" << p->first << ">>> in day(s): ";
			for(int i = 0; i < 31; i++) {
				if(p->second[i] > e*sum+e*e) cout << " "<< i;
			}
			cout << endl;

			/* Construct the co-occurence graph if this word has a peak */
			map <string, map <string, int> > graph;
			int cortweets = 0;
			int nb_links = 0;
			cout << "Start constructing the co-occurence graph... ";
			cout.flush();
			construct_cograph(graph, p->first, &cortweets, &nb_links, src);	
			cout << "Done." << endl;	

			/* Compute max_degree */
			map <string, map <string, int> >::iterator q;
			int max_degree = 0;
			for(q = graph.begin(); q != graph.end(); q++) {
				map <string, int>::iterator r;
				int degree = 0;
				for(r = q->second.begin(); r != q->second.end(); r++) degree += r->second;
				if(degree > max_degree) max_degree = degree;
			}
			
			/* Print some stats */	
			cout << "  Correlated tweets: " << cortweets << endl << "  Total edge weight: " << nb_links << endl << "  Keywords: " << graph.size()  << endl << "  Max degree: " << max_degree << endl;	

			/* Graph mining */
			cout << "Start mining the graph... ";
			cout.flush();
			double r_g = nb_links / graph.size();
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
			cout << "Done." << endl;

			/* Printing keywords of the densest subgraph */
			cout << "Most relevant keywords: " << endl;
			map <string, map <string, int> >::iterator it;
			for(it = best_graph.begin(); it != best_graph.end(); it++) {
				cout << "  " << it->first << endl;
			}			
		}
	}
	if(!fpeaks) cout << "No peak found." << endl;
    return 0;
}
