# Chomsky Normal Form

### Course: Formal Languages & Finite Automata

### Author: Otgon Dorin

## Theory

The **Chomsky Normal Form** is a set of rules that can be applied to context-free grammars.
The rules state that all productions must follow the following form: A->BC or A->a, where A, B, C are non-terminal symbols and a is a terminal one.
## Objectives

* Learning the theory behind the Chomsky Normal Form
* Studying and applying the algorithm of conversion of a type-2 grammar to a corresponding CNF

## Implementation Description
4 new functions have been added to the Grammar object, 3 of which are auxiliary.
`delete_delta_vars()` is for deleting variables outside of a specific set  
`create_prod_combos()` is used for generating new productions by ommitting specific symbols in a specific way to guarantee all combinations' generation  
`remove_duplicate_prods()` does as the name says: removes duplicate productions from the grammar  

`to_Chomsky()` is split up into 6 parts, each with code responsible of performing interemediary steps for the conversion.  
Code examples have been omitted due to how robust the relevant code is, available in `grammar.hpp`. It is meant to work on any type-2 grammar.

- Step 1: Elimination of ε-productions

&emsp;The code checks for any production of the type A->ε and other productions from all other variables leading to A. Any non-terminal with a rule of the type A->B~1~B~2~...B~n~ where B~i~ is a nullable character will render A itself nullable.
By repeatedly going through all the rules, all the nullable variables are found. Productions of the form A->ε are eliminated from the grammar. If that production aws the only one a variable had, the variable itself is eliminated from the grammar as well.
All productions with a nullable non-terminal symbol in them will have all possible variations with specific symbols absent be generated as well using `create_prod_combos()`. This may produce duplicate productions, which are later cleaned up.

- Step 2: Removing unit productions

&emsp;The code checks every production to see if there are any of size 1 and that lead to a non-terminal. In that case, the production is eliminated, and all the rules of the eliminated rule's left hand side are introduced to the current symbol's productions. If any unit productions are introduced, they are substituted as well.
A string called `visited` keeps track of which variables have been substitude already, to avoid infinite loops or self-referenciation (rules of the type A->A)  
This may also generate duplicate productions, which are cleaned up at the end.

- Step 3: Eliminating inaccessible symbols

&emsp;S is the starting symbol, so it will always be accessible. The program will check all productions from S and remember which non-terminal symbols the productions lead to. One of these symbols will be picked to have its productions checked as well, adding to the set of accessible variables. This process will continue iteratively until no more new elements are added to the set.
Any variables deemed inaccessible are erased, along with their productions.

- Step 4: Eliminating useless productions

&emsp;Similar to Step 3, but the process happens backwards. Rules that lead only to terminal symbols indicate that the non-terminal is useful. Non-terminal symbols with productions that lead to other useful non-terminal symbols are themselves useful.
All production rules are checked repeatedly until the set of useful variables does not increase anymore. Any variables outside this set are deemed useless and thus erased, along with their productions.

- Step 5: Substitution of terminal symbols within larger productions

&emsp;With a cleaned up grammar, we can convert to a Chomsky Normal Form in 2 steps. This represents the first step, where the program checks which terminal symbols are part of rules greater than size 1, and dedicates a non-terminal symbol with a single production to that terminal.
Any instances of terminals in rules with more than 1 character produced are substituted with this rule.

- Step 6: Splitting productions with more than 2 non-terminals

&emsp;At this point, the grammar only has rules of the type A->a or A->B~1~B~2~...B~n~, where A, B~i~ are non-terminal and a is terminal. The A->a kind are ignored in this step.
Checking for rules greater than size 2, they are split up in an iterative process where the first 2 characters of a rule are replaced with a new non-terminal with a single production to those 2 characters.
This process repeats iteratively until all rules are of size 2, achieving the Chomsky Normal Form.

&emsp;Due to the 26 non-terminal character limit, new rule memoization has been implemented to avoid going over the limit. In the opposing case, an intermediary result is printed and the user is informed.

An example of execution can be the assigned exercise:
```
Vn = {S A B C E}
Vt = {a d}
P = {
	S->dB|B
	A->d|dS|aAdCB
	B->aC|dA|AC
	C-><e>
	E->AS
}
```
will get converted to:
```
Vn = {S A B C D E F}
Vt = {a d}
P = {
	S->a|FB|d|DA|DB|DS
	A->FB|d|DS
	B->a|FB|d|DA|DS
	C->a
	D->d
	E->CA
	F->ED
}
```

Another, more complex example is:
```
Vn = {S A B C D}
Vt = {a}
P = {
	B->ABC|ε
	D->aa
	C->AA
	S->ABBADBCa
	A->BC|ε
}
```
which is filled with useless productions. The program generates the following CNF:
```
Vn = {S D A}
Vt = {a}
P = {
	A->a
	D->AA
	S->DA
}
```

The results can be proof-checked to be correct using other software or pen and paper given a modest amount of spare time.

## Conclusion
&emsp;Over the course of this laboratory work, a successful method of conversion from a context-free grammar to a corresponding Chomsky Normal Form has been created. The studied steps for achieving this form have been succesfully implemented in such a way as to support any type-2 grammar.
