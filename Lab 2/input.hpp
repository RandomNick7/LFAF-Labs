int is_unique(char b, vector<char> vars){	
	int unique = 1;
	for(int i=0;i<vars.size();i++){
		if(b == vars[i]){
			unique = 0;
		} 
	}
	return unique;
}

void filter_numbers(string* buffer){
	int i=0;
	while(i<buffer->size()){
		if((*buffer)[i]<'0' || '9'<(*buffer)[i]){
			buffer->erase(i,1);
		}else{
			i++;
		}
	}
}

void filter_whitespace(string* buffer){
	int i=0;
	while(i<buffer->size()){
		if((*buffer)[i] == ' ' || (*buffer)[i] == '\t'){
			buffer->erase(i,1);
		}else{
			i++;
		}
	}
}

int str_to_int(string buffer){	
	int count = 0;
	for(int i=buffer.size()-1,d=1;i>=0;i--,d*=10){
		count+=(buffer[i]-'0')*d;
	}
	return count;
}

int process_vars(vector<char>* vars, int terminal){
	int i=0;
	string buffer;
	
	getline(cin,buffer);
	filter_whitespace(&buffer);
	
	if(terminal){
		for(int i=0;i<buffer.size();i++){
			if(buffer[i]<'A' || buffer[i]>'Z'){
				if(is_unique(buffer[i],*vars)){
					vars->push_back(buffer[i]);
				}
			}else{
				cout << "Use lowercase alphabetical characters, digits or symbols!" << endl;
				return 1;
			}
		}
	}else{
		for(int i=0;i<buffer.size();i++){
			if('A'<= buffer[i] && buffer[i]<='Z'){
				if(is_unique(buffer[i],*vars)){
					vars->push_back(buffer[i]);
				}
			}else{
				cout << "Use uppercase alphabetical characters only!" << endl;
				return 1;
			}
		}
	}
	return 0;
}

int process_states(vector<char>* Q){
	int i=0;
	string buffer;
	getline(cin,buffer);
	
	filter_numbers(&buffer);
	
	if(buffer.size() == 0){
		cout << "No numbers found!" << endl;
		return 1;
	}
	
	int state_num = str_to_int(buffer);
	if(state_num>26){
		cout << "Only up to 26 states are supported right now, sorry!" << endl;
		return 1;
	}
	
	for(int i=0;i<state_num;i++){
		Q->push_back(i+'A');
	}
	return 0;
};

int process_finals(vector<char>* F, unordered_map<char,vector<pair<char,char>>>* T, vector<char> Q){
	string buffer;
	getline(cin,buffer);
	if(buffer.size() == 0){
		cout << "You can't have no exit states!"<< endl;
		return 1;
	}
	
	while(buffer.front()==' '){
		buffer.erase(0,1);
	}
	while(buffer.back()==' '){
		buffer.erase(buffer.size()-1,1);
	}
	
	for(int i=2;i<buffer.size();i++){
		if(buffer[i]==' '){
			if(buffer[i-1]==' '){
				buffer.erase(i,1);
			}
		}
	}
	
	int i = 0, num;
	string temp;
	while(i<buffer.size()){
		if(buffer[i]==' '){
			filter_numbers(&temp);
			num = str_to_int(temp);
			if(num<Q.size()){
				F->push_back('A'+num);
				pair<char,char> p(' ',' ');
				(*T)['A'+num].push_back(p);
			}else{
				cout << "That state doesn't exist!" << endl;
			}
			temp.clear();
		}else{
			temp+=buffer[i];
		}
		i++;
	}
	filter_numbers(&temp);
	num = str_to_int(temp);
	if(num<Q.size()){
		F->push_back('A'+num);
		pair<char,char> p(' ',' ');
		(*T)['A'+num].push_back(p);
	}else{
		cout << "That state doesn't exist!" << endl;
	}
	
	return 0;
} 

