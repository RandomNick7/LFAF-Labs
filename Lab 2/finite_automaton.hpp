class FiniteAutomaton{
	private:
		vector<char> states;
		vector<char> alphabet;
		unordered_map<char,vector<pair<char,char>>> transition;
		char start_state = 'S';	//It will always be S, no point in adding it to constructor
		vector<char> end_states;
		int orientation = 1;
	
	public:
		int determinism = 1;
		//Constructor used by Grammar
		FiniteAutomaton(vector<char> V_n, vector<char> V_t, unordered_map<string, vector<string>> P){
			states = V_n;
			alphabet = V_t;
			for(auto x:P){
				for(int i=0;i<x.second.size();i++){
					pair<char,char> temp;
					char left = x.first[0];
					string right = x.second[i];
					
					if(right.size()>1){
						if('A'<=right[0] && right[0]<='Z'){
							orientation = -1;
							temp.first = right[1];
							temp.second = right[0];
						}else{
							temp.first = right[0];
							temp.second = right[1];
						}
					}else{
						if('A'<=right[0] && right[0]<='Z'){
							temp.first = ' ';
							temp.second = right[0];
						}else{
							temp.first = right[0];
							temp.second = ' ';
							end_states.push_back(left);
						}
					}
					
					if(transition.count(left)){
						transition[left].push_back(temp);
					}else{
						vector<pair<char,char>> t(1,temp);
						transition.insert({left,t});
					}
				}
			}
			check_determinism();
		}
		
		FiniteAutomaton(vector<char> Q, vector<char> S, unordered_map<char,vector<pair<char,char>>> T, vector<char> F){
			states = Q;
			alphabet = S;
			transition = T;
			end_states = F;
			check_determinism();
			//Orientation will stay right-regular for convenience.
		}
		
		void check_determinism(){
			for(auto x:transition){
				vector<pair<char,char>> y = x.second;
				vector<char> temp;
				for(int i=0;i<y.size();i++){
					if(y[i].first == ' '){
						determinism = -1;
						break;
					}
					
					if(determinism == 1){
						for(int j=0;j<temp.size();j++){
							if(y[i].second == temp[j]){
								determinism = 0;
								break;
							}
						}
					}
					
					temp.push_back(y[i].second);
				}
				if(determinism == -1){
					break;
				}
			}
		}
		
		int is_valid_word(string word){
			if(determinism == 1){
				char curr_state = start_state;
				int i = 0, end = 0;
				if(orientation == -1){
					i=word.size()-1;
				}
				
				while(true){
					if(transition.count(curr_state)){
						vector<pair<char,char>> moves = transition.at(curr_state);
						int t=0;
						for(int j=0;j<moves.size() && !end;j++){
							if(moves[j].first == word[i]){
								t=1;
								curr_state=moves[j].second;
								i+=orientation;
								if(curr_state == ' '){
									end = 1;
								}
								break;
							}
						}
						if(end || t==0){
							break;
						}
					}else{
						break;
					}
				}
				
				if(!end){
					return 0;
				}
				
				if(orientation == -1 && i!=-1){
					return 0;
				}else if(orientation == 1 && i!=word.size()){
					return 0;
				}else{
					return 1;
				}
			}else{
				string buffer;
				cout << "This Finite Automaton is non-deterministic, do you want to convert it?(Y/N)" << endl;
				getline(cin,buffer);
				if(buffer[0] == 'y' || buffer[0] == 'Y'){
					NFA_to_DFA();
				}
			}
		}
		
		vector<char> get_states(){
			return states;
		}
		
		vector<char> get_alphabet(){
			return alphabet;
		}
		
		unordered_map<string, vector<string>> get_Grammar_productions(){
			unordered_map<string, vector<string>> P;
			for(auto x:transition){
				string left;
				left = x.first;
				vector<pair<char,char>> right = x.second;
				for(int i=0;i<right.size();i++){
					string temp;
					if(right[i].first!=' '){
						temp+=right[i].first;
					}
					if(right[i].second!=' '){
						temp+=right[i].second;
					}
					P[left].push_back(temp);
				}
			}
			return P;
		};
		
		void DFS(char curr_state, vector<int>* visited, unordered_map<char,vector<char>>* e_closure){
			vector<pair<char,char>> connections = transition[curr_state];
			(*visited)[curr_state-'A'] = 1;
			for(int i=0;i<connections.size();i++){
				char next_state = connections[i].second;
				//cout << curr_state << connections[i].first << next_state << endl;
				if((*visited)[next_state-'A']==0 && connections[i].first == ' ' && next_state!=' '){
					DFS(next_state,visited,e_closure);
				}
			}
			
			/*for(int i=0;i<visited->size();i++){
				cout << (*visited)[i] << " ";
			}
			cout << endl;*/
			
			/*if((*visited)[curr_state-'A']!=1){
				for(int i=0;i<connections.size();i++){
					char next_state = connections[i].second;
					if((*visited)[i] == 0 && connections[i].first == ' ' && next_state!=' '){
						cout << endl << curr_state << "-" << connections[i].first << "-" << connections[i].second <<" ";
						DFS(next_state,visited,e_closure);
						(*visited)[next_state-'A'] = 1;
					}
				}
			}
			cout << endl;*/
		}
		
		//TODO: Finish.
		void NFA_to_DFA(){
			//Create Epsilon-closures for all states
			unordered_map<char,vector<char>> e_closure;
			for(int i=0;i<states.size();i++){
				vector<int> temp(states.size(),0);
				DFS('A'+i,&temp,&e_closure);
				for(int j=0;j<temp.size();j++){
					if(temp[j] == 1){
						e_closure[i+'A'].push_back(j+'A');
					}
				}
			}
			
			for(int i=0;i<e_closure.size();i++){
				cout << i << ":";
				for(int j=0;j<e_closure[i+'A'].size();j++){
					cout << " "<< e_closure[i+'A'][j];
				}
				cout << endl;
			}
		}
		
		void print(){
			int i;
			switch(determinism){
				case -1:
					cout << "<e>-NFA";
					break;
				case 0:
					cout << "NFA";
					break;
				case 1:
					cout << "DFA";
					break;
			}
			cout << endl;
			
			cout << "Q = {";
			for(i=0;i<states.size();i++){
				cout << states[i] << " ";
			}
			if(i>0){
				cout << "\b";
			}
			cout << "}" << endl;
			
			cout << "S = {";
			for(i=0;i<alphabet.size();i++){
				cout << alphabet[i] << " ";
			}
			if(i>0){
				cout << "\b";
			}
			cout << "}" << endl;
			
			cout << "F = {";
			for(i=0;i<end_states.size();i++){
				cout << end_states[i] << " ";
			}
			if(i>0){
				cout << "\b";
			}
			cout << "}" << endl;
			
			for(auto x:transition){
				for(int i=0;i<x.second.size();i++){
					if(x.second[i].second!=' '){
						if(x.second[i].first!=' '){
							cout << "D(" << x.first << "," << x.second[i].first << ") = " << x.second[i].second << endl;
						}else{
							cout << "D(" << x.first << ",<e>) = " << x.second[i].second << endl;
						}
					}
				}
			}
		}
};