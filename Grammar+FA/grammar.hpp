class Grammar{
	private:
		vector<char> non_terminal_vars;
		vector<char> terminal_vars;
		unordered_map<string, vector<string>> prod_rules;
		char start_char = 'S';		//It will always be S, no point in adding it to constructor
		
	public:
		int type = -1;
		Grammar(vector<char> V_n, vector<char> V_t, unordered_map<string, vector<string>> P){
			non_terminal_vars = V_n;
			terminal_vars = V_t;
			prod_rules = P;
			determine_type();
		}
		
		//"Type 4" grammar = right-regular grammar
		//"Type 3" grammar = left-regular grammar
		//"Type -1" stands for "Undetermined grammar. If -1 is outputted by this function, case was not covered.
		void determine_type(){
			//Assess type of grammar according to input rule
			vector<char> e_set;
			for(auto x:prod_rules){
				string left = x.first;
				for(int i=0;i<x.second.size();i++){
					string right = x.second[i];
					if(type!=0){
						if(type!=1){
							if(type!=2){
								if(type==-1){
									if(left.size()>1){
										type = 0;
									}else if(right.size()>1){
										if('A'<=right[0] && right[0]<='Z'){
											type = 3;
										}else{
											type = 4;
										}
									}else{
										type = -1;
									}
								}
								
								if(right.size()>2){
									type = 2;
								}else if(right.size()==2){						
									if(type == 3 && (('A'<=right[1] && right[1]<='Z') || (right[0]<'A' || 'Z'<right[0]))){
										type = 2;
									}
									if(type == 4 && (('A'<=right[0] && right[0]<='Z') || (right[1]<'A' || 'Z'<right[1]))){
										type = 2;
									}
								}
							}
							
							if(left.size()>1){
								type = 1;
							}
						}
						
						if(left.size()>right.size() && right.size()>0){
							type = 0;
						}
						
						if(left.size()>1){
							int i=0, j=0, k=0;
							while(left[i] == right[j] && i<left.size() && j<right.size()){
								i++;
								j++;
							}
							if(i != left.size()){
								k=i;
								i = left.size()-1; j = right.size()-1;
								while(left[i] == right[j] && i>0 && j>0){
									i--;
									j--;
								}
								if(i > k){
									type = 0;
								}else{
									type = 1;
								}
							}
						}
					
						if(right.size() == 0){
							if(left.size()>1){
								type = 0;
							}else{
								e_set.push_back(left[0]);
							}
						}
					}
				}
			}
			
			//Check if any transition includes a non-terminal with a trnsition to empty string
			//i.e. S->A; A->;
			for(int i=0;i<e_set.size() && type!=0;i++){
				for(auto x:prod_rules){
					for(int j=0;j<x.second.size() && type!=0;j++){
						string right = x.second[j];
						for(int k=0;k<right.size();k++){
							if(right[k] == e_set[i]){
								type = 0;
								break;
							}
						}
					}
					if(type == 0){
						break;
					}
				}
			}
			
			//Only remaining cases are grammars where all rules are of type A->B, A->b or A->
			if(type == -1){
				type = 2;
			}
		}
		
		string generate_string(){
			string word;
			word += start_char;
			int repeat = 1;
			//Repeat as long as there are non-terminal chars
			while(repeat){
				repeat = 0;
				for(int i=0;i<word.size();i++){
					if('A'<=word[i] && word[i]<='Z'){
						//If there is a non-terminal char, check rules
						string ch;
						ch+=word[i];
						repeat = 1;
						if(prod_rules.count(ch)){
							vector<string> temp = prod_rules.at(ch);
							repeat = 1;
							//Randomly pick one of the viable replacements, if any available.
							if(temp.size()>0){
								word.replace(i,1,temp[rand()%temp.size()]);
								break;
							}
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
		
		FiniteAutomaton convert_to_FA(){
			if(type>2){
				return FiniteAutomaton(non_terminal_vars,terminal_vars,prod_rules);
			}else{
				cout << "Grammar type is " << type << ", incompatible for a Finite Automaton." << endl;
			}
		}
		
		void print(){
			int i;
			
			cout << "Vn = {";
			for(i=0;i<non_terminal_vars.size();i++){
				cout << non_terminal_vars[i] << " ";
			}
			if(i>0){
				cout << "\b";
			}
			cout << "}" << endl;
			
			cout << "Vt = {";
			for(i=0;i<terminal_vars.size();i++){
				cout << terminal_vars[i] << " ";
			}
			if(i>0){
				cout << "\b";
			}
			cout << "}" << endl;
			
			cout << "P = {";
			for(auto x:prod_rules){
				cout << endl << "\t" << x.first << "->";
				for(int i=0;i<x.second.size();i++){
					if(x.second[i].size()>0){
						cout << x.second[i];
					}else{
						cout << "<e>";
					}
					cout << "|";
				}
				if(i == 0){
					cout << "<e>";
				}else{
					cout << "\b";
				}
			}
			if(prod_rules.size()>0){
				cout << endl;
			}
			cout << "}" << endl;
		}
};