void process_rules(vector<char> V_n,vector<char> V_t, unordered_map<string,vector<string>>* P){
	string buffer;
	while(true){
		getline(cin,buffer,'-');
		int valid = 0,non_term_count = 0;
		if(buffer.size()>0){
			int i=0;
			filter_whitespace(&buffer);
			
			for(int i=0;i<buffer.size();i++){
				valid = 0;
				if('A'<=buffer[i] && buffer[i]<='Z'){
					for(int j=0;j<V_n.size();j++){
						if(buffer[i] == V_n[j]){
							valid = 1;
							non_term_count++;
							break;
						}
					}
				}else{
					for(int j=0;j<V_t.size();j++){
						if(buffer[i] == V_t[j]){
							valid = 1;
							break;
						}
					}
				}
				if(valid == 0){
					break;
				}
			}
			if(valid == 0 || non_term_count == 0){
				cout << "Illegal Production!" << endl;
				getline(cin,buffer);
				continue;
			}
		}
		
		string left = buffer;
		
		getline(cin,buffer);
		if(buffer[0] == '>'){
			buffer.erase(0,1);
		}
		
		int i=0;
		filter_whitespace(&buffer);
		
		if(buffer.size()>0){
			if(left.size() == 0){				
				cout << "Illegal Production!" << endl;
				continue;
			}
			
			valid = 0;
			for(int i=0;i<buffer.size();i++){
				valid = 0;
				if('A'<=buffer[i] && buffer[i]<='Z'){
					for(int j=0;j<V_n.size();j++){
						if(buffer[i] == V_n[j]){
							valid = 1;
							break;
						}
					}
				}else{
					for(int j=0;j<V_t.size();j++){
						if(buffer[i] == V_t[j]){
							valid = 1;
							break;
						}
					}
				}
				if(valid == 0){
					break;
				}
			}
		}else{
			valid = 1;
			if(left.size()==0){
				break;
			}
		}
		
		if(valid == 1){
			string right = buffer;
			//Add result to map
			if(P->count(left)){
				(*P)[left].push_back(right);
			}else{
				vector<string> temp(1,right);
				P->insert({left,temp});
			}
		}else{
			cout << "Illegal Production!" << endl;
		}
	}
}

void process_transitions(vector<char> Q, vector<char> S, unordered_map<char,vector<pair<char,char>>>* T){
	string buffer;
	char left;
	pair<char,char> right;
	int err = 0;
	
	while(true){
		getline(cin,buffer,'-');
		if(buffer.size() == 0){
			break;
		}
		
		if(buffer[0] == ' '){
			cout << "Transition source can't be empty!" << endl;
			err = 1;
		}
		
		if(!err){
			filter_numbers(&buffer);
			
			if(buffer.size() == 0){
				cout << "No numbers found!" << endl;
				err = 1;
			}else{
				left = 'A'+str_to_int(buffer);
				if(is_unique(left,Q)){
					cout << "Illegal transition!" << endl;
					getline(cin,buffer);
					err = 1;
				}
			}
		}
		
		if(!err){
			getline(cin,buffer,'-');
			filter_whitespace(&buffer);
			
			if(buffer.size() == 0){
				right.first = ' ';
			}else{
				right.first = buffer[0];
				if(is_unique(right.first,S)){
					cout << "Illegal transition!" << endl;
					getline(cin,buffer);
					err = 1;
				}
			}
		}
		
		if(!err){
			getline(cin,buffer);
			filter_numbers(&buffer);
			
			if(buffer.size() == 0){
				cout << "Transition destination can't be empty!" << endl;
				err = 1;
			}else{
				right.second = 'A'+str_to_int(buffer);
				if(is_unique(right.second,Q)){
					cout << "Illegal transition!" << endl;
					getline(cin,buffer);
					err = 1;
				}
			}
			
		}
		
		if(!err){
			if(T->count(left)){
				(*T)[left].push_back(right);
			}else{
				vector<pair<char,char>> temp(1,right);
				T->insert({left,temp});
			}
		}
	}
}