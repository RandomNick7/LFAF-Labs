#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>

//Simple Example:  =>20-10><10{int kek <~ 10} WORKS
//TODO: Check for ignorable strings: escape char \		quote " and '	Comment, single and multi 

using namespace std;
int line=1, col=1;

void show_error(string s, string* code){
	cout << "Error at line " << line << ", column " << col << ": " << s << endl;
	cout << "Here -> " << *code << endl;
}

int determine_num(string* code, vector<pair<string,string>>* lexemes){
	int i=0;
	while(isdigit((*code)[i])){
		i++;
	}
	col+=i;
	
	if(i==0){
		if((*code)[i]=='-'){
			show_error("No negative numbers as indices allowed!",code);
		}else{
			show_error("Missing number",code);
		}
		return 1;
	}
	
	if((*code)[i]=='.'){
		i++;
		while(isdigit((*code)[i])){
			i++;
		}
		col+=i;
		
		if((*code)[i-1]=='.'){
			show_error("No digits after decimal point found",code);
			return 1;
		}
		lexemes->push_back({code->substr(0,i),"CONST_FLOAT"});
	}else{
		lexemes->push_back({code->substr(0,i),"CONST_INT"});
	}
	code->erase(0,i);
	
	//If there is a negative sign in front and there is no number previous to that
	//Then this number is simply negative, no need for negative sign token
	if(lexemes->size()>2){
		if(	lexemes->end()[-2].second.compare("OP_SUB") == 0 &&
			lexemes->end()[-3].second.compare("CONST_INT") != 0 &&
			lexemes->end()[-3].second.compare("CONST_FLOAT") != 0){
				lexemes->erase(lexemes->end()-2);
				lexemes->back().first.insert(0,"-");
		}
	}else if(lexemes->size()==2){
		if(lexemes->end()[-2].second.compare("OP_SUB") == 0){
			lexemes->erase(lexemes->end()-2);
			lexemes->back().first.insert(0,"-");
		}
	}
	return 0;
}

int determine_id(string* code, vector<pair<string,string>>* lexemes){
	if(isalpha((*code)[0])){
		int i = 1;
		while(isalnum((*code)[i]) || (*code)[i] == '_'){
			i++;
		}
		lexemes->push_back({code->substr(0,i),"IDENTIFIER"});
		//Eliminate whitespaces until next token
		while(isspace((*code)[i])){
			i++;
		}
		code->erase(0,i);
		return 0;
	}else{
		show_error("Variables can't start with a non-letter",code);
		return 1;
	}
}

int determine_type(string* code, vector<pair<string,string>>* lexemes){
	string type[]{
		"int","flt","chr","str","bool"
	};
	string token[]{
		"TYPE_INT","TYPE_FLOAT","TYPE_CHAR","TYPE_STRING","TYPE_BOOL"
	};
	
	int i=0, t, types_len = end(type)-begin(type);
	
	while(isalpha((*code)[i])){
		i++;
	}
	col+=i;
	
	bool is_type = false;
	string temp = code->substr(0,i);
	for(t=0; t < types_len && !is_type; t++){
		is_type = (temp.compare(type[t])==0);
	}
	
	if(is_type){
		t--;
		code->erase(0,i);
		lexemes->push_back({type[t],token[t]});
		
		//If identifier has '#' after it, it is an array instead. Repeat.
		int dimensions = 1;
		while((*code)[0]=='#'){
			code->erase(0,1);
			col++;
			
			int err = determine_num(code,lexemes);
			if(err == 0){
				if(lexemes->back().second.compare("CONST_INT") == 0){
					//Append "ARR" to the value of the second to last lexeme for each dimension
					lexemes->end()[-1-dimensions].second.insert(0,"ARR_");
				}else{
					show_error("Floats cannot determine array sizes",code);
					return 1;
				}
			}else{
				return err;
			}
			dimensions++;
		}
		return 0;
	}else{
		//Not a type, check if valid word instead
		return determine_id(code,lexemes);
	}
}

int determine_symbol(string* code, vector<pair<string,string>>* lexemes){
	char ops[]{
		'+',	//Add
		'-',	//Subtract
		'*',	//Multiply
		'/',	//Divide
		'%',	//Modulo Division
		'&',	//Binary AND
		'^',	//Binary OR
		'!',	//Binary NOT
		'>',	//Greater than
		'<',	//Less than
		'\\',	//Escape
		',',	//Comma Separator
		'(',	//Open Parenthesis
		')',	//Closed Parenthesis
		'[',	//Open Braket
		']',	//Closed Braket
		'{',	//Open Brace
		'}'		//Closed Brace
	};
	
	string compound_ops[]{
		"<~",	//Assign
		"><",	//Equal to
		"<>",	//Different to
		">=",	//Greater or Equal
		"<="	//Less or Equal
	};
	
	string token[]{
		"OP_ADD",
		"OP_SUB",
		"OP_MULT",
		"OP_DIV",
		"OP_MOD",
		"OP_AND",
		"OP_OR",
		"OP_NOT",
		"OP_GT",
		"OP_LT",
		"SYM_ESC",
		"SYM_SEP",
		"SYM_PAREN_L",
		"SYM_PAREN_R",
		"SYM_BRKET_L",
		"SYM_BRKET_R",
		"SYM_BRACE_L",
		"SYM_BRACE_R",
		"OP_ASSGN",
		"OP_EQ",
		"OP_NEQ",
		"OP_GTE",
		"OP_LTE"
	};
	
	int i=0, o, op_count = end(ops)-begin(ops), 
		c_op_count = end(compound_ops)-begin(compound_ops);
	
	char special[]{'-',',','#','\"','(',')','[',']','{','}'};
	bool is_special = false;
	//Do not count above symbols behind other chars

	while(ispunct((*code)[i])){
		if(i>0){
			for(int j=0;j<end(special)-begin(special) && !is_special;j++){
				is_special = (*code)[i]==special[j];
			}
			if(is_special){
				break;
			}
		}
		i++;
	}
	
	string temp = code->substr(0,i);
	
	bool is_op = false;
	if(temp.size()==1){
		for(o=0; o<op_count && !is_op; o++){
			is_op = (temp[0]==ops[o]);
		}
		if(is_op){
			o--;
			code->erase(0,i);
			lexemes->push_back({temp,token[o]});
		}else{
			show_error("Unrecognized symbol!",code);
			return 1;
		}
	}else{
		for(o=0; o<c_op_count && !is_op; o++){
			is_op = (temp.compare(compound_ops[o]) == 0);
		}
		if(is_op){
			o--;
			code->erase(0,i);
			lexemes->push_back({temp,token[o+op_count]});
		}else{
			show_error("Unrecognized symbols!",code);
			return 1;
		}
	}
	return 0;
}

