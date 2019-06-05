#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

using Path = vector<Coordinate>;

const vector<Delta> barred_if_empty = {Delta{1, 1}, Delta{1, -1}};

enum Team = {white, black};

struct Coordinate {
	int row;
	int col;
	void operator+=(Coordinate coord) {
		row += coord.row;
		col += coord.col;
	}
	void operator-=(Coordinate coord) {
		row -= coord.row;
		col -= coord.col;
	}
	bool operator==(Coordinate coord) {
		row == coord.row;
		col == coord.col;
	}
	bool on_board() {
		return (row >= 0 && row <= 7 && col >= 0 && col <= 7);
	}
	Coordinate(int abscissa, int ordinate) {
		row = abscissa;
		col = ordinate;
	}
};

Coordinate operator+(Coordinate c1, Coordinate c2) {
	return Coordinate{(c1.row + c2.row), (c1.col + c2.col)};
}

Coordinate operator-(Coordinate c1, Coordinate c2) {
	return Coordinate{(c1.row - c2.row), (c1.col - c2.col)};
}

struct Delta : Coordinate {};

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

class Piece {
	protected:
		Coordinate position;
		Team team;
	public:
		vector<Delta>* barred_if_empty = nullptr;
		bool has_moved = false;
		Delta distance_to(Coordinate destination) {
			Delta delta; 
			if (team == black) {           // black should start at row 0
				delta.row = position.row - destination.row;
				delta.col = destination.col - position.col;
			} else {
				delta.row = destination.row - position.row;
				delta.col = position.col - destination.col;
			}
			return delta;
		}
		Delta abs_distance_to(Coordinate destination) {
			Delta d_t = distance_to(destination);
			d_t.row = abs(d_t.row);
			d_t.col = abs(d_t.col);
			return d_t;
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
		virtual Path delta_valid(Delta move) = 0;
};

class Pawn : public Piece {
	public:
		Pawn(Coordinate pos, Team tm):position(pos), team(tm), barred_if_empty(&pawn_barred_if_empty) {}
		Path delta_valid(Coordinate destination) {
			Delta d_t = distance_to(destination);
			if (destination.on_board() && ((d_t.row == 0 && d_t.col == 1)
						        || (abs(d_t.row) == 1 && d_t.col == 1) 
						        || (abs(d_t.row) == 0 && d_t.col == 2 && (not has_moved)))) {
				return Path{destination};
			} else {
				return Path{};
			}
		}
};

class Knight : public Piece {
	public:
		Knight(Coordinate pos, Team tm):position(pos), team(tm) {}
		Path delta_valid(Coordinate destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && abs_d_t.row > 0 && abs_d_t.col > 0 && abs_d_t.row < 3 && abs_d_t.col < 3 
			    && (abs_d_t.row / abs_d_t.col == 2 || abs_d_t.col / abs_d_t.row == 2)) {
				return Path{destination};
			} else {
				return Path{};
			}
		}
};

class Bishop : public Piece {
	public:
		Bishop(Coordinate pos, Team tm):position(pos), team(tm) {}
		Path delta_valid(Coordinate destination) {
			Delta d_t = distance_to(destination);
			if (destination.on_board() && d_t.row != 0 && d_t.row == d_t.col) {
				return path_to(destination);
			} else {
				return Path{};
			}
		}
};

class Rook : public Piece {
	public:
		Rook(Coordinate pos, Team tm):position(pos), team(tm) {}
		Path delta_valid(Coordinate destination) {
			Delta d_t = distance_to(destination);
			if (destination.on_board() && ((d_t.row == 0 && d_t.col != 0) || (d_t.row != 0 && d_t.col == 0))) {
				return path_to(destination);
			} else {
				return Path{};
			}
		}
}

class Queen: public Piece {
	public:
		Queen(Coordinate pos, Team tm):position(pos), team(tm) {}
		Path delta_valid(Coordinate destination) {
			Delta d_t = distance_to(destination);
			if (destination.on_board() && ((d_t.row == 0 && d_t.col != 0) || (d_t.row != 0 && d_t.col == 0) || (d_t.row != 0 && d_t.row == d_t.col))) {
				return path_to(destination);
			} else {
				return Path{};
			}
		}
}

class King: public Piece {
	public:
		King(Coordinate pos, Team tm):position(pos), team(tm) {}
		Path delta_valid(Coordinate destination) {
			Delta abs_d_t = abs_distance_to(destination);
			if (destination.on_board() && d_t.row < 2 && d_t.col < 2 && (d_t.row > 0 || d_t.col > 0)) {
				return Path{destination}
			} else {
				return Path{};
			}
		}
}
