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

		void shift_char(char* ch){
			if(*ch=='A'){
				*ch = 'S';
			}else if(*ch<='S' && *ch!=' '){
				(*ch)--;
			}
		}
		
		//Determinism = 1 -> DFA
		//Determinism = 0 -> NFA
		//Determinism = -1 -> E-NFA
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
				cout << "This Finite Automaton is non-deterministic!" << endl;
			}
		}
		
		vector<char> get_states(){
			return states;
		}
		
		vector<char> get_final_states(){
			return end_states;
		}
		
		vector<char> get_alphabet(){
			return alphabet;
		}
		
		unordered_map<char,vector<pair<char,char>>> get_transitions(){
			return transition;
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
				if((*visited)[next_state-'A']==0 && connections[i].first == ' ' && next_state!=' '){
					DFS(next_state,visited,e_closure);
				}
			}
		}
		
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
			
			vector<vector<vector<char>>> state_table;
			vector<vector<char>> temp;
			temp.push_back(e_closure['A']);
			for(int i=0;i<alphabet.size();i++){
				temp.push_back(vector<char>(0));
			}
			state_table.push_back(temp);
			
			int row=0;
			while(state_table.size()<=26 && row!=state_table.size()){
				for(int j=0;j<alphabet.size();j++){
					//Find all states reachable through a character, followed by eps-transitions
					vector<char> pending_state;
					for(int k=0;k<state_table[row][0].size();k++){
						vector<pair<char,char>> connections = transition[state_table[row][0][k]];
						for(int x=0;x<connections.size();x++){
							if(connections[x].first == alphabet[j]){
								pending_state.insert(pending_state.end(),e_closure[connections[x].second].begin(),e_closure[connections[x].second].end());
							}
						}
					}
					
					//Clear the pending state of repeating elements
					vector<char> unique_states;
					int x=0;
					while(x<pending_state.size()){
						int erased = 0;
						for(int y=0;y<unique_states.size();y++){
							if(pending_state[x] == unique_states[y]){
								pending_state.erase(pending_state.begin()+x);
								erased = 1;
							}
						}
						if(!erased){
							unique_states.push_back(pending_state[x]);
							x++;
						}
					}
					
					//Sort the remaining characters
					sort(pending_state.begin(),pending_state.end());
					
					state_table[row][j+1] = pending_state;
					
					if(pending_state.size() > 0){
						//If the state set has been analysed before, don't add it
						int unique = 1;
						for(int i=0;i<=row;i++){
							if(state_table[i][0] == pending_state){
								unique = 0;
							}
						}
						
						//If the state is new, add it to the bottom of our table in a new row
						if(unique){
							vector<vector<char>> temp;
							temp.push_back(pending_state);
							for(int i=0;i<alphabet.size();i++){
								temp.push_back(vector<char>(0));
							}
							state_table.push_back(temp);
						}
					}
				}
				row++;
			}
			
			if(state_table.size()>26){
				cout << "The resulting conversion would have more than 26 states, not enough letters to label them all!" << endl;
			}else{
				//Map our new states to single characters
				vector<vector<char>> new_state_arr;
				for(int i=0;i<state_table.size();i++){
					new_state_arr.push_back(state_table[i][0]);
				}
				
				//Replace our final states
				vector<char> temp;
				char index;
				for(int i=0;i<state_table.size();i++){
					int terminal = 0;
					for(int k=0;k<state_table[i][0].size() && terminal == 0;k++){
						for(int x=0;x<end_states.size();x++){
							if(state_table[i][0][k] == end_states[x]){
								index = i+'A';
								shift_char(&index);
								temp.push_back(index);
								terminal = 1;
								break;
							}
						}
					}
				}
				end_states = temp;
				
				states.clear();
				transition.clear();
				
				//Replace the states and transitions accordingly.
				for(int i=0;i<state_table.size();i++){
					for(int j=0;j<alphabet.size();j++){
						if(state_table[i][j+1].size()>0){
							pair<char,char> temp;
							temp.first = alphabet[j];
							for(int k=0;k<new_state_arr.size();k++){
								if(state_table[i][j+1] == new_state_arr[k]){
									index = k+'A';
									shift_char(&index);
									temp.second = index;
									break;
								}
							}
							index = i+'A';
							shift_char(&index);
							transition[index].push_back(temp);
						}
					}
					index = i+'A';
					shift_char(&index);
					states.push_back(index);
				}
				
				//Add our hidden exit transitions for the grammar conversion and word-checking
				for(int i=0;i<end_states.size();i++){
					pair<char,char> temp(' ',' ');
					transition[end_states[i]].push_back(temp);
				}
				determinism = 1;
				
				print();
			}
		}
		
		void relabel(){
			for(int i=0;i<states.size();i++){
				shift_char(&states[i]);
			}
			
			for(int i=0;i<end_states.size();i++){
				shift_char(&end_states[i]);
			}
			
			unordered_map<char,vector<pair<char,char>>> temp;
			for(auto x:transition){
				char ch = x.first;
				shift_char(&ch);
				for(int i=0;i<x.second.size();i++){
					shift_char(&x.second[i].second);
				}
				temp[ch]=x.second;
			}
			transition = temp;
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
			cout << endl;
		}
};