int determine_keyword(string* code, vector<pair<string,string>>* lexemes){
	string keyword[]{
		"||>",		//Import
		"<~",		//Assign
		"=>",		//If
		"#=",		//Else
		">->",		//While
		"|->",		//For
		">>=>",		//Function Declaration
		"|^|",		//Return Statement
		">]",		//Scan
		"[<"		//Print
	};
	
	string token[]{
		"IMPORT",
		"ASSIGN",
		"IF_COND",
		"ELSE_COND",
		"WHILE_LOOP",
		"FOR_LOOP",
		"FUNCTION_DEF",
		"RETURN",
		"READ",
		"WRITE"
	};
	
	int i=0,k,keyword_len = end(keyword)-begin(keyword);
	while(ispunct((*code)[i])){
		i++;
	}
	
	//Do not consider open brackets after keyword
	if((*code)[i] == '{' || (*code)[i]=='('){
		i--;
	}
	
	bool is_keyword = false;
	string temp = code->substr(0,i);
	for(k=0; k < keyword_len && !is_keyword; k++){
		is_keyword = (temp.compare(keyword[k])==0);
	}
	
	if(is_keyword){
		k--;
		code->erase(0,i);
		lexemes->push_back({keyword[k],token[k]});
		return 0;
	}else{
		//Not a keyword, check if valid symbol instead
		return determine_symbol(code,lexemes);
	}
}

int lexer(string* code, vector<pair<string,string>>* lexemes, string* buffer, int* exception){
	int err = 0;
	while(code->size()>0 && err==0){
		if(*exception == 0){
			int i=0;
			while(isspace((*code)[i])){
				i++;
			}
			code->erase(0,i);
			
			if(code->size()>1){
				if((*code)[0]=='/' && (*code)[1]=='>'){
					lexemes->push_back({"/>","COMMENT_LINE"});
					code->erase(0,2);
					lexemes->push_back({*code,"COMMENT_BODY"});
					break;
				}
				
				if((*code)[0]=='%' && (*code)[1]=='>'){
					lexemes->push_back({"%>","COMMENT_BLOCK_L"});
					code->erase(0,2);
					*exception = 2;
					break;
				}
			}
			
			//If there is a " and it is not escaped...
			if((*code)[0]=='\"' && lexemes->end()[-1].second.compare("SYM_ESC") != 0){
				lexemes->push_back({"\"","QUOTATION"});
				code->erase(0,1);
				*exception = 1;
			}
			if(*exception!=1){
				if(isalpha((*code)[0])){
					err = determine_type(code, lexemes);
				}else if(isdigit((*code)[0])){
					err = determine_num(code, lexemes);
				}else if(ispunct((*code)[0])){
					err = determine_keyword(code, lexemes);
				}
			}
		}
		
		if(*exception == 1){
			int i=0;
			while((*code)[i]!='\"' && i<code->size()){
				i++;
			}
			
			if(i==code->size()){
				show_error("No matching quotation mark found on line!",code);
				return 1;
			}
			
			i++;
			code->erase(0,i);
			lexemes->push_back({code->substr(0,i),"CONST_STRING"});
			lexemes->push_back({"\"","QUOTATION"});
			*exception = 0;
		}
		
		if(*exception == 2){
			int i=1;
			while(((*code)[i-1]!='<' || (*code)[i]!='%') && i<code->size()){
				*buffer += *code;
			}
			if(i!=code->size()){
				lexemes->push_back({*buffer,"COMMENT_BODY"});
				lexemes->push_back({"<%","COMMENT_BLOCK_R"});
				buffer->clear();
				*exception = 0;
			}
			return 0;
		}
	}
	return err;
}

int main(int argc, char* argv[]){
	ifstream source_code;
	vector<pair<string,string>> lexemes;
	string src_line, buffer;
	int exception = 0, err=0;
	
	if(argc==1){
		cout << "Input file path:\n";
		string path;
		cin >> path;
		source_code.open(path);
	}else{
		source_code.open(argv[1]);
	}
	
	
	if(source_code.is_open()){
		while(getline(source_code,src_line) && err==0){
			err = lexer(&src_line,&lexemes,&buffer,&exception);
			line++;
			col=1;
		}
		
		if(err == 0){
			for(int i=0;i<lexemes.size();i++){
				cout << "{\"" << lexemes[i].first << "\", " << lexemes[i].second << "}" << endl;
			}
		}
	}else{
		cout << "File not found!";
		err = 1;
	}
	return err;
}