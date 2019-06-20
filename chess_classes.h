#ifndef CHESS_CLASSES_H
#define CHESS_CLASSES_H
#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <utility>
#include "chess_gameplay.h"

using namespace std;

enum class Piece_type {Rook, Bishop, Knight, Queen, King, Pawn, None};

enum class Team {white, black};

Team operator!(Team t) {
	return (t == Team::white ? Team::black : Team::white);
}

struct Coordinate;

using Delta = Coordinate;

struct Coordinate {
	int row;
	int col;
	bool operator==(Coordinate coord) {
		return (row == coord.row && col == coord.col);
	}
	void operator+=(Coordinate coord) {
		row += coord.row;
		col += coord.col;
	}
	void operator-=(Coordinate coord) {
		row -= coord.row;
		col -= coord.col;
	}
	bool on_board() {
		return (row >= 0 && row <= 7 && col >= 0 && col <= 7);
	}
	Coordinate(int abscissa = 0, int ordinate = 0) {
		row = abscissa;
		col = ordinate;
	}
	Delta operator+(Coordinate c) {
		return Delta{(row + c.row), (col + c.col)};
	}

	Delta operator-(Coordinate c) {
		return Delta{(row - c.row), (col - c.col)};
	}
};

struct Search_path {
	Delta d;
	int cnt;
	vector<Piece_type> pts;
};

const vector<Delta> pawn_barred_if_opp {Delta{1, 0}, Delta{2, 0}};

const vector<Delta> pawn_barred_if_not_opp {Delta{1, 1}, Delta{1, -1}};

const vector<Delta> others_barred_if_opp {};

const vector<Delta> others_barred_if_not_opp {};

using Path = vector<Coordinate>;

/*
| | | | | | | |
| | | |O| | | |
| | | | | | | |
| | | | | | | |
| | | | |X| | |
| | | | | | | |
| | | | | | | |
| | | | | | | |
*/

struct Piece {
	Coordinate position;
	Team team;
	const vector<Delta>* barred_if_opp;
	const vector<Delta>* barred_if_not_opp;
	bool has_moved = false;
	Piece_type type;
	Delta distance_to(Coordinate destination) {
		Delta delta{destination.row - position.row, destination.col - position.col};
		if (team == Team::black) {           // black should start at row 0
			delta.col *= -1;
		} else {
			delta.row *= -1;
		}
		return delta;
	}
	Delta abs_distance_to(Coordinate destination) {
		return Delta{abs(destination.row - position.row), abs(destination.col - position.col)};
	}
	Path path_to(Coordinate destination) {
		int row_inc = ((destination.row - position.row) == 0 ? 0 : ((destination.row - position.row) / abs(destination.row - position.row)));
		int col_inc = ((destination.col - position.col) == 0 ? 0 : ((destination.col - position.col) / abs(destination.col - position.col)));
		Path path;
		for (int prow = position.row + row_inc, pcol = position.col + col_inc, cnt = 1; 
			cnt <= abs(destination.row - position.row); 
			prow += row_inc, pcol += col_inc, cnt++) 
		{
			path.push_back(Coordinate{prow, pcol});
		}
		return path;
	}
	bool path_valid(Path);
	bool result_valid();
	virtual Path delta_valid(Delta move) = 0;
	Piece(Coordinate pos, Team tm, Piece_type t, const vector<Delta>* b_i_o = &others_barred_if_opp, const vector<Delta>* b_i_n_o = &others_barred_if_not_opp) 
		:position{pos}, team{tm}, type{t}, barred_if_opp{b_i_o}, barred_if_not_opp{b_i_n_o} {}
};

struct Pawn : public Piece {
		Pawn(Coordinate pos, Team tm):Piece{pos, tm, Piece_type::Pawn, &pawn_barred_if_opp, &pawn_barred_if_not_opp} {}
		Path delta_valid(Delta destination) {
			Delta d_t = distance_to(destination);
			//cout << "Distance to = (" << d_t.row << "," << d_t.col << ")\n";
			if (destination.on_board() && ((d_t.row == 1 && d_t.col == 0)
						        || (d_t.row == 1 && abs(d_t.col) == 1) 
						        || (d_t.row == 2 && d_t.col == 0 && (not has_moved)))) {
				return Path{destination};
			} else {
				return Path{};
			}
		}
};

