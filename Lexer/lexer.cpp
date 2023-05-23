#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

void remove_space(string* code){
	int i=0, len = code->size();
	
	while(isspace((*code)[i]) && i<len){
		i++;
	}
	code->erase(0,i);
}

void check_type(string* code, vector<pair<string,string>>* lexemes){
	int i=0;
	
	vector<string> type{
		"int","flt","chr","str","bool"
	};
	vector<string> token{
		"TYPE_INT","TYPE_FLOAT","TYPE_CHAR","TYPE_STRING","TYPE_BOOL"
	};
	
	while(isalpha((*code)[i])){
		i++;
	}
	string buffer = code->substr(0,i);
	code->erase(0,i);
	
	for(int i=0;i<type.size();i++){
		if(buffer == type[i]){
			lexemes->push_back({buffer,token[i]});
			return;
		}
	}
	lexemes->push_back({buffer,"IDENTIFIER"});
}

void check_num(string* code, vector<pair<string,string>>* lexemes){
	int i=0;
	string type = "CONST_INT";
	
	while(isdigit((*code)[i])){
		i++;
	}
	if((*code)[i] == '.'){
		i++;
		while(isdigit((*code)[i])){
			i++;
		}
		type = "CONST_FLOAT";
	}
	lexemes->push_back({code->substr(0,i),type});
	code->erase(0,i);
}

void check_keyword(string* code, vector<pair<string,string>>* lexemes, int* exception){
	vector<string> keyword{
		"<~",		//Assign
		"=>",		//If
		"#=",		//Else
		">->",		//While
		"|->",		//For
		">>=>",		//Function Declaration
		"|^|",		//Return Statement
		">]",		//Scan
		"[<",		//Print
		"<~",		//Assign
		"><",		//Equal to
		"<>",		//Different to
		">=",		//Greater or Equal
		"<=",		//Less or Equal
		"%>",		//Block Comment
		"/>"		//Line Comment
	};
	
	vector <char> ops{
		'(',	//Open Parenthesis
		')',	//Closed Parenthesis
		'[',	//Open Braket
		']',	//Closed Braket
		'{',	//Open Brace
		'}',	//Closed Brace
		',',	//Comma Separator
		'+',	//Add
		'-',	//Subtract
		'*',	//Multiply
		'/',	//Divide
		'%',	//Modulo Division
		'&',	//Binary AND
		'|',	//Binary OR
		'^',	//Binary XOR
		'!',	//Binary NOT
		'>',	//Greater than
		'<',	//Less than
		'"',	//Quotation
		'#'		//Array Size
	};
	
	vector<string> token{
		"ASSIGN",
		"IF_COND",
		"ELSE_COND",
		"WHILE_LOOP",
		"FOR_LOOP",
		"FUNCTION_DEF",
		"RETURN",
		"READ",
		"WRITE",
		"OP_ASSGN",
		"COMP_EQ",
		"COMP_NEQ",
		"COMP_GTE",
		"COMP_LTE",
		"COMMENT_BLOCK_L",
		"COMMENT_LINE",
		"SYM_PAREN_L",
		"SYM_PAREN_R",
		"SYM_BRKET_L",
		"SYM_BRKET_R",
		"SYM_BRACE_L",
		"SYM_BRACE_R",
		"SYM_SEP",
		"OP_ADD",
		"OP_SUB",
		"OP_MULT",
		"OP_DIV",
		"OP_MOD",
		"OP_AND",
		"OP_OR",
		"OP_XOR",
		"OP_NOT",
		"COMP_GT",
		"COMP_LT",
		"QUOTATION",
		"ARR_SIZE"
	};
	
	int i=0;
	while(ispunct((*code)[i])){
		i++;
	}
	string buffer = code->substr(0,i);
	
	for(i=0;i<keyword.size();i++){
		if(buffer.rfind(keyword[i],0) != string::npos){
			code->erase(0,keyword[i].size());
			if(keyword.size()-i < 3){
				*exception = keyword.size()-i;
			}else{
				lexemes->push_back({keyword[i],token[i]});
			}
			return;
		}
	}
	
	for(int j=0;j<ops.size();i++,j++){
		if((*code)[0] == ops[j]){
			string temp(1,ops[j]);
			lexemes->push_back({temp,token[i]});
			if(ops[j] == '"'){
				*exception = 3;
			}
			break;
		}
	}
	
	code->erase(0,1);
}

void lexer(string code, vector<pair<string,string>>* lexemes, int* exception){
	int i=0;
	
	while(code.size()>0 && *exception == 0){
		//Depending on what character code stopped at, call one of the follwoing functions:
		lexer_loop:
		char c = code[0];
		if(isalpha(c)){
			check_type(&code,lexemes);
		}else if(isdigit(c)){
			check_num(&code, lexemes);
		}else if(ispunct(c)){
			check_keyword(&code, lexemes, exception);
		}else if(isspace(c)){
			remove_space(&code);
		}else{
			code.erase(0,1);
		}
	}
	
	if(*exception == 1){
		*exception = 0;
	}else if(*exception == 2){
		size_t search = code.find("<%");
		if(search != string::npos){
			code.erase(0,search+2);
			*exception = 0;
		}
	}else{
		size_t search = code.find('"');
		if(search!=string::npos){
			if(search>0){
				lexemes->push_back({code.substr(0,search),"CONST_STR"});
			}
			lexemes->push_back({"\"","QUOTATION"});
			code.erase(0,search+1);
			*exception = 0;
			if(code.size()>0){
				goto lexer_loop;
			}
		}
	}
}

int main(int argc, char* argv[]){
	ifstream source_code;
	vector<pair<string,string>> lexemes;
	string src_line;
	int exception = 0;
	
	if(argc==1){
		cout << "Input file path:\n";
		string path;
		cin >> path;
		source_code.open(path);
	}else{
		source_code.open(argv[1]);
	}
	
	if(source_code.is_open()){
		while(getline(source_code,src_line)){
			lexer(src_line,&lexemes, &exception);
		}
		
		ofstream out_file;
		out_file.open("../Parser/token_list.txt");

		for(int i=0;i<lexemes.size();i++){
			out_file << lexemes[i].second << " \"" << lexemes[i].first << "\"" << endl;
		}
		out_file.close();
	}else{
		cout << "File not found!" << endl;
	}
	return 0;
}