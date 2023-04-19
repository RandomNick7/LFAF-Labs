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
			
			//Check if any transition includes a non-terminal with a transition to empty string
			//i.e. S->A; A->;
			if(type == 1){
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
			}
			
			//Only remaining cases are grammars where all rules are of type A->B, A->b or A->
			if(type == -1){
				type = 2;
			}
		}
		
		string generate_string(){
			string word;
			word += start_char;
			int repeat = 1, iter = 0;
			//Repeat as long as there are non-terminal chars or until a specific count is reached
			while(repeat && iter < 1000){
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
				iter++;
			}
			if(iter == 500){
				cout << "Max iterations (1000) reached!" << endl;
			}
			return word;
		}
		
		FiniteAutomaton convert_to_FA(){
			return FiniteAutomaton(non_terminal_vars,terminal_vars,prod_rules);
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
		
		void remove_duplicate_prods(){
			for(auto x:prod_rules){
				vector<string> temp = x.second;
				sort(temp.begin(), temp.end());
				temp.erase(unique(temp.begin(),temp.end()), temp.end());
				prod_rules[x.first] = temp;
			}
		}
		
		//Eliminate the unreachable and useless characters from the language
		void delete_delta_vars(string vars, string set){
			size_t search = vars.find_first_not_of(set);
			
			while(search != string::npos){
				string temp(1,vars[search]);
				prod_rules.erase(temp);
				vars.erase(search,1);
				non_terminal_vars.erase(non_terminal_vars.begin()+search);
				search = vars.find_first_not_of(set, search);
			}
			
			//Erasing rules with unreachable/useless productions from the rest of the rules
			for(int i=0; i<vars.size(); i++){
				string left(1,vars[i]);
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					search = rule.find_first_not_of(vars);
					if(search != string::npos){
						if(rule[search]<'a'||'z'<rule[search]){
							prod_rules[left].erase(prod_rules[left].begin()+j);
							j--;
						}
					}
				}
			}
		}
		
		void create_prod_combos(string left, vector<char> arr){
			vector<string> temp;
			for(int j=0; j<prod_rules[left].size(); j++){
				//Check number of occurences for non-terminals with empty productions in the rules
				int occurences = 0;
				for(int k=0; k<arr.size(); k++){
					size_t search = prod_rules[left][j].find(arr[k]);
					while(search != string::npos){
						occurences++;
						search = prod_rules[left][j].find(arr[k], search+1);
					}
				}
				
				//Create every possible rule alteration for those referring to the non-terminals with empty productions
				string eps_set(arr.begin(), arr.end());
				for(int k=0; k < (1<<occurences)-1; k++){
					string new_prod = prod_rules[left][j];
					size_t search = new_prod.find_first_of(eps_set);
					int mask = 1;
					while(search != string::npos){
						int flag = 1;
						if((k & mask) == 0){
							new_prod.erase(search,1);
							flag = 0;
						}
						mask<<=1;
						search = new_prod.find_first_of(eps_set, search+flag);
					}
					if(new_prod.size()>0){
						temp.push_back(new_prod);
					}
				}
			}
			
			for(int j=0; j<temp.size(); j++){
				prod_rules[left].push_back(temp[j]);
			}
		}
		
		void to_Chomsky(){
			//Step 1 - Remove epsilon productions
			//Find Epsilon productions
			string nullables;
			for(int i=0; i<non_terminal_vars.size(); i++){
				string left(1,non_terminal_vars[i]);
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					if(rule.size() == 0){
						nullables+=non_terminal_vars[i];
						break;
					}
				}
			}
			
			//Find all productions leading to epsilon productions
			int curr_size = nullables.size(), last_size = 0;
			while(last_size!=curr_size){
				for(int i=0; i<non_terminal_vars.size(); i++){
					string left(1,non_terminal_vars[i]);
					if(nullables.find(left) == string::npos){
						for(int j=0; j<prod_rules[left].size(); j++){
							string rule = prod_rules[left][j];
							int occurences = 0;
							size_t search = rule.find_first_of(nullables);
							while(search != string::npos){
								occurences++;
								search = rule.find_first_of(nullables,search+1);
							}
							if(occurences == rule.size()){
								nullables+=left;
								break;
							}
						}
					}
				}
				last_size = curr_size;
				curr_size = nullables.size();
			}
			
			//Eliminate the productions and useless variables if needed
			for(int i=0; i<nullables.size(); i++){
				string left(1,nullables[i]);
				if(prod_rules[left].size() == 1){
					if(prod_rules[left][0].size() == 0){
						prod_rules.erase(left);
						for(int j=0;j<non_terminal_vars.size();j++){
							if(non_terminal_vars[j] == left[0]){
								non_terminal_vars.erase(non_terminal_vars.begin()+j);
								break;
							}else{
								string temp(1,non_terminal_vars[j]);
								for(int k=0;k<prod_rules[temp].size();k++){
									string rule = prod_rules[temp][k];
									size_t search = rule.find(left);
									while(search!=string::npos){
										rule.erase(search,1);
										prod_rules[temp][k].erase(search,1);
										search = rule.find(left,search);
									}
								}
							}
						}
					}
				}else{
					for(int j=0; j<prod_rules[left].size();j++){
						if(prod_rules[left][j].size() == 0){
							prod_rules[left].erase(prod_rules[left].begin()+j);
							break;
						}
					}
				}
			}
			
			vector<char> nullables_arr;
			for(int i=0;i<nullables.size();i++){
				nullables_arr.push_back(nullables[i]);
			}
			
			for(int i=0; i<non_terminal_vars.size(); i++){
				string left(1,non_terminal_vars[i]);
				create_prod_combos(left, nullables_arr);
			}
			remove_duplicate_prods();
			
			//Step 2 - Remove unit productions
			for(int i=0; i<non_terminal_vars.size(); i++){
				string left(1,non_terminal_vars[i]);
				string visited(1,non_terminal_vars[i]);
				//Transfer productions from variable to variable according to unit productions
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					if(rule.size()==1 && 'A'<=rule[0] && rule[0]<='Z'){
						if(rule.find_first_of(visited) == string::npos){
							visited+=rule;
							for(int k=0; k<prod_rules[rule].size(); k++){
								prod_rules[left].push_back(prod_rules[rule][k]);
							}
						}
					}
				}
				//Eliminate unit productions
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					if(rule.size()==1 && 'A'<=rule[0] && rule[0]<='Z'){
						prod_rules[left].erase(prod_rules[left].begin()+j);
						j--;
					}
				}
			}
			remove_duplicate_prods();
			
			//Step 3 - Eliminate inaccessible symbols
			string accessed(1,'S');
			string V_n;
			for(int i=0; i<non_terminal_vars.size();i++){
				V_n+=non_terminal_vars[i];
			}
			
			//Check the rules from accessible vars for non-terminals, S is accessible by default
			for(int i=0; i<accessed.size(); i++){
				string left(1,accessed[i]);
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					size_t search = rule.find_first_of(V_n);
					while(search != string::npos){
						if(rule.find_first_of(accessed) == string::npos && accessed.find(rule[search]) == string::npos){
							accessed+=rule[search];
						}
						search = rule.find_first_of(V_n, search+1);
					}
				}
			}
			delete_delta_vars(V_n,accessed);
			
			//Step 4 - Eliminate useless productions
			string useful;
			V_n.clear();
			for(int i=0; i<non_terminal_vars.size();i++){
				V_n+=non_terminal_vars[i];
			}
			
			//Check all rules for the ones that contain only non-terminals - it's useful then
			for(int i=0; i<non_terminal_vars.size(); i++){
				string left(1,non_terminal_vars[i]);
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					if(rule.find_first_of(V_n) == string::npos){
						useful+=left[0];
						break;
					}
				}
			}
			
			//Repeatedly check all rules that derive into useful non-terminals
			curr_size = useful.size();
			last_size = 0;
			while(last_size!=curr_size){
				for(int i=0; i<non_terminal_vars.size(); i++){
					string left(1,non_terminal_vars[i]);
					if(useful.find(left) == string::npos){
						for(int j=0; j<prod_rules[left].size(); j++){
							string rule = prod_rules[left][j];
							if(rule.find_first_of(useful) != string::npos){
								useful+=left[0];
								break;
							}
						}
					}
				}
				last_size = curr_size;
				curr_size = useful.size();
			}
			delete_delta_vars(V_n,useful);
			
			//Step 5 - Substitute non-single terminal symbols with intermediary non-terminals
			V_n.clear();
			for(int i=0; i<non_terminal_vars.size();i++){
				V_n+=non_terminal_vars[i];
			}
			
			string V_t;
			for(int i=0; i<non_terminal_vars.size(); i++){
				string left(1,non_terminal_vars[i]);
				for(int j=0; j<prod_rules[left].size(); j++){
					string rule = prod_rules[left][j];
					if(rule.find_first_of(V_n)!=string::npos){
						size_t search = rule.find_first_not_of(V_n);
						while(search!=string::npos){
							if(V_t.find(rule[search])==string::npos){
								V_t+=rule[search];
							}
							search = rule.find_first_not_of(V_n,search+1);
						}
					}
				}
			}
			
			int reserved_char[26] = {0};
			bool err = false;
			for(int i=0; i<non_terminal_vars.size(); i++){
				reserved_char[non_terminal_vars[i]-'A']++;
			}
			
			string substitutes;
			int char_index=0;
			for(int i=0;i<V_t.size();i++){
				for(;char_index<26;char_index++){
					if(!reserved_char[char_index]){
						char c = char_index+'A';
						substitutes+=c;
						reserved_char[char_index]++;
						non_terminal_vars.push_back(c);
						
						string left(1,c);
						string rule(1,V_t[i]);
						vector<string> right;
						right.push_back(rule);
						prod_rules[left]=right;
						break;
					}
				}
				if(char_index==26){
					err = true;
					cout << "Not enough characters to reserve for Chomsy Normal Form! Partial result:" << endl;
				}
			}
			
			if(!err){
				for(int i=0; i<non_terminal_vars.size(); i++){
					string left(1,non_terminal_vars[i]);
					for(int j=0; j<prod_rules[left].size(); j++){
						string rule = prod_rules[left][j];
						if(rule.size()>1){
							for(int k=0; k<V_t.size(); k++){
								replace(prod_rules[left][j].begin(), prod_rules[left][j].end(), V_t[k], substitutes[k]);
							}
						}
					}
				}

				//Step 6: Divide productions with more than 2 characters on right side
				vector<string> prods_used;
				string substitutes;
				for(int i=0; i<non_terminal_vars.size() && !err; i++){
					string left(1,non_terminal_vars[i]);
					for(int j=0; j<prod_rules[left].size(); j++){
						string rule = prod_rules[left][j];
						char source = left[0];
						while(rule.size()>2 && !err){
							string temp_rule(rule.begin(), rule.begin()+2);
							int k=0;
							
							for(; k<prods_used.size();k++){
								if(prods_used[k] == temp_rule){
									break;
								}
							}
							
							if(k == prods_used.size()){
								prods_used.push_back(temp_rule);
								if(char_index == 26){
									cout << "Not enough characters to reserve for Chomsy Normal Form! Partial result:" << endl;
									err = true;
									break;
								}else{
									int l = char_index;
									while(reserved_char[l] && l<26){
										l++;
									}
									if(l == 26){										
										cout << "Not enough characters to reserve for Chomsy Normal Form! Partial result:" << endl;
										err = true;
										break;
									}
									reserved_char[l]++;
									char_index = l;
									substitutes+=char_index+'A';
									non_terminal_vars.push_back(char_index+'A');
									string temp(1,substitutes[k]);
									prod_rules[temp].push_back(temp_rule);
								}
							}
							string temp(1,substitutes[k]);
							rule.replace(0,2,temp);
							prod_rules[left][j].replace(0,2,temp);
						}
					}
				}
			}
			this->print();
		}
};