struct Knight : public Piece {
		Knight(Coordinate pos, Team tm):Piece{pos, tm, Piece_type::Knight} {}
		Path delta_valid(Delta destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && abs_d_t.row > 0 && abs_d_t.col > 0 && abs_d_t.row < 3 && abs_d_t.col < 3 
			    && (abs_d_t.row / abs_d_t.col == 2 || abs_d_t.col / abs_d_t.row == 2)) {
				return Path{destination};
			} else {
				return Path{};
			}
		}
};

struct Bishop : public Piece {
		Bishop(Coordinate pos, Team tm):Piece{pos, tm, Piece_type::Bishop} {}
		Path delta_valid(Delta destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && abs_d_t.row != 0 && abs_d_t.row == abs_d_t.col) {
				return path_to(destination);
			} else {
				return Path{};
			}
		}
};

struct Rook : public Piece {
		Rook(Coordinate pos, Team tm):Piece{pos, tm, Piece_type::Rook} {}
		Path delta_valid(Delta destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && ((abs_d_t.row == 0 && abs_d_t.col != 0) || (abs_d_t.row != 0 && abs_d_t.col == 0))) {
				return path_to(destination);
			} else {
				return Path{};
			}
		}
};

struct Queen: public Piece {
		Queen(Coordinate pos, Team tm):Piece{pos, tm, Piece_type::Queen} {}
		Path delta_valid(Delta destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && ((abs_d_t.row == 0 && abs_d_t.col != 0) || (abs_d_t.row != 0 && abs_d_t.col == 0) || (abs_d_t.row != 0 && abs_d_t.row == abs_d_t.col))) {
				return path_to(destination);
			} else {
				return Path{};
			}
		}
};

struct King: public Piece {
		King(Coordinate pos, Team tm):Piece{pos, tm, Piece_type::King} {}
		Path delta_valid(Delta destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && abs_d_t.row < 2 && abs_d_t.col < 2 && (abs_d_t.row > 0 || abs_d_t.col > 0)) {
				return Path{destination};
			} else {
				return Path{};
			}
		}
};

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

