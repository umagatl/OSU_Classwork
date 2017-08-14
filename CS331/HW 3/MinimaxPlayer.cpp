/*
 * MinimaxPlayer.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: wong
 */

/*
References used
http://stackoverflow.com/questions/6887838/improving-minimax-algorithm
http://mnemstudio.org/game-reversi-example-2.htm
https://github.com/bekoeppel/Lynx-Reversi-Player/blob/master/miniMaxPlayer/MinimaxPlayer.java
Class slides
*/

#include <iostream>
#include <assert.h>
#include "MinimaxPlayer.h"

using std::vector;

MinimaxPlayer::MinimaxPlayer(char symb) :
		Player(symb) {

}

MinimaxPlayer::~MinimaxPlayer() {

}

int MinimaxPlayer::runUtility(OthelloBoard *board) {
	return board->count_score(board->get_p1_symbol()) - board->count_score(board->get_p2_symbol());
}

vector<OthelloBoard*> MinimaxPlayer::getSucc(char playerSym, OthelloBoard *board){
	vector<OthelloBoard*> bVector;
	int boardDim = 4;
	
	for(int i = 0; i < boardDim; i++){
		for(int j = 0; j < boardDim; j++){
			if(board->is_legal_move(i, j, playerSym)){
				bVector.push_back(new OthelloBoard(*board));
				bVector.back()->play_move(i,j, symbol);
				
				bVector.back()->setCol(i);
				bVector.back()->setRow(j);
			}
		}
	}
	return bVector;
}

int MinimaxPlayer::evalMin(int &row, int&col, char playerSym, OthelloBoard *board){
	vector<OthelloBoard*> bVector;
	int minRow = 0;
	int minCol = 0;
	int minInt = -32767;
	
	//Check turn
	if(playerSym == 'X'){
		bVector = getSucc('X', board);
	}
	else if(playerSym == 'O'){
		bVector = getSucc('O', board);
	}
	
	// check to see if list is empty
	if(bVector.size() == 0){
		return runUtility(board);
	}
	
	// recursively traverse the board
	for( int i = 0; i < bVector.size(); i++){
		if(evalMin(row, col, playerSym, bVector[i]) > minInt){
			minRow = bVector[i]->getRow();
			minCol = bVector[i]->getCol();
			minInt = evalMin(row, col, playerSym, bVector[i]);
		}
	}
	
	row = minRow;
	col = minCol;
	return minInt;
}

int MinimaxPlayer::evalMax(int &row, int&col, char playerSym, OthelloBoard *board){
	vector<OthelloBoard*> bVector;
	int maxRow = 0;
	int maxCol = 0;
	int maxInt = -32767;
	
	//Check turn
	if(playerSym == 'X'){
		bVector = getSucc('X', board);
	}
	else if(playerSym == 'O'){
		bVector = getSucc('O', board);
	}
	
	// check to see if list is empty
	if(bVector.size() == 0){
		return runUtility(board);
	}
	
	// recursively traverse the board
	for( int i = 0; i < bVector.size(); i++){
		if(evalMin(row, col, playerSym, bVector[i]) > maxInt){
			maxRow = bVector[i]->getRow();
			maxCol = bVector[i]->getCol();
			maxInt = evalMin(row, col, playerSym, bVector[i]);
		}
	}
	
	row = maxRow;
	col = maxCol;
	return maxInt;
}

void MinimaxPlayer::get_move(OthelloBoard* b, int& col, int& row) {
    // To be filled in by you
	// Check player and initialize minimax on appropriate player
	if (symbol == b->get_p1_symbol()) {
		evalMax(row, col, 'X', b);
	} else if (symbol == b->get_p2_symbol()) {
		evalMax(row, col, 'O', b);
	}
}

MinimaxPlayer* MinimaxPlayer::clone() {
	MinimaxPlayer* result = new MinimaxPlayer(symbol);
	return result;
}
