# Lexer & Scanner

### Course: Formal Languages & Finite Automata

### Author: Otgon Dorin

## Theory

**Scanner** - A program that splits an input string into tokens, according to a delimiter. The delimiter is susually whitespace.  
**Lexer** - A program which turns a string into a series of tokens that describes the data and the corresponding type

## Objectives

* Learning about lexers - what they are and how they work
* Create a set of rules which the lexer must follow in order to generate lexemes accordingly
* Implementing a lexer by hand which can successfully separate strings into lexems, following the rules. 

## Implementation Description
The lexer was made in C++ without the need of any code of the previous 2 laboratory works.
This lexer was made on the basis of an esoteric language, which supports basic commands such as conditionals and loops.
Although lacking a properly defined grammar, it takes inspiration from C, using simillar commands.
The quirk that makes this language esoteric is that the only real "words" in it are variable types and the user's identifiers.
To illustrate, this is an array containing the special keywords within the language:
```
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
```

A font with ligatures, such as Fira Code, Iosevka or Monoid are recommended to make the text somewhat more readable.

The way this was implemented in the program was using a set of functions meant to determine a specific sequence of characters. All of them are meant to read part of the input until reaching a space.
Specifically, the code is separated into 6 functions responsible for such:
* `lexer` - responsible for recognizing characters that make the code following them not be executed: such as comments or quotation marks denoting string constants. Calls below functions depending on character encountered.
* `determine_num` - checks and separates valid integers and floating point numbers from the rest of the code. Adds negative signs to single numbers preceeded by a minus.
* `determine_id` - verifies if a string of alphanumeric characters and underscores form a valid identifier. Identifiers may not start with anything but a letter.
* `determine_type` - verifies if a string describes a specific type. Types supported are integers, floats, characters, strings, booleans and arrays of these elements. If the string does not match, call the previous function.
* `determine_symbol` - checks for valid symbols. Includes a list of exceptions in order to split the string properly and not throw exceptions for lack of space separation.
* `determine_keyword` - verifies if a sequence of characters creates a valid keyword within the language. The list of keywords was shown earlier. On failure to recognnize, call previous function.

While a Finite Automaton could have been used to perform this task, setting one up and debugging it would prove to be tedious.

Notable features of the lexer include adjusting lexems according to existing ones - multi-dimensional array types occupy only 1 lexeme instead of one per dimension. Negative numbers are considered as such when the minus does not function as an infix. Escape character `\` is included for allowing the use of `"` in string literals.
Another feature is basic error checking: including invalid identifiers, floats as array sizes or without digits past the decimal point, use of unrecognized symbols or string literals that haven't been closed on the same line - all return an error, interrupting the lexing process.
The error is indicated with a specific line and column, with the error itself being pointed out.

## Results
The program was made to accept user input from a file by providing its name either as an argument or while running the program.

Providing the program with the following example input:
```
=>20-10><10{int test <~ 10}
```

The program successfully prints out a string of lexemes:
```
{"=>", IF_COND}
{"20", CONST_INT}
{"-", OP_SUB}
{"10", CONST_INT}
{"><", OP_EQ}
{"10", CONST_INT}
{"{", SYM_BRACE_L}
{"int", TYPE_INT}
{"test", IDENTIFIER}
{"<~", ASSIGN}
{"10", CONST_INT}
{"}", SYM_BRACE_R}
```

An algorithm for checking the Greatest Common Divisor, featuring most of the commands within the program, is shown below as a more verbose example:
```
/>Greatest Common Divisor
>>=>gcd(int a, int b){
	>->b<>0{
		int t<~b
		b <~ a%b
		a <~ t
	}
	|^| a
}

int#4 arr <~ {24,16,-54,135}
int res
|->int i<~1, i<3, i<~i+1{
	res <~ gcd(arr[i-1],[i])
	=>res>=2{
		>]"GCD = ",gcd
	}#= {
		>]"There is no common divisor"
	}
}
```