struct Board {
	vector<vector<Piece*>> board;
	Path check_lane;
	Coordinate black_king = {0, 4};
	Coordinate white_king = {7, 4};
	/*void swap(Coordinate c1, Coordinate c2) {
		Piece* saved_val = board[c1.row][c1.col];
		board[c1.row][c1.col] = board[c2.row][c2.col];
		board[c2.row][c2.col] = saved_val;
	}*/
	void display() {
		map<Team, map<Piece_type, string>> piece_strings {  { Team::black, {{Piece_type::Pawn, " \u2659 "},
			                                                            {Piece_type::Knight, " \u2658 "},
								                    {Piece_type::Bishop, " \u2657 "},
								                    {Piece_type::Rook, " \u2656 "},
								                    {Piece_type::Queen, " \u2655 "},
								                    {Piece_type::King, " \u2654 "}} },
			                                            { Team::white, {{Piece_type::Pawn, " \u265F "},
			                                                            {Piece_type::Knight, " \u265E "},
								                    {Piece_type::Bishop, " \u265D "},
								                    {Piece_type::Rook, " \u265C "},
								                    {Piece_type::Queen, " \u265B "},
								                    {Piece_type::King, " \u265A "}} }  };
		string spacer = "\n   -----------------------------------------\n";
		cout << "     0    1    2    3    4    5    6    7";
		cout << spacer;
		int row_cnt = 0;
		for (auto row : board) {
			cout << row_cnt << "  |";
			++row_cnt;
			for (auto piece : row) {
				cout << (piece ? piece_strings[piece->team][piece->type] : "   ") << " |";
			}
			cout << spacer;
		}

	}
	Path in_check(Team tm) {
		Coordinate king = (tm == Team::black ? black_king : white_king);
		int forward = (tm == Team::black ? 1 : -1);
		vector<Search_path> sps = { { {forward, 1}, 1, {Piece_type::Pawn} },
			                    { {forward, -1}, 1, {Piece_type::Pawn} },
					    { {1, 1}, 7, {Piece_type::Queen, Piece_type::Bishop} },
					    { {1, -1}, 7, {Piece_type::Queen, Piece_type::Bishop} },
					    { {-1, 1}, 7, {Piece_type::Queen, Piece_type::Bishop} },
					    { {-1, -1}, 7, {Piece_type::Queen, Piece_type::Bishop} },
					    { {1, 0}, 7, {Piece_type::Queen, Piece_type::Rook} },
					    { {-1, 0}, 7, {Piece_type::Queen, Piece_type::Rook} },
					    { {0, 1}, 7, {Piece_type::Queen, Piece_type::Rook} },
					    { {0, -1}, 7, {Piece_type::Queen, Piece_type::Rook} },
					    { {1, 2}, 1, {Piece_type::Knight} },
					    { {2, 1}, 1, {Piece_type::Knight} },
					    { {-1, 2}, 1, {Piece_type::Knight} },
					    { {-2, 1}, 1, {Piece_type::Knight} },
					    { {1, -2}, 1, {Piece_type::Knight} },
					    { {2, -1}, 1, {Piece_type::Knight} },
					    { {-1, -2}, 1, {Piece_type::Knight} },
					    { {-2, -1}, 1, {Piece_type::Knight} } };
		for (sp : sps) {
			int cnt = sp.cnt;
			Path return_path {king};
			for (Coordinate coord = (king + sp.d);
			     cnt > 0 && coord.on_board();
			     coord += sp.d, cnt--) {
				Piece* occupant = board[coord.row][coord.col];
				return_path.push_back(coord);
				if (occupant && occupant->team != tm && any_of(sp.pts.begin(), sp.pts.end(),
						                               [&occupant](Piece_type pt) { return pt == occupant->type; })) {
					return return_path;
				}
			}
		}
		return Path{};
	}
	pair<Piece*, bool> move(Coordinate c1, Coordinate c2) {
		Piece* to_move = board[c1.row][c1.col];
		bool old_has_moved = to_move->has_moved;
		if (to_move->type == Piece_type::King) {
			(to_move->team == Team::black) ? black_king = c2 : white_king = c2;
		}
		Piece* to_return = board[c2.row][c2.col];
		board[c2.row][c2.col] = to_move;
		to_move->position = c2;
		to_move->has_moved = true;
		board[c1.row][c1.col] = nullptr;
		return make_pair(to_return, old_has_moved);
	}
	void undo_move(Coordinate c1, Coordinate c2, Piece* replace_piece, bool replace_has_moved) {
		if (replace_piece && replace_piece->type == Piece_type::King) {
			(replace_piece->team == Team::black) ? black_king = c1 : white_king = c1;
		}
		//change has_moved here! MAYBE!
		board[c1.row][c1.col] = board[c2.row][c2.col];
		board[c1.row][c1.col]->position = c1;
		board[c1.row][c1.col]->has_moved = replace_has_moved;
		board[c2.row][c2.col] = replace_piece;
	}
	vector<Piece*>& operator[](int row) {
		return board[row];
	}
	Piece*& get_at(Coordinate c) {
		return board[c.row][c.col];
	}
	Board() {
		vector<Piece_type> edge_row { Piece_type::Rook, Piece_type::Bishop, Piece_type::Knight, Piece_type::Queen, 
			                      Piece_type::King, Piece_type::Knight, Piece_type::Bishop, Piece_type::Rook };
		vector<vector<Piece_type>> order { edge_row,
						   vector<Piece_type>(8, Piece_type::Pawn),
					           vector<Piece_type>(8, Piece_type::None),
					           vector<Piece_type>(8, Piece_type::None),
					           vector<Piece_type>(8, Piece_type::None),
					           vector<Piece_type>(8, Piece_type::None),
					           vector<Piece_type>(8, Piece_type::Pawn),
						   edge_row };
		Team tm = Team::black;
		for (int row = 0; row <= 7; ++row) {
			board.push_back(vector<Piece*>());
			if (row == 2) { tm = Team::white; }
			for (int col = 0; col <= 7; ++col) {
				Piece_type el = order[row][col];
				if (el == Piece_type::None) {
					board[row].push_back(nullptr);
				} else {
					board[row].push_back(create_piece(el, Coordinate(row, col), tm));
				}
			}
		}
	}
};
#endif
