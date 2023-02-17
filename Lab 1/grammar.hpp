struct Tr{
	char start;
	string symbols;
	char end;
};

class FiniteAutomaton{
	private:
		vector<char> states;
		vector<char> alphabet;
		vector<Tr> transition;
		char startState = 'S';	//It will always be S, no point in adding it to constructor
		char endState = '0';	//0 will serve as a singular end state with no outgoing transitions for simplicity
	
	public:
		FiniteAutomaton(vector<char> V_n, vector<char> V_t, vector<string> P[2]){
			states = V_n;
			states.push_back('0');
			alphabet = V_t;
			for(int i=0;i<P[0].size();i++){
				char temp = '0';
				for(int j=0;j<P[1][i].size();j++){
					if('A'<=P[1][i][j] && P[1][i][j]<='Z'){
						temp = P[1][i][j];
						P[1][i].erase(j,1);
						break;
					}
				}
				transition.push_back({P[0][i][0],P[1][i],temp});
			}
		}
	
		int is_valid_word(string word){
			char st = startState;
			int i=0;
			while(true){
				int j=0;
				while(j<transition.size()){
					if(st == transition[j].start){
						int match = 0;
						while(i+match<word.size() && word[i+match] == transition[j].symbols[match]){
							match++;
						}
						if(match == transition[j].symbols.size()){
							i+=match;
							st=transition[j].end;
							j=0;
						}else{
							j++;
						}
					}else{
						j++;
					}
				}
				
				if(st != endState || i!=word.size()){
					return 0;
				}else{
					return 1;
				}
			}
		}
};

class Grammar{
	private:
		vector<char> nonTerminalVars;
		vector<char> terminalVars;
		vector<string> prodRules[2];
		char startChar = 'S';		//It will always be S, no point in adding it to constructor
		
	public:
		//Pass the input to object
		Grammar(vector<char> V_n, vector<char> V_t, vector<string> P[2]){
			nonTerminalVars = V_n;
			terminalVars = V_t;
			prodRules[0] = P[0];
			prodRules[1] = P[1];
		}
		
		string generate_string(){
			string word;
			word += startChar;
			int repeat = 1;
			//Repeat as long as there are non-terminal chars
			while(repeat){
				repeat = 0;
				for(int i=0;i<word.size();i++){
					if('A'<=word[i] && word[i]<='Z'){
						//If there is a non-terminal char, check rules
						vector<string> temp;
						repeat = 1;
						for(int j=0;j<prodRules[0].size();j++){
							//Add each possible replacement to a temporary array
							if(prodRules[0][j][0] == word[i]){
								temp.push_back(prodRules[1][j]);
							}
						}
						//Randomly pick one of the viable replacements, if any available.
						if(temp.size()>0){
							word.replace(i,1,temp[rand()%temp.size()]);
							break;
						}else{
							cout << "Missing rule! Partial result:" << endl;
							repeat = 0;
							break;
						}
					}
				}
			}
			return word;
		}
		
		FiniteAutomaton convert(){
			return FiniteAutomaton(nonTerminalVars,terminalVars,prodRules);
		}
};