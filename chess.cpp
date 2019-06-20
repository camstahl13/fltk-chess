#include <iostream>
#include <algorithm>
#include <string>
#include <regex>
#include "chess_classes.h"

using namespace std;

Board board;

bool Piece::path_valid(Path p) {
	auto p_b = p.begin();
	auto p_e = p.end();
	auto l = p.back();
	Delta d = distance_to(l);
	Piece* dest_occ = board[l.row][l.col];
	if ((dest_occ == nullptr && any_of(barred_if_not_opp->begin(), barred_if_not_opp->end(), [&](Delta el) { return el == d; }))
	   ||
	   (dest_occ != nullptr && (dest_occ->team == team || any_of(barred_if_opp->begin(), barred_if_opp->end(), [&](Delta el) { return el == d; })))) {
		return false;
	}
	p.pop_back();
	for (auto coord : p) {
		Piece* occupant = board.get_at(coord);
		if (occupant != nullptr) {
			return false;
		}
	}
	return true;
}

bool Piece::result_valid() {
	if (board.check_lane.empty()) {
		if (not board.in_check(team).empty()) {
			return false;
		}
	} else if (none_of(++(board.check_lane.begin()), board.check_lane.end(), [&](Coordinate el) { return board.get_at(el)->team == team; }) 
			   &&
			   board.get_at(board.check_lane.front()) != nullptr) {
		return false;
	}
	board.check_lane = board.in_check(!team);
	return true;
}

int extract(int idx, smatch sm) {
	return stoi(sm[idx].str());
}

int main() {
	Team turn = Team::white;
	string user_input;
	regex rx("\\(([0-7]),([0-7])\\) -> \\(([0-7]),([0-7])\\)");
	smatch match;
	bool repeat = false;
	do {
		board.display();
		do {
			if (repeat) {
				cout << "Invalid move. Still ";
			}
			cout << (turn == Team::white ? "White" : "Black") << "'s turn:\n";
			getline(cin, user_input);
		} while (!regex_match(user_input, match, rx));
		Coordinate from {extract(1, match), extract(2, match)};
		Coordinate to {extract(3, match), extract(4, match)};
		Piece* piece = board.get_at(from);
		Piece* dest = board.get_at(to);
		//cout << "from = (" << from.row << "," << from.col << ")\n";
		Path r_d_v = piece->delta_valid(to);
		if (piece && piece->team == turn && !r_d_v.empty() && piece->path_valid(r_d_v)) {
			pair<Piece*, bool> r_m = board.move(from, to);
			if (piece->result_valid()) {
				turn = !turn;
				repeat = false;
			} else {
				board.undo_move(from, to, r_m.first, r_m.second);
				repeat = true;
			}
		} else {
			repeat = true;
		}
	} while (true/*!board.check_mate()*/); // will implement board.check_mate() later
	return 0;
}

// vim:ts=4:sw=4:noet