The program outputs a correspondingly larger set of lexemes:
```
{"/>", COMMENT_LINE}
{"Greatest Common Divisor", COMMENT_BODY}
{">>=>", FUNCTION_DEF}
{"gcd", IDENTIFIER}
{"(", SYM_PAREN_L}
{"int", TYPE_INT}
{"a", IDENTIFIER}
{",", SYM_SEP}
{"int", TYPE_INT}
{"b", IDENTIFIER}
{")", SYM_PAREN_R}
{"{", SYM_BRACE_L}
{">->", WHILE_LOOP}
{"b", IDENTIFIER}
{"<>", OP_NEQ}
{"0", CONST_INT}
{"{", SYM_BRACE_L}
{"int", TYPE_INT}
{"t", IDENTIFIER}
{"<~", ASSIGN}
{"b", IDENTIFIER}
{"b", IDENTIFIER}
{"<~", ASSIGN}
{"a", IDENTIFIER}
{"%", OP_MOD}
{"b", IDENTIFIER}
{"a", IDENTIFIER}
{"<~", ASSIGN}
{"t", IDENTIFIER}
{"}", SYM_BRACE_R}
{"|^|", RETURN}
{"a", IDENTIFIER}
{"}", SYM_BRACE_R}
{"int", ARR_TYPE_INT}
{"4", CONST_INT}
{"arr", IDENTIFIER}
{"<~", ASSIGN}
{"{", SYM_BRACE_L}
{"24", CONST_INT}
{",", SYM_SEP}
{"16", CONST_INT}
{",", SYM_SEP}
{"-54", CONST_INT}
{",", SYM_SEP}
{"135", CONST_INT}
{"}", SYM_BRACE_R}
{"int", TYPE_INT}
{"res", IDENTIFIER}
{"|->", FOR_LOOP}
{"int", TYPE_INT}
{"i", IDENTIFIER}
{"<~", ASSIGN}
{"1", CONST_INT}
{",", SYM_SEP}
{"i", IDENTIFIER}
{"<", OP_LT}
{"3", CONST_INT}
{",", SYM_SEP}
{"i", IDENTIFIER}
{"<~", ASSIGN}
{"i", IDENTIFIER}
{"+", OP_ADD}
{"1", CONST_INT}
{"{", SYM_BRACE_L}
{"res", IDENTIFIER}
{"<~", ASSIGN}
{"gcd", IDENTIFIER}
{"(", SYM_PAREN_L}
{"arr", IDENTIFIER}
{"[", SYM_BRKET_L}
{"i", IDENTIFIER}
{"-1", CONST_INT}
{"]", SYM_BRKET_R}
{",", SYM_SEP}
{"[", SYM_BRKET_L}
{"i", IDENTIFIER}
{"]", SYM_BRKET_R}
{")", SYM_PAREN_R}
{"=>", IF_COND}
{"res", IDENTIFIER}
{">=", OP_GTE}
{"2", CONST_INT}
{"{", SYM_BRACE_L}
{">", OP_GT}
{"]", SYM_BRKET_R}
{""", QUOTATION}
{",gcd", CONST_STRING}
{""", QUOTATION}
{",", SYM_SEP}
{"gcd", IDENTIFIER}
{"}", SYM_BRACE_R}
{"#=", ELSE_COND}
{"{", SYM_BRACE_L}
{">", OP_GT}
{"]", SYM_BRKET_R}
{""", QUOTATION}
{"", CONST_STRING}
{""", QUOTATION}
{"}", SYM_BRACE_R}
{"}", SYM_BRACE_R}
```

The results can be proof-checked to be correct using other software or pen and paper given a decent amount of spare time.

## Conclusion
&emsp;During this laboratory work, there has been a successful implementaation of a lexer for an original language. 
The lexer shows a set of results corresponding to the user's input and is capable of recognizing a decent set of features that can consist as a basis for a General Purpose Language. 
Despite the language not seeing practical use due to its peculiar syntax, it serves as an educational tool and provides a somewhat more challenging experience for implementation.