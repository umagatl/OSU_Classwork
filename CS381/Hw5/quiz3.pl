lives(zebra, savanna).     
lives(lion, savanna).      
lives(sloth, forest).      
lives(deer, forest).       
lives(shark, ocean).

eats(zebra, grass).
eats(lion, meat).
eats(sloth, leaves).
eats(deer, grass).     
eats(shark, meat).

portal(1,blue,2).
portal(2,blue,3).            
portal(3,blue,4).

portal(3,red,4).
portal(4,red,1).
portal(4,red,2).

portal(2,green,1).
portal(3,green,2).
portal(4,green,2).

whichAnimal(X,Y) :- eats(X,grass), lives(X,Y).

neighbor(X,Y) :- lives(X,S), lives(Y,S), X\=Y.

prey(X) :- neighbor(X,S), eats(S,meat).

path(X,Y,Z) :- portal(X,Y,Z).
path(X,Y,Z) :- portal(X,Y,S), path(S,Y,Z).