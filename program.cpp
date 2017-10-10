// Jiayao Li
// Spring 17 CSCE-221-507
// UIN 423002027
// jiayaoli@tamu.edu

#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <stdio.h>
#include <ctype.h>

using namespace std;

//struct to store all the ads info
struct ad{
	string id;
	double height;
	double width;
	double default_bid;
	vector<string> keywords;
	vector<double> bids;
};

vector<ad> ads; //vector of ads to store 
vector<string> inputs; //vector of strings to store user inputs
vector<string> final_id; //vector of strings to store ad ids per requests
vector<double> final_bid; //vector of strings to sore highest bids per requests

int main() {
	//read in ads in standard input
	ifstream file_ads("ads.txt");
	string line_ads;
	while(getline(file_ads, line_ads)){
		//regex pattern to part texts
		regex pattern{R"(([\d]+),\s([\d]+),\s([\d]+),\s([\s\S]+))"};
		smatch matches;
		regex_search(line_ads, matches, pattern);
		
		ad new_ad; //create new ad object
		new_ad.id = matches[1];
		new_ad.height = stod(matches[2]);
		new_ad.width = stod(matches[3]);
		
		string bids_info_raw; //raw data for bids info
		bids_info_raw = matches[4];
		
		//break line into vector of strings
		istringstream iss(bids_info_raw);	
		vector<string> bids_info;	
		for(string s; iss >> s;){
			bids_info.push_back(s);//push string back to vector
		}
		
		regex pattern_bid{R"(([\s\S]+),)"};
		smatch matches_bid;
		regex_search(bids_info[0], matches_bid, pattern_bid);
		new_ad.default_bid = stod(matches_bid[1]);
		
		
		//check what kind of bids information is contained
		if(bids_info.size()==3){//if only one keyword is contained
			regex pattern_keyword_front{R"(\[\"([\s\S]+)\",)"};
			smatch matches_keyword_front;
			regex_search(bids_info[1], matches_keyword_front, pattern_keyword_front);

			string newkeyword;
			newkeyword = matches_keyword_front[1];
			new_ad.keywords.push_back(newkeyword);
			
			regex pattern_bid_end{R"(([\s\S]+)\])"};
			smatch matches_bid_end;
			regex_search(bids_info[2], matches_bid_end, pattern_bid_end);

			double newbid;
			newbid = stod(matches_bid_end[1]);
			new_ad.bids.push_back(newbid);
			
		}
		
		//check the size rest of information stored within brackets
		else if(bids_info.size()>3){
			regex pattern_keyword_front{R"(\[\"([\s\S]+)\",)"};
			smatch matches_keyword_front;
			regex_search(bids_info[1], matches_keyword_front, pattern_keyword_front);

			string newkeyword;
			newkeyword = matches_keyword_front[1];
			new_ad.keywords.push_back(newkeyword);
			
			bool flag=true;//flag is true when i is on numbers
			for(int i=2; i<bids_info.size()-1; i++){
				if(flag){
					regex pattern_mid{R"(([\s\S]+),)"};
					smatch matches_mid;
					regex_search(bids_info[i], matches_mid, pattern_mid);

					double midbid;
					midbid = stod(matches_mid[1]);
					new_ad.bids.push_back(midbid);
				}
				else{//flag is false when i is on keywords
					regex pattern_keyword{R"(\"([\s\S]+)\",)"};
					smatch matches_keyword;
					regex_search(bids_info[i], matches_keyword, pattern_keyword);

					string midkeyword;
					midkeyword = matches_keyword[1];
					new_ad.keywords.push_back(midkeyword);
				}
				flag = !(flag);//swtich flags each turn
			}
			
			regex pattern_bid_end{R"(([\s\S]+)\])"};
			smatch matches_bid_end;
			regex_search(bids_info[bids_info.size()-1], matches_bid_end, pattern_bid_end);

			double newbid;
			newbid = stod(matches_bid_end[1]);
			new_ad.bids.push_back(newbid);
			
		}
		else{
			cout << "ID " << new_ad.id << " cotains insufficient information!" << endl;
		}
		
		ads.push_back(new_ad);
	}
	
	ifstream file_request("request.log");
	string line_request;
	while(getline(file_request, line_request)){ //read test file
		inputs.push_back(line_request); //record user requests
		bool option = true; //true if request by height and width
		
		if(isalpha(line_request[0])) //false if request by keyword
			option = false;
		
		if(option==true){//if request by height and width
			double user_height=0.0;
			double user_width=0.0;
					
			vector<string> results_num;
			vector<double> results_bid;
		
			regex pattern_request{R"(([\d]+),\s([\d]+))"};
			smatch matches_request;
			regex_search(line_request, matches_request, pattern_request);
			user_height = stod(matches_request[1]);
			user_width = stod(matches_request[2]);
			bool found = false;
			
			//loop through ads vector to find match
			for(int i=0; i<ads.size(); i++){
				if(ads[i].height==user_height && ads[i].width==user_width){			
					double temp = 0.0;
					found = true;
					results_num.push_back(ads[i].id);
					temp = ads[i].default_bid;
					for(int j=0; j<ads[i].bids.size(); j++){
						if(ads[i].bids[j]>temp)
							temp = ads[i].bids[j];
					}
					results_bid.push_back(temp);
				}
			}
			//if maches are found, find the highest
			if(found = true){
				string number = "0";
				double highest = 0.0;
				for(int i=0; i<results_bid.size(); i++){
					if(results_bid[i]>highest){
						highest=results_bid[i];
						number=results_num[i];
					}
				}
				final_id.push_back(number);
				final_bid.push_back(highest);
			}
			else{//else reutrn zero for id and bid
				final_id.push_back("0");
				final_bid.push_back(0.0);
			}

		}
		
		else{//if request by keyword
			string user_keyword;				
			vector<string> results_num;
			vector<double> results_bid;
			bool found = false;
			
			istringstream ism(line_request);	
			for(char s; ism >> s;){
				user_keyword.push_back(s);
			}
			
			//loop through ads vector to find match
			for(int i=0; i<ads.size(); i++){
				for(int j=0; j<ads[i].keywords.size(); j++){
					if(ads[i].keywords[j]==user_keyword){
						results_num.push_back(ads[i].id);
						results_bid.push_back(ads[i].bids[j]);
						found = true;
					}
				}
			}
			//if maches are found, find the highest
			if(found = true){
				string number = "0";
				double highest = 0.0;
				for(int i=0; i<results_bid.size(); i++){
					if(results_bid[i]>highest){
						highest=results_bid[i];
						number=results_num[i];
					}
				}
				final_id.push_back(number);
				final_bid.push_back(highest);
			}
			else{//else reutrn zero for id and bid
				final_id.push_back("0");
				final_bid.push_back(0.0);
			}

		}
		
	}

	//print out result:
	for(int i=0; i<inputs.size(); i++){
		cout << "Request: " << inputs[i] << endl;
		cout << "ID: " << final_id[i] << endl;
		cout << "Bid: " << final_bid[i] << endl;
		cout << endl;
	}
	
	return 0;
}