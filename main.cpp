#include <stdlib.h> 
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <sstream>  
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
	cout << "Program start.." << endl;

	map <string, vector<int> > occurences;
	string src = "data/marchCrisis";
	int e = 5;
	int v = 0;

	/* Parsing options and parameters */
	while(1) {
		int opt = getopt(argc, argv, "e:vh");
		if(opt < 0) break;
		switch(opt) {
			case 'e': 
				e = atoi(optarg);
				break;
			case 'v':
				v = 1;
				break;
			case 'h':
			default:
				fprintf (stdout, " Usage: %s\n", argv[0]);
				fprintf (stdout, "  -e,	<int>		Precision for the pick.	Default value: 5\n");
				fprintf (stdout, "  -v,			Verbose mode.		Default: off\n");
				fprintf (stdout, "  -h, 			Display help.\n");
				exit(1);
		}
	}

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
			ifstream tweets_src(src.c_str());
			int cortweets = 0;
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
										// TODO!!!
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
			cout << cortweets << endl;
		}
	}
    return 0;
}
