# Introduction to Formal Languages. Regular Grammars. Finite Automata.

### Course: Formal Languages & Finite Automata

### Author: Otgon Dorin

## Theory

**Formal Language** - Set of words formed according to a rigid set of rules whose letters are taken from a pre-defined alphabet  
**Formal Grammar** - Set of rules according to which valid words or sentences can be constructed  
**Finite Automaton** - Set of specific states with a set of functions that define transition rules between the states and follow input symbols from an alphabet (specific set of symbols)

## Objectives

* Understand what a language is and what makes it qualify as a "formal" one  
* Setting up the project for the semester by creating a local and remote repository and choosing a programming language to work with
* Implement a grammar within code and add a function that generates 5 strings from it
* Create a function that can convert your Grammar to a Finite Automaton and implement a function that can check whether a string can be obtained or not

## Implementation Description

Each objective has been completed, starting with the Grammar object, which is of the form:
```cpp
class Grammar{
	private:
		vector<char> nonTerminalVars;
		vector<char> terminalVars;
		vector<string> prodRules[2];
		char startChar = 'S';
	public:
		Grammar(...){...};
		string generate_string(){...};
		FiniteAutomaton convert(){...};
}
```
as following the mathematical definition. The methods within have been added to satisfy the latter part of Objective #3.
The Finite Automaton follows a similar structure, following the mathematical model as well:
```cpp
class FiniteAutomaton{
	private:
		vector<char> states;
		vector<char> alphabet;
		vector<Tr> transition;
		char startState = 'S';
		char endState = '0';
	FiniteAutomaton(...){...};
	int is_valid_word(string word){...};
```

where `Tr` is a structure representing a transition:
```cpp
struct Tr{
	char start;
	string symbols;
	char end;
};
```

An additional `0` state has been added to serve as a singular exit state for simplicity. It can be reached by states that have transitions that do not involve any other states.



With the structure of our classes defined, we can now discuss about the implementation of the functions.

The grammar must be able to create strings on demand. The function `generate_string()` will start from the default starting character `S` and will proceed to follow the production rules provided by the grammar to iteratively generate the word.
The function shown below goes through our current word character by character, replacing non-terminal variables with strings containing other variables, according to the rules provided. If there are multiple rules involving the same variable, all rules will be considered and a random outcome will be chosen.
If a rule is missing, the function exits early, warning the user that their production rules are incomplete, and showing a partial result afterwards.

Note: The function does not check previously-generated strings, so depending on the ruleset, repeating strings may be a common outcome.

```cpp
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
```
The above function may be called any number of times for random valid string generation.

In order to convert a Grammar to a Finite Automaton, we simply need to convert the information of the Grammar object in a specific way, where:
- The non-terminal variables become states
- The terminal variables become the alphabet
- The production rules become transitions
- The starting state is the starting string
- The end state is determined according to the set of transitions

This has been implemented via the constructor of the FiniteAutomaton object, taking as input the data of a grammar:
```cpp
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
```
The above `for` loop simply changes the format we store our transitions in; the grammar stores them as `"string->string"`, while the automaton storees them as {`char`,`string`,`char`} 

Following that, a function is defined that puts the Fresh Automaton to use: `is_valid_word()` accepts any string as input and outputs the truth value of whether the word can be constructed by the automaton or not (in other words, if it belongs to the corresponding grammar or not)
```cpp
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
```

The above function starts from the first state of the automaton and goes through all the rules, checking which ones are both valid and allow for the creation of the provided string. The automaton will change state if a valid rule is provided, generating a sequence of characters matching the input. The automaton will stop as soon as no transitions may be acheieved, or once it reached the state `0`, where the only option is to stop.
If there have been no problems with the generation of the word, a final check is made to see if the automaton had to stop early or had to keep going. This is done to prevent the automaton from passing "incomplete" words as valid.

## Results
The program was made to accept user input and provides instructions on how it must be formatted for smooth execution.
Giving the program the grammar definition of Variant 19...  
![Providing input to the program](../Screenshots/Lab_1/input.png)

The grammar, now an object within our program, will randomly generate 5 valid strings:  
![String generation](../Screenshots/Lab_1/gen.png)

Afterwards, the program creates a finite automaton object, asking the user to input any strings they wish in order to check if they belong to the grammar or not:  
![Checking several strings](../Screenshots/Lab_1/valid.png)

The results can be proof-checked to be correct using other software or pen and paper given spare time.

## Conclusion
Over the course of this laboratory work, a functioning implementation of the mathematical model of the newly learned concepts of a grammar and a finite automaton have been successfully made and tested, providing useful results and serving as a basis for developing more advanced algorithms based on the aforementioned concepts.

## References
Johnson, M., & Zelenski, J. (2012, June 29). Formal Grammars. Retrieved February 17, 2023, from https://web.stanford.edu/class/archive/cs/cs143/cs143.1128/handouts/080%20Formal%20Grammars.pdf
Jim, A., & Nathan, O. (2019, Spring). Finite Automata. https://www.cs.unc.edu/~otternes/comp455/2-finite-automata.pdf