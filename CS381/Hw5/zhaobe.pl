% Lance Umagat umagatl
% Chris Nguyen nguyenc5
% Benny Zhao zhaobe

% Here are a bunch of facts describing the Simpson's family tree.
% Don't change them!

female(mona).
female(jackie).
female(marge).
female(patty).
female(selma).
female(lisa).
female(maggie).
female(ling).

male(abe).
male(clancy).
male(herb).
male(homer).
male(bart).

married_(abe,mona).
married_(clancy,jackie).
married_(homer,marge).

married(X,Y) :- married_(X,Y).
married(X,Y) :- married_(Y,X).

parent(abe,herb).
parent(abe,homer).
parent(mona,homer).

parent(clancy,marge).
parent(jackie,marge).
parent(clancy,patty).
parent(jackie,patty).
parent(clancy,selma).
parent(jackie,selma).

parent(homer,bart).
parent(marge,bart).
parent(homer,lisa).
parent(marge,lisa).
parent(homer,maggie).
parent(marge,maggie).

parent(selma,ling).



%%
% Part 1. Family relations
%%

% 1. Define a predicate `child/2` that inverts the parent relationship.
child(X,Y) :- parent(Y,X).

% 2. Define two predicates `isMother/1` and `isFather/1`.
isMother(X) :- parent(X,_), female(X).
isFather(X) :- parent(X,_), male(X).

% 3. Define a predicate `grandparent/2`.
grandparent(X,Z) :- parent(X,Y), parent(Y,Z).

% 4. Define a predicate `sibling/2`. Siblings share at least one parent.
sibling(X,Y) :- parent(P,X), parent(P,Y), X \= Y.

% 5. Define two predicates `brother/2` and `sister/2`.
brother(X,Y) :- sibling(X,Y), male(X).
sister(X,Y) :- sibling(X,Y), female(X).

% 6. Define a predicate `siblingInLaw/2`. A sibling-in-law is either married to
%    a sibling or the sibling of a spouse.
siblingInLaw(X,Y) :- married(X,S), sibling(S,Y), X \= Y.
siblingInLaw(X,Y) :- sibling(X,S), married(S,Y), X \= Y.

% 7. Define two predicates `aunt/2` and `uncle/2`. Your definitions of these
%    predicates should include aunts and uncles by marriage.
aunt(X,Y) :- female(X), sibling(X,S), child(Y,S).
aunt(X,Y) :- female(X), sibling(P,S), child(Y,S), married(P,Q).

uncle(X,Y) :- male(X), sibling(X,S), child(Y,S).
uncle(X,Y) :- male(X), married(X,P), sibling(P,S), child(Y,S).

% 8. Define the predicate `cousin/2`.
cousin(X,Y) :- child(X,P), sibling(P,S), parent(S,Y).

% 9. Define the predicate `ancestor/2`.
ancestor(X,Y) :- parent(X,Y).
ancestor(X,Y) :- parent(X,S), ancestor(S,Y).

% all members of this group collaborated to achieve this.
% Extra credit: Define the predicate `related/2`.
related(X,Y) :- male(Y), X \= Y.
related(X,Y) :- female(Y), X \= Y.





%%
% Part 2. Language implementation
%%
bool(t).
bool(f).

literal(X) :- number(X).
literal(X) :- string(X).
literal(X) :- bool(X).

% load values into stack
cmd(C, S1,S2) :- literal(C), S2 = [C|S1].

% 1. Define the predicate `cmd/3`, which describes the effect of executing a
%    command on the stack.

cmd(add, [Left, Right | S1], S2) :- S2 = [Res | S1], Res is Left + Right.

% Had to add the cutoff because of the backtrace returning a false value.
cmd(lte, [Left, Right | S1], S2) :- S2 = [t | S1], Left =< Right, !.
cmd(lte, [ _ , _ | S1], S2) :- S2 = [f|S1].

cmd(if(P1,_), [t|S1], S2) :- prog(P1, S1, S2).
cmd(if(_,P2), [f|S1], S2) :- prog(P2, S1, S2).

% 2. Define the predicate `prog/3`, which describes the effect of executing a
%    program on the stack.

prog([Cmd], S1, Last) :- cmd(Cmd, S1, Last).
prog([Cmd | CmdsLeft], S1, Last) :- cmd(Cmd, S1, S2), prog(CmdsLeft, S2, Last).
