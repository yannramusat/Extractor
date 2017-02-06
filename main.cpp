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
	int d = 1;
	int v = 0;
	parse_opt(&e, &d, &v, argc, argv);

	/* Parsing source file */ 
	map <string, vector<int> > occurences;
	cout << "Parsing source file... ";
	cout.flush();
	parse_source(occurences, d, src);
	cout << "Parsing done." << endl;
	
	/* Find and solve peaks */
	map<string, vector<int> >::iterator p;
	bool fpeaks = false;
	ofstream fichier_sortie("results.txt", ios::out | ios::trunc);
	for(p = occurences.begin(); p != occurences.end(); p++) {
		/* Verify if there's a peak */
		bool ok = false;
		int sum = 0;
		for(int i = 0; i < 31*d; i++) {
			sum += p->second[i];
		}
		sum /= 31;
		for(int i = 0; i < 31*d; i++) {
			if(p->second[i] > e*sum+e*e) { ok = true; fpeaks = true; }
		}

		/* Start mining if yes */
		if(ok) {
			/* Print occurences day per day */
			cout << "================================================================" << endl;
			cout << "Peak found for location <<<" << p->first << ">>> in day(s):";
			for(int i = 0; i < 31*d; i++) {
				if(p->second[i] > e*sum+e*e) cout << " "<< (int)i/d+1 << "(" << (24/d)*(i%d) << "h)";
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

			/* Initialize buckets to do it in linear time */
			vector <set <string> > deltas;
			deltas.resize(max_degree+1);
			initialize_buckets(graph, deltas);

			/* Peel iteratively the graph */
			map <string, map <string, int> > best_graph = graph;
			peel_graph(graph, best_graph, deltas, &nb_links);
			cout << "Done." << endl;

			fichier_sortie << p->first;
			/* Printing keywords of the densest subgraph */
			cout << "Most relevant keywords: " << endl;
			print_graph(best_graph, fichier_sortie);
		}
	}
	fichier_sortie.close();
	if(!fpeaks) cout << "No peak found." << endl;
	else cout << "\nAll results have been saved in 'results.txt'." << endl;
    return 0;
}
