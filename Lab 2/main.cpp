#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "finite_automaton.hpp"
#include "grammar.hpp"
#include "input.hpp"

int main(int argc, char **argv){
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
			
			G->print();
			
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
				}else{
					F->relabel();
				}
			}
			
			cout << "Do you want to create a Grammar from this Finite Automaton?(Y/N)" << endl;
			getline(cin,buffer);
			if(buffer[0] == 'y' || buffer[0] == 'Y'){
				Grammar* G = new Grammar(F->get_states(), F->get_alphabet(), F->get_Grammar_productions());
				G->print();
			}
			
			cout << "Would you like to see a graphical representation of the automaton?(Y/N)" << endl;
			getline(cin,buffer);
			if(buffer[0] == 'y' || buffer[0] == 'Y'){
				//Write all the data to a file, embedding Python in C/C++ is more complicated and time is already short...
				FILE* py_in;
				py_in = fopen("temp.txt","w");
				
				vector<char> temp_states = F->get_states();
				vector<char> temp_finals = F->get_final_states();
				
				for(int i=0;i<temp_finals.size();i++){
					temp_states.erase(remove(temp_states.begin(),temp_states.end(),temp_finals[i]));
				}
				
				for(int i=0;i<temp_states.size();i++){
					fprintf(py_in,"%c ",temp_states[i]);
				}
				fprintf(py_in,"\n");
				
				for(int i=0;i<temp_finals.size();i++){
					fprintf(py_in,"%c ",temp_finals[i]);
				}
				fprintf(py_in,"\n");
				
				unordered_map<char,vector<pair<char,char>>> temp_transitions = F->get_transitions();
				for(auto x:temp_transitions){
					fprintf(py_in,"|%c",x.first);
					for(int i=0;i<x.second.size();i++){
						if(x.second[i].second!=' '){
							fprintf(py_in,"%c%c",x.second[i].first,x.second[i].second);
						}
					}
				}
				fclose(py_in);
				
				//Launch the graphing script in Python, delete the temporary file
				//I would have done all of this in C++, but graphviz refused to collaborate
				system("python graph.py");
				remove("temp.txt");
			}
		}
		
	}else{
		cout << "Incorrect option! Try again." << endl;
	}
	return err;
}

/* Lab 2 test case "Easy"
F
3
2
ab
0-a-1
0-a-0
1-b-2
0-b-0
1-b-1
2-b-2
-

"Medium"
F
8
5 7
ab
0- -1
1- -2
2-a-3
3- -4
4-b-5
5- -1
1- -6
6-a-7
7- -1
-

"Hard"
F
6
4 5
ab
0- -1
1-a-1
1- -2
2-b-2
2- -0
2- -5
1-a-3
3- -5
3-b-4
4-a-5
-
*/