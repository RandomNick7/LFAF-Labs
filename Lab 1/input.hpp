int is_unique(char b, vector<char> vars){	
	int unique = 1;
	for(int i=0;i<vars.size();i++){
		if(b == vars[i]){
			unique = 0;
		} 
	}
	return unique;
}

int process_vars(string buffer, vector<char>* vars, int terminal){
	int i = 0;
	while(i<buffer.size() && buffer[i] == ' '){
		i++;
	}
	
	int prev_space = 1;
	for(;i<buffer.size();i++){
		if(prev_space){
			if(buffer[i] == ' ' || buffer[i] == ','){
				continue;
			}else if(!terminal){
				if(buffer[i]>='A'&& buffer[i]<='Z'){
					if(is_unique(buffer[i],*vars)){
						vars->push_back(buffer[i]);
						prev_space = 0;
					}
				}else{
					cout << "Use uppercase alphabetical characters only!" << endl;
					return 1;
				}
			}else{
				if(buffer[i]>='a'&& buffer[i]<='z'){
					if(is_unique(buffer[i],*vars)){
						vars->push_back(buffer[i]);
						prev_space = 0;
					}
				}else{
					cout << "Use lowercase alphabetical characters only!" << endl;
					return 1;
				}
			}
		}else{
			if(buffer[i] == ' ' || buffer[i] == ','){
				prev_space = 1;
			}else{
				cout << "Use spaces to delimit your variables!" << endl;
				return 1;
			}
		}
	}
}

void process_rules(vector<char> V_n,vector<char> V_t,vector<string> P[2],string buffer){
	//"Empty string" supported! Something like "A->" would work.
	while(true){
		getline(cin,buffer,'-');
		int i = 0;
		while(i<buffer.size() && buffer[i] == ' '){
			i++;
		}
		
		char initial = buffer[i];
		int valid = 0;
		for(int i=0;i<V_n.size();i++){
			if(V_n[i]==initial){
				valid = 1;
				break;
			}
		}
		
		getline(cin,buffer);
		buffer.erase(0,1);
		for(int i=0;i<buffer.size();i++){
			if(buffer[i] == ' '){
				buffer.erase(i,1);
			}
		}
		
		if(valid){
			while(buffer.back() == ' '){
				buffer.pop_back();
			}
			P[1].push_back(buffer);
			buffer = initial;
			P[0].push_back(buffer);
		}else{
			if(buffer.size() == 0){
				break;
			}else{
				cout << "Illegal transition!" << endl;
			}
		}
	}
}