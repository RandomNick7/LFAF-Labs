#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "grammar.hpp"
#include "input.hpp"

int main(){
	srand(time(0));
	/*Take a set of rules as input
	Why solve 1 variant when you can solve ALL?
	⠻⣦⡈⠛⢿⡌⢆⢂⠹⣿⣧⡄⡄⠹⡄⢆⡟⢄⠉⢟⣮⣷⢄⠐⢤⡤⢤⣬⣭⣭
	⣷⣤⠙⣶⣦⠙⡌⢻⠂⠘⣿⣷⢹⠄⢳⠘⣿⡟⣿⡄⢳⡆⢳⠛⢢⠈⠉⠛⠛⠛
	⢿⣿⣷⣀⠈⠓⠌⠂⠘⢆⠈⢙⣎⢣⠘⡆⢣⡛⣿⡇⢀⡙⠚⠶⠶⠿⠷⠶⠒⠒
	⣾⠿⣿⣿⣦⣄⣁⣀⣠⡀⠡⠄⠳⡛⢆⠱⠈⢧⣿⡇⠸⠟⠉⠐⠠⠄⠄⠄⣼⣿
	⣿⣷⣬⣉⣛⣛⠛⠿⠿⣧⣷⣦⣄⠘⢿⡆⠡⠘⣀⡇⢠⣶⣷⣮⣤⣤⣴⣿⣿⣿
	⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⢿⡀⠄⠉⡇⢸⣿⣿⣿⣉⣉⣉⣉⣉⣰
	⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⣿⣦⡙⠄⠄⠁⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿
	⣿⡙⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠁⣿⣿⣷⣌⠄⠄⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿
	⣿⣿⣦⣀⡨⣝⡻⠿⣻⣿⣧⣄⠄⠄⣿⣿⣿⣿⣿⣦⡀⢻⣿⣿⣿⣿⣿⡿⠿⣿
	⣿⣿⣿⣿⡇⢿⡛⢿⣿⣿⣿⣿⣧⠘⣿⣛⣻⣿⣿⣿⣿⣤⡿⢿⠿⣛⡃⠄⣸⣿
	⡹⣿⣿⣿⣿⣼⣷⣶⣝⢿⣿⣿⣿⣧⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⣿⢇⣈⣼⣿⣿
	⣿⣜⢿⣭⢻⣿⣿⣿⣯⢧⠙⢻⠛⣛⣛⠛⠛⠿⠿⠟⣛⣥⣶⣼⢏⣾⣿⣿⣿⣿
	⣿⣿⣎⠿⣇⢻⣿⣿⣿⡌⢷⣆⢭⣓⣒⣒⣛⣣⣶⠞⣵⣿⣿⣿⡿⣱⣾⡟⣵⣿
	⣿⣿⣿⣤⣻⡘⣿⣿⣿⣷⡜⢻⣮⣛⣛⣟⣫⡿⣛⣴⣿⣿⣿⣿⢃⣿⣿⣷⣿⣿
	⣿⣿⣿⣿⣷⣣⢹⣿⣿⣿⣿⣯⡙⠛⠛⠛⢛⣺⣿⣿⣿⣿⣿⠇⣼⢫⣵⣿⣿⣿
	Grammar class works with Type 2 and 3 grammars
	FA works with Type 3 right grammars only
	*/
	int err = 0;
	string buffer;
	vector<char> V_n;
	vector<char> V_t;
	vector<string> P[2];
	
	cout << "List the values spearated by spaces or commas" << endl;
		cout << "List the non-terminal characters" << endl;
		getline(cin,buffer);
		err = process_vars(buffer,&V_n,0);
	
	if(!err){
		cout << "List the terminal characters" << endl;
		getline(cin,buffer);
		err = process_vars(buffer,&V_t,1);
	}
	
	if(!err){
		cout << "Indicate Production Rules (format: \"char->string\")" << endl;
		cout << "Enter a hyphen to continue" << endl;
		process_rules(V_n,V_t,P,buffer);
	
		//Objective 1: (check grammar.hpp for the class itself)
		Grammar* G = new Grammar(V_n,V_t,P);
		
		//Objective 2: Randomly generated from given rules
		int word_count = 5;
		for(int i=0;i<word_count;i++){
			cout << "Word " << i+1 << ": " << G->generate_string() << endl;
		}
		
		//Objective 3: Convert Grammar to Finite Automata
		FiniteAutomaton F = G->convert();
		
		//Objective 4: Check a string's validity
		cout << "Enter strings to check if they belong to the language!" << endl;
		cout << "Hyphen to exit" << endl;
		cin >> buffer;
		while(buffer.size()!=1 && buffer[0]!='-'){
			if(F.is_valid_word(buffer)){
				cout << "Belongs to language!" << endl;
			}else{
				cout << "Does not belong!" << endl;
			}
			cin >> buffer;
		}
	}
	return err;
}