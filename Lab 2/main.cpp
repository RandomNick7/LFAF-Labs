#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "finite_automaton.hpp"
#include "grammar.hpp"
#include "input.hpp"

int main(){
	srand(time(0));
	int err = 0;
	string buffer;
	
	cout << "Input Grammar or Finite Automaton (G/F)" << endl;
	getline(cin,buffer);
	if(buffer[0] == 'G' || buffer[0] == 'g'){		
		vector<char> V_n, V_t;
		unordered_map<string, vector<string>> P;
		
		cout << "List the non-terminal characters (S is the starting non-terminal!)" << endl;
		cout << "Space separation is optional" << endl;
		err = process_vars(&V_n,0);
		
		if(!err){
			cout << "List the terminal characters" << endl;
			err = process_vars(&V_t,1);
		}
		
		if(!err){
			cout << "Indicate Production Rules (format: \"string->string\")" << endl;
			cout << "Enter a hyphen to continue" << endl;
			process_rules(V_n,V_t,&P);
		
			Grammar* G = new Grammar(V_n,V_t,P);
			
			if(G->type == 4){
				cout << "Grammar is of type 3, regular-right" << endl;
			}else if(G->type == 3){
				cout << "Grammar is of type 3, regular-left" << endl;
			}else{
				cout << "Grammar is of type " << G->type << endl;
			}
			
			if(G->type>=2){
				cout << "Generate 10 random words using the grammar? (Y/N)" << endl;
				cin >> buffer;
				if(buffer[0] == 'Y' || buffer[0] == 'y'){
					int word_count = 10;
					for(int i=0;i<word_count;i++){
						cout << "Word " << i+1 << ": " << G->generate_string() << endl;
					}
				}
				cout << endl;
				
				FiniteAutomaton F = G->convert_to_FA();
				
				F.print();
				
				cout << "Enter strings to check if they belong to the language!" << endl;
				cout << "Hyphen to exit" << endl;
				cin >> buffer;
				while(buffer[0]!='-'){
					if(F.is_valid_word(buffer)){
						cout << "Belongs to language!" << endl;
					}else{
						cout << "Does not belong!" << endl;
					}
					cin >> buffer;
				}
			}
		}
	}else if(buffer[0] == 'F' || buffer[0] == 'f'){
		vector<char> Q,S,E;
		unordered_map<char,vector<pair<char,char>>> T;
		
		cout << "Input the number of states" << endl;
		cout << "They will be automatically labelled from 0" << endl;
		err = process_states(&Q);
		
		if(!err){
			cout << "Specify which states are final with space-separated numbers" << endl;
			err = process_finals(&E,&T,Q);
		}
		
		if(!err){
			cout << "List the alphabet" << endl;
			err = process_vars(&S,1);
		}
		
		if(!err){
			cout << "Indicate Transitions, space for empty string (format: \"number-char-number\")" << endl;
			cout << "Enter a hyphen to continue" << endl;
			process_transitions(Q,S,&T);
			getline(cin,buffer);
			
			FiniteAutomaton* F = new FiniteAutomaton(Q,S,T,E);
			
			F->print();
			
			if(F->determinism != 1){
				cout << "This automaton is non-deterministic, do you want to convert it?(Y/N)" << endl;
				getline(cin,buffer);
				if(buffer[0] == 'y' || buffer[0] == 'Y'){
					F->NFA_to_DFA();
				}
			}
			
			cout << "Do you want to create a Grammar from this Finite Automaton?(Y/N)" << endl;
			getline(cin,buffer);
			if(buffer[0] == 'y' || buffer[0] == 'Y'){
				Grammar* G = new Grammar(F->get_states(), F->get_alphabet(), F->get_Grammar_productions());
				G->print();
			}
		}
		
	}else{
		cout << "Incorrect option! Try again." << endl;
	}
	return err;
}
