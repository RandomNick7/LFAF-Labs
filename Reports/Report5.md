# Parser & Building an Abstract Syntax Tree

### Course: Formal Languages & Finite Automata

### Author: Otgon Dorin

## Theory

The **Parser** is a program responsible for syntax analysis with the purpose of transforming a list of tokens given by the Lexer into an Abstract Syntax Tree, while conforming to a set of rules defined by a grammar.  
The **Abstract Syntax Tree**, further abbreviated as "AST", is a data structure which represents the syntactic structure of code. They're different from parse trees because they do not represent the entirety of the tokens used in the syntax analysis - some can be inferred within the hierarchy (like brackets).
ASTs serve as an intermediate representation of code, required by an interpreter or compiler
## Objectives

* Understanding the concept of parsing and how it can be implemented
* Getting familiar with the concept of an AST
* Using the work done on Lab #3, create a parser which can use the output of that lab's lexer

## Implementation Description
Before describing the parser, some minor changes have been performed to the Lexer:
1) Comments no longer have tokens and are simply skipped over. They've only proven to be more problematic
2) The code has been vastly simplified and its length greatly reduced (from ~410 down to ~250 lines)

The parser was based on a fairly simple grammar reminiscent of GPL ones:
```
<program> ::= <function><program> | ε
<function> ::= ">>=>"<function_name>"("<function_args>"){"<block>"}"|<function_name>"{"<block>"}"
<function_name> ::= <identifier>|ε
<function_args> ::= <type><identifier>|<type><identifier>","<function_args>
<block> ::= <while_loop><block>|<for_loop><block>|<if_condition><block>|<read><block>|<write><block>|<identifier><block>|<assignment><block>|<return><block>
<while_loop> ::= ">->"<condition>"{"<block>"}"
<for_loop> ::= "|->"<for_declaration>"{"<block"}"
<for_declaration> ::= <for_init>","<condition>","<var_set>
<for_init> ::= <var_decl>|<var_set>
<if_condition> ::= "=>"<condition><block>|"=>"<condition><block><else_condition>
<else_condition> ::= "#="<block>|"#="<if_condition>
<read> ::= ">]"<input>
<input> ::= <identifier>|<identifier>","<input>
<write> ::= "[<"<output>
<output> ::= <identifier>|<string>|<identifier>","<output>|<string>","<output>
<condition> ::= <var_decl><comparison_op><var_decl>|<var_decl><comparison_op><var_set>|<var_decl><comparison_op><expression>|<var_set><comparison_op><var_decl>|<var_set><comparison_op><var_set>|<var_set><comparison_op><expression>|<expression><comparison_op><var_decl>|<expression><comparison_op><var_set>|<expression><comparison_op><expression>
<var_decl> ::= <type><identifier><assignment>
<var_set> ::= <identifier><assignment>
<identifier> ::= <string>
<assignment> ::= "<~"<expression>
<expression> ::= <const>|<identifier>|"("<expression>")"|<expression><operation><expression>
<const> ::= <string>|<float_number>|<int_number>
<string> ::= <char><string>|<char>
<float_number> ::= <int_number>"."<int_number>
<int_number> ::= <digit><int_number>|<digit>
<char> ::= "A"|"B"|...|"Z"|"a"|"b"|...|"z"
<operation> ::= "+"|"-"|"*"|"/"|"%"|"&"|"^"|"|"
<digit> ::= "0"|"1"|"2"|...|"9"
```

The parsing program itself was implemented via recursive descent, that is, the AST is built top-down. The grammar was implemented in a way to require no backtracking.  
Expressions are parsed is a special way using the shunting yard algorithm to remove brackets and organize operands and operators in a way as to not require brackets and be easy to merge with the larger AST. It could be considered as a parser within a parser.

Going through the code, all the tokens from the lexer are split into value and type, each placed in their own vector, which is iterated through by the parser, implemented via one huge function. The parser function has the following arguments:  
`vector<string>* tokens` - the vector of token types  
`vector<string>* token_vals` - the vector of token values  
`Node* AST` - the current node within the AST where the parser function is being executed  
`vector<string>* tree` - vector of values storing the data required for the AST  
`vector<int>* level` - vector of numbers storing the recurrence level within the parser function, used for AST representation  
`int* index` - index of the token lists, shared by all recurrences of the parser function  
`int lv` - current recursion level, mostly used by the aforementioned `level` variable  

Since the AST is an n-ary tree, it has been converted to a binary tree instaed by using the following structure to represent each node:
```
struct Node{
	string data;
	Node* first_child = NULL;
	Node* next_child = NULL;
};
```

`next_child`, as in "sibling" implies no descent within the AST. The images below show how this conversion works:

![A visual representation of a generic tree](../Screenshots/Lab_2/generic_tree.png)  
![The same generic tree, restructured in binary form](../Screenshots/Lab_2/binary_tree.png)


The default node the parser starts is called `root`, which corresponds to `<program>` within the grammar. The parser will analyse a specific number of tokens depending on its `Node* AST` argument and will check if they follow the grammar rules. If they do, add the tokens to the AST and move to the first child to be analyzed. This has been implemented with a lengthy `if...else` block, as C++ does not support `switch` statements with strings.  

When reaching an expression, the parser will perform the shunting yard algorithm to re-write the expression in Reverse Polish Notation. This removes the need for brackets and inherently represents a postfix traversal of the expression's AST. It is then reorganized to fit with the node representation chosen, and then to be merged with the program's AST.  

The only shortcomings of the parser is arrays, as successful parsing of them has been proven to be too problematic to be done within the time frame given.  

Code examples have been omitted due to the program being almost 700 lines large, most of which relevant to the subject. Instead, 2 practical examples are given to show what the program does:

1) Parsing an expression  

Code:
```
>>=>{
	int test <~ 10|3+5^7*2/(1-5)+6&3*4
}
```
The above code uses a bunch of operations with varying priorities, including binary operations. The precedence order was inspired by the one implemented in C/C++ and is shown below:
```
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
```
The higher the number, the greater the priority of the operation, brackets being an exception.

Upon execution of the parser program, the following output is produced:
```
root
  FuncDef
    Block
      VarDecl
        TYPE_INT
        test
        Assignment
          <~
          Expression
            |
              10
              ^
                +
                  3
                  5
                &
                  +
                    /
                      *
                        7
                        2
                      -
                        1
                        5
                    6
                  *
                    3
                    4
```
The indentation corresponds to the level within the tree. The children of a node are all the nodes with 2 spaces to the left up until another node with the same level of indentation.  
For example, `VarDecl` has children `TYPE_INT`, `test`, `Assignment`, `^` has children `+` and `&`, `&` has children `+` and `*`, and so on.
This representation has been chosen for convenience and to occupy a small amount of space while making the tree structure still visible.

2) Parsing a program  

A program valid within the defined language has been written:
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

>>=>{
	int x <~ 24
	int y <~ 16
	|->int i<~1, i<3, i<~i+1{
		int res <~ gcd(x,y)
		=>res>=2{
			[<"GCD = ", gcd
		}#={
			[<"There is no common divisor"
		}
	}
}

%>=>20-10><10{int test <~ 10.5}
<%
```

This is a program capable of calculating the gratest common divisor between 2 numbers x and y, in this case 24 and 16. This program was chosen as it contains most of the rules within the grammar. The following is the parser's output:

```
root
  FuncDef
    FuncName
      gcd
        FuncArgs
          ArgDecl
            int
              a
            int
              b
        Block
          WhileLoop
            Cond
              b
              <>
              0
            Block
              VarDecl
                TYPE_INT
                t
                Assignment
                  <~
                  Expression
                    b
              VarSet
                b
                <~
                Expression
                  %
                    a
                    b
              VarSet
                a
                <~
                Expression
                  t
          Return
            |^|
            a
  FuncDef
    Block
      VarDecl
        TYPE_INT
        x
        Assignment
          <~
          Expression
            24
      VarDecl
        TYPE_INT
        y
        Assignment
          <~
          Expression
            16
      ForLoop
        ForDecl
          ForFirst
            VarDecl
              TYPE_INT
              i
              Assignment
                <~
                Expression
                  1
          Cond
            i
            <
            3
          VarSet
            i
            <~
            Expression
              +
                i
                1
        Block
          VarDecl
            TYPE_INT
            res
            Assignment
              <~
              Expression
                gcd(x,y)
          If
            Cond
              res
              >=
              2
            Block
              Write
                CONST_STR
                IDENTIFIER
          Else
            Block
              Write
                CONST_STR
```

Same as before, the indentation represents the level within the tree, and the parent/child relationships can be easily noticed.  
A representation using drawn graphs was omitted due to time constraints.

The parser can also show errors in case of some mistakes, such as wrongful declarations or misplaced blocks of code, however, it cannot show the specific line and column of the error, as the only input of the program is a basic list of token-value pairs generated by the Lab 3 lexer.

The results can be proof-checked to be correct using other software or pen and paper given a great amount of spare time.

## Conclusion
&emsp;During this laboratory work, a successful implementation of a parser has been created for the original language. It actually includes another parser inside of it for analysing mathematical expressions. It outputs a simple representation of the generated AST, illustrating the data and hierarchy within it.
It is capable of producting ASTs for any program that respects a specific set of rules and report some common errors that the user may commit.
The parser was made to be mostly compatible with the output given by Lab #3's lexer.
The language will not see any practical use due to its peculiar syntax, but it has still been useful in the process of learning about lexer and parser implementation by providing a challenge to both.