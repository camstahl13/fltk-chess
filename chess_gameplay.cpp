/*
#include <iostream>
#include <vector>
#include <cstdlib>
#include "chess_gameplay.h"

using namespace std;

Piece* create_piece(Piece_type pt, Coordinate pos, Team tm) {
	switch (pt) {
		case Piece_type::Rook:
			return new Rook(pos, tm);
		case Piece_type::Bishop:
			return new Bishop(pos, tm);
		case Piece_type::Knight:
			return new Knight(pos, tm);
		case Piece_type::Queen:
			return new Queen(pos, tm);
		case Piece_type::King:
			return new King(pos, tm);
		case Piece_type::Pawn:
			return new Pawn(pos, tm);
	}
}
*/
