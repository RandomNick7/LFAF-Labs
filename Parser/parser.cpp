#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <cctype>

//TODO 1: Try to use the Shunting Yard Algorithm on the atoms vector to parse them as well
//If repeated failure, skip to TODO#2
//TODO 2: Represent the tree using DFS, just navigate the tree by prioritizing first_child first
//TODO 3: Make a short report describing the parser, mentioning improvements of Lexer and including the example script

using namespace std;

//Just turn an n-ary tree into a binary tree
struct Node{
	string data;
	Node* first_child = NULL;
	Node* next_child = NULL;
};

void store_tree(Node node, vector<string>* tree, vector<int>* level, int lv){
	tree->push_back(node.data);
	level->push_back(lv);
	if(node.first_child!=NULL){
		store_tree(*node.first_child, tree, level, lv+1);
	}
	if(node.next_child!=NULL){
		store_tree(*node.next_child, tree, level, lv);
	}
}

void parser(vector<string>* tokens, vector<string>* token_vals, Node* AST, vector<string>* tree, vector<int>* level, int* index, int lv){
	//Can't do switch statements with strings in an easy way, so we're going the YandereDev route
	Node vertex;
	if(AST->data == "root"){
		//Represents the global scope
		//Only function declarations allowed here
		while((*tokens)[*index] == "FUNCTION_DEF"){
			vertex.data = "FuncDef";
			Node* curr_node = AST->first_child;
			if(curr_node == NULL){
				tree->push_back(vertex.data);
				level->push_back(lv);
				AST->first_child = &vertex;
				(*index)++;
				parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
			}else{
				Node temp;
				temp.data = vertex.data;
				while(curr_node->next_child!=NULL){
					curr_node = curr_node->next_child;
				}
				curr_node->next_child = &temp;
				tree->push_back(temp.data);
				level->push_back(lv);
				(*index)++;
				parser(tokens,token_vals,&temp,tree,level,index,lv+1);
			}
		}
		(*index)++;
		
		if(*index < tokens->size()){
			cout << "Code outside any function block!" << endl;
			exit(1);
		}
	}else if(AST->data == "FuncDef"){
		//Function definition, nameless or not ">>=>(...){...}"
		if((*tokens)[*index] == "IDENTIFIER"){
			vertex.data = "FuncName";
			Node leaf;
			leaf.data = (*token_vals)[*index];
			vertex.first_child = &leaf;
			tree->push_back(vertex.data);
			level->push_back(lv);
			tree->push_back(leaf.data);
			level->push_back(lv+1);
			AST->first_child = &vertex;
			(*index)++;
			parser(tokens,token_vals,AST,tree,level,index,lv+2);
		}else if((*tokens)[*index] == "SYM_PAREN_L"){
			vertex.data = "FuncArgs";
			//If function has a name then set args branch as sibbling
			if(AST->first_child==NULL){
				tree->push_back(vertex.data);
				level->push_back(lv);
				AST->first_child = &vertex;
			}else{
				tree->push_back(vertex.data);
				level->push_back(lv);
				AST->first_child->next_child = &vertex;
			}
			(*index)++;
			parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		}
		if((*tokens)[*index] == "SYM_BRACE_L"){
			vertex.data = "Block";
			Node* curr_node = AST->first_child;
			if(curr_node == NULL){
				tree->push_back(vertex.data);
				level->push_back(lv);
				AST->first_child = &vertex;
			}else{
				while(curr_node->next_child!=NULL){
					curr_node = curr_node->next_child;
				}
				tree->push_back(vertex.data);
				level->push_back(lv);
				curr_node->next_child = &vertex;
			}
			(*index)++;
			parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		}
	}else if(AST->data == "FuncArgs"){
		//List of arguments in function definition "(...)"
		if((*tokens)[*index].rfind("TYPE",0) != string::npos){
			vertex.data = "ArgDecl";
			Node* curr_node = AST->first_child;
			if(curr_node == NULL){
				tree->push_back(vertex.data);
				level->push_back(lv);
				AST->first_child = &vertex;
			}else{
				while(curr_node->next_child!=NULL){
					curr_node = curr_node->next_child;
				}
				tree->push_back(vertex.data);
				level->push_back(lv);
				curr_node->next_child = &vertex;
			}
		}
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
	}else if(AST->data == "ArgDecl"){
		//Argument declaration in function, different from ID declaration - commas are checked and no assignment allowed
		//Set type as first child
		vertex.data = (*token_vals)[*index];
		(*index)++;
		//Set name as second child
		Node leaf;
		leaf.data = (*token_vals)[*index];
		vertex.first_child = &leaf;
		tree->push_back(vertex.data);
		level->push_back(lv);
		tree->push_back(leaf.data);
		level->push_back(lv+1);
		AST->first_child = &vertex;
		(*index)++;
		if((*tokens)[*index] == "SYM_SEP"){
			(*index)++;
			parser(tokens,token_vals,AST,tree,level,index,lv);
		}else{
			(*index)++;
		}
	}else if(AST->data == "Block"){
		//Generic block of code "{...}", can contain pretty much anything in it
		while((*tokens)[*index] != "SYM_BRACE_R"){
			if((*tokens)[*index] == "WHILE_LOOP"){
				vertex.data = "WhileLoop";
			}else if((*tokens)[*index] == "FOR_LOOP"){
				vertex.data = "ForLoop";
			}else if((*tokens)[*index].rfind("TYPE",0) != string::npos){
				vertex.data = "VarDecl";
			}else if((*tokens)[*index] == "IDENTIFIER"){
				vertex.data = "VarSet";
			}else if((*tokens)[*index] == "ASSIGN"){
				vertex.data = "Assignment";
			}else if((*tokens)[*index] == "RETURN"){
				vertex.data = "Return";
			}else if((*tokens)[*index] == "IF_COND"){
				vertex.data = "If";
			}else if((*tokens)[*index] == "ELSE_COND"){
				vertex.data = "Else";
			}else if((*tokens)[*index] == "READ"){
				vertex.data = "Read";
			}else if((*tokens)[*index] == "WRITE"){
				vertex.data = "Write";
			}else{
				cout << "Unrecognized code" << endl;
				exit(1);
			}
			
			Node* curr_node = AST->first_child;
			if(curr_node == NULL){
				tree->push_back(vertex.data);
				level->push_back(lv);
				AST->first_child = &vertex;
				parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
			}else{
				Node temp;
				temp.data = vertex.data;
				while(curr_node->next_child!=NULL){
					curr_node = curr_node->next_child;
				}
				tree->push_back(temp.data);
				level->push_back(lv);
				curr_node->next_child = &temp;
				parser(tokens,token_vals,&temp,tree,level,index,lv+1);
			}
		}
		(*index)++;
	}else if(AST->data == "WhileLoop"){
		//While loop made of a condition and a block ">->...{...}"
		vertex.data = "Cond";
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		(*index)++;
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		
		Node leaf;
		leaf.data = "Block";
		tree->push_back(leaf.data);
		level->push_back(lv);
		vertex.next_child = &leaf;
		(*index)++;
		parser(tokens,token_vals,&leaf,tree,level,index,lv+1);
	}else if(AST->data == "ForLoop"){
		//For loop made of a declaration and a block "|->...{...}"
		vertex.data = "ForDecl";
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		(*index)++;
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		
		Node leaf;
		leaf.data = "Block";
		tree->push_back(leaf.data);
		level->push_back(lv);
		vertex.next_child = &leaf;
		(*index)++;
		parser(tokens,token_vals,&leaf,tree,level,index,lv+1);
	}else if(AST->data == "ForDecl"){
		//The declaration of a For loop: "(value or assignment), Condition, Expression"
		vertex.data = "ForFirst";
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		(*index)++;
		
		Node leaf;
		leaf.data = "Cond";
		tree->push_back(leaf.data);
		level->push_back(lv);
		vertex.next_child = &leaf;
		parser(tokens,token_vals,&leaf,tree,level,index,lv+1);
		(*index)++;
		
		Node next_leaf;
		next_leaf.data = "VarSet";
		tree->push_back(next_leaf.data);
		level->push_back(lv);
		leaf.next_child = &next_leaf;
		parser(tokens,token_vals,&next_leaf,tree,level,index,lv+1);
	}else if(AST->data == "ForFirst"){
		//First part of a for - either an ID declaration or setting a value
		if((*tokens)[*index].rfind("TYPE",0) != string::npos){
			vertex.data = "VarDecl";
		}else if((*tokens)[*index] == "IDENTIFIER"){
			vertex.data = "VarSet";
		}else{
			cout << "Invalid For Loop entry!" << endl;
			exit(1);
		}
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
	}else if(AST->data == "Cond"){
		//Any combination of initialization, ID or constant being compared with another
		if((*tokens)[*index].rfind("TYPE",0) != string::npos){
			vertex.data = "VarDecl";
			tree->push_back(vertex.data);
			level->push_back(lv);
			AST->first_child = &vertex;
			parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		}else if((*tokens)[*index].rfind("CONST",0) != string::npos || (*tokens)[*index] == "IDENTIFIER"){
			vertex.data = (*token_vals)[*index];
			tree->push_back(vertex.data);
			level->push_back(lv);
			AST->first_child = &vertex;
		}else{
			cout << "Invalid comparison argument" << endl;
			exit(1);
		}
		(*index)++;
		
		Node leaf;
		if((*tokens)[*index].rfind("COMP",0)!=string::npos){
			leaf.data = (*token_vals)[*index];
			tree->push_back(leaf.data);
			level->push_back(lv);
			vertex.next_child = &leaf;
		}else{
			cout << "Missing exit condition!" << endl;
			exit(1);
		}
		(*index)++;
		
		Node next_leaf;
		if((*tokens)[*index].rfind("TYPE",0) != string::npos){
			next_leaf.data = "VarDecl";
			tree->push_back(next_leaf.data);
			level->push_back(lv);
			leaf.first_child = &next_leaf;
			parser(tokens,token_vals,&next_leaf,tree,level,index,lv+1);
		}else if((*tokens)[*index].rfind("CONST",0) != string::npos || (*tokens)[*index] == "IDENTIFIER"){
			next_leaf.data = (*token_vals)[*index];
			tree->push_back(next_leaf.data);
			level->push_back(lv);
			leaf.first_child = &next_leaf;
		}else{
			cout << "Invalid comparison argument" << endl;
			exit(1);
		}
		(*index)++;
	}else if(AST->data == "VarDecl"){
		//"Type ID ..." part
		if((*tokens)[*index].rfind("TYPE",0) != string::npos){
			vertex.data = (*tokens)[*index];
			tree->push_back(vertex.data);
			level->push_back(lv);
			AST->first_child = &vertex;
		}else{
			cout << "Variable type error" << endl;
			exit(1);
		}
		(*index)++;
		
		Node leaf;
		if((*tokens)[*index] == "IDENTIFIER"){
			leaf.data = (*token_vals)[*index];
			tree->push_back(leaf.data);
			level->push_back(lv);
			vertex.next_child = &leaf;
		}else{
			cout << "Variable declaration error" << endl;
			exit(1);
		}
		(*index)++;
		
		//If the ID is assigned a value (i.e. Type ID <~ ...), make an Assignment branch
		Node next_leaf;
		if((*tokens)[*index] == "ASSIGN"){
			next_leaf.data = "Assignment";
			tree->push_back(next_leaf.data);
			level->push_back(lv);
			leaf.next_child = &next_leaf;
			parser(tokens,token_vals,&next_leaf,tree,level,index,lv+1);
		}
	}else if(AST->data == "Assignment"){
		//"<~ ..." part
		vertex.data = (*token_vals)[*index];
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		(*index)++;
		
		Node leaf;
		leaf.data = "Expression";
		tree->push_back(leaf.data);
		level->push_back(lv);
		vertex.next_child = &leaf;
		parser(tokens,token_vals,&leaf,tree,level,index,lv+1);
	}else if(AST->data == "Expression"){
		vector<Node> input;
		int id_combo = 0, len = 0, add_to_list;
		//Find the entire expression
		while((*tokens)[*index] == "IDENTIFIER" || (*tokens)[*index].rfind("CONST",0) != string::npos || (*tokens)[*index].rfind("SYM_PAREN",0) != string::npos ||(*tokens)[*index].rfind("OP",0) != string::npos){
			add_to_list = 1;
			vertex.data = (*token_vals)[*index];
			//Bracket right after ID - this indicates a function call
			//Take entire call as a single part
			if(id_combo == 1 && (*tokens)[*index].rfind("SYM_PAREN",0) != string::npos){
				(*index)++;
				while((*tokens)[*index].rfind("SYM_PAREN",0)){
					vertex.data += (*token_vals)[*index];
					(*index)++;
				}
				vertex.data += (*token_vals)[*index];
				input[input.size()-1].data += vertex.data;
				add_to_list = 0;
			}
			
			//Count number of IDs in a row
			if((*tokens)[*index] == "IDENTIFIER"){
				id_combo++;
			}else{
				id_combo=0;
			}
			//If there are 2 IDs in a row, the expression is over
			if(id_combo == 2){
				break;
			}
			
			if(add_to_list){
				input.push_back(vertex);
			}
			
			len++;
			(*index)++;
		}
		
		if(len == 0){
			cout << "Missing expression!" << endl;
			exit(1);
		}
		//All the expression tokens are stored in input vector, perform the Shunting Yard Algorithm
		vector<Node> stack;
		vector<Node> output;
		unordered_map<string, int> precedence;
		string operators = "!*%/+-&^|()";
		//Set up the operator precedence (Following C/C++ operator precedence)
		precedence["!"] = 5;	//Binary NOT
		precedence["*"] = 4;	//Multiplication
		precedence["/"] = 4;	//Division
		precedence["%"] = 4;	//Modulo
		precedence["+"] = 3;	//Addition
		precedence["-"] = 3;	//Subtraction
		precedence["&"] = 2;	//Binary AND
		precedence["^"] = 1;	//Binary XOR
		precedence["|"] = 0;	//Binary OR
		precedence["("] = -1;	//Brackets placed at the bottom
		
		for(int i=0; i<input.size(); i++){
			if(input[i].data.find_first_of(operators) == string::npos || input[i].data.size()>1){
				output.push_back(input[i]);
			}else{
				if(stack.size()>0){
					if(input[i].data == "("){
						stack.push_back(input[i]);
					}else if(input[i].data == ")"){
						while(stack.back().data != "("){
							output.push_back(stack.back());
							stack.pop_back();
							if(stack.size() == 0){
								cout << "Mismatched brackets in expression!" << endl;
								exit(1);
							}
						}
						stack.pop_back();
					}else{
						if(precedence[input[i].data] > precedence[stack.back().data]){
							stack.push_back(input[i]);
						}else{
							while(precedence[input[i].data] <= precedence[stack.back().data] && stack.size()>0){
								output.push_back(stack.back());
								stack.pop_back();
							}
							stack.push_back(input[i]);
						}
					}
				}else{
					stack.push_back(input[i]);
				}
			}
		}
		
		for(int i=stack.size()-1; i>=0; i--){
			output.push_back(stack.back());
			stack.pop_back();
		}
		
		//Rearranging our Reverse Polish Notation to AST form and appending to tree...
		vector<int> top_nodes;
		for(int i=0; i<output.size(); i++){
			if(output[i].data.find_first_of(operators) == string::npos || input[i].data.size()>1){
				top_nodes.push_back(i);
			}else{
				len = top_nodes.size()-1;
				output[top_nodes[len-1]].next_child = &output[top_nodes[len]];
				output[i].first_child = &output[top_nodes[len-1]];
				top_nodes.pop_back();
				top_nodes.pop_back();
				top_nodes.push_back(i);
			}
		}
		AST->first_child = &output[top_nodes.back()];
		store_tree(*AST->first_child, tree, level, lv);
		
	}else if(AST->data == "VarSet"){
		//"ID <~ ..." part
		vertex.data = (*token_vals)[*index];
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		(*index)++;
		
		Node leaf;
		if((*tokens)[*index] == "ASSIGN"){
			leaf.data = (*token_vals)[*index];
			tree->push_back(leaf.data);
			level->push_back(lv);
			vertex.next_child = &leaf;
		}else{
			cout << "Faulty assignment" << endl;
			exit(1);
		}
		(*index)++;
		
		Node next_leaf;
		next_leaf.data = "Expression";
		tree->push_back(next_leaf.data);
		level->push_back(lv);
		leaf.next_child = &next_leaf;
		parser(tokens,token_vals,&next_leaf,tree,level,index,lv+1);
	}else if(AST->data == "Return"){
		//|^| represents the return statement, one ID allowed after it
		vertex.data = (*token_vals)[*index];
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		(*index)++;
		
		Node leaf;
		if((*tokens)[*index] == "IDENTIFIER" || (*tokens)[*index].rfind("CONST",0) != string::npos){
			leaf.data = (*token_vals)[*index];
			tree->push_back(leaf.data);
			level->push_back(lv);
			vertex.next_child = &leaf;
			(*index)++;
		}
	}else if(AST->data == "If"){
		//"=>...{...}" parts
		vertex.data = "Cond";
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		(*index)++;
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
		if((*tokens)[*index] == "SYM_BRACE_L"){
			Node leaf;
			leaf.data = "Block";
			tree->push_back(leaf.data);
			level->push_back(lv);
			vertex.next_child = &leaf;
			(*index)++;
			parser(tokens,token_vals,&leaf,tree,level,index,lv+1);
		}
	}else if(AST->data == "Else"){
		//"#={...}" parts, can include an If statement right behind it instead
		(*index)++;
		if((*tokens)[*index] == "IF_COND"){
			vertex.data = "If";
		}else if((*tokens)[*index] == "SYM_BRACE_L"){
			(*index)++;
			vertex.data = "Block";
		}else{
			cout << "Illegal Else statement" << endl;
			exit(1);
		}
		tree->push_back(vertex.data);
		level->push_back(lv);
		AST->first_child = &vertex;
		parser(tokens,token_vals,&vertex,tree,level,index,lv+1);
	}else if(AST->data == "Read"){
		//">] ..." parts, any number of arguments
		(*index)++;
		vector<Node> parts;
		if((*tokens)[*index] == "IDENTIFIER"){
			vertex.data = (*tokens)[*index];
			(*index)++;
		}else{
			cout << "Invalid write statement" << endl;
			exit(1);
		}
		parts.push_back(vertex);
		
		while((*tokens)[*index] == "SYM_SEP"){
			if((*tokens)[*index] == "IDENTIFIER"){
				vertex.data = (*tokens)[*index];
				(*index)++;
			}else{
				cout << "Invalid write statement" << endl;
				exit(1);
			}
			parts.push_back(vertex);
		}
		
		for(int i=parts.size()-2; i>=0; i--){
			parts[i].next_child = &parts[i+1];
		}
		
		for(int i=0; i<parts.size(); i++){
			tree->push_back(parts[i].data);
			level->push_back(lv);
		}
		
		AST->first_child = &(parts[0]);
	}else if(AST->data == "Write"){
		//"[< ..." parts, any number of arguments
		(*index)++;
		vector<Node> parts;
		if((*tokens)[*index] == "QUOTATION"){
			(*index)++;
			vertex.data = (*tokens)[*index];
			(*index)+=2;
		}else if((*tokens)[*index] == "IDENTIFIER"){
			vertex.data = (*tokens)[*index];
			(*index)++;
		}else{
			cout << "Invalid write statement" << endl;
			exit(1);
		}
		parts.push_back(vertex);
		
		while((*tokens)[*index] == "SYM_SEP"){
			(*index)++;
			if((*tokens)[*index] == "QUOTATION"){
				(*index)++;
				vertex.data = (*tokens)[*index];
				(*index)+=2;
			}else if((*tokens)[*index] == "IDENTIFIER"){
				vertex.data = (*tokens)[*index];
				(*index)++;
			}else{
				cout << "Invalid write statement" << endl;
				exit(1);
			}
			parts.push_back(vertex);
		}
		
		for(int i=parts.size()-2; i>=0; i--){
			parts[i].next_child = &parts[i+1];
		}
		
		for(int i=0; i<parts.size(); i++){
			tree->push_back(parts[i].data);
			level->push_back(lv);
		}
		
		AST->first_child = &(parts[0]);
	}
}

int main(){
	ifstream token_list;
	token_list.open("token_list.txt");
	Node AST;
	vector<string> token_arr;
	vector<string> token_contents;
	//Tree structure gets lost when exiting parse function, store structure in these 2
	vector<string> tree;
	vector<int> tree_lvl;
	
	if(token_list.is_open()){
		string token;
		while(getline(token_list,token)){
			int index = token.find('"');
			string token_type = token.substr(0,index-1);
			string token_content = token.substr(index+1,token.size()-index-2);
			
			token_arr.push_back(token_type);
			token_contents.push_back(token_content);
		}
		
		AST.data = "root";
		tree.push_back(AST.data);
		tree_lvl.push_back(0);
		int index = 0;
		parser(&token_arr, &token_contents, &AST, &tree, &tree_lvl, &index, 1);
		
		for(int i=0; i<tree.size(); i++){
			string prefix(tree_lvl[i]*2,' ');
			cout << prefix << tree[i] << endl;
		}
	}else{
		cout << "File not found!" << endl;
	}
	return 0;
}