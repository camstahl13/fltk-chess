#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <regex>
#include <type_traits>
#include <fstream>
#include <unistd.h> // isatty
#include <thread>
#include "chess_classes.h"
#include "draw.h"

#include <thread>
#include <chrono>

using namespace std;

Coordinate from;

Coordinate to;

Team turn = Team::white;

map<int, map<int, Fl_PNG_Image*>> test_pngs = {  { 1, {{2, (new Fl_PNG_Image("./Black_Pawn.png"))}} }  };

map<Team, map<Piece_type, Fl_PNG_Image*>> piece_pngs {  { Team::black, {{Piece_type::Pawn, new Fl_PNG_Image("./Black_Pawn.png")},
																		{Piece_type::Knight, new Fl_PNG_Image("./Black_Knight.png")},
																		{Piece_type::Bishop, new Fl_PNG_Image("./Black_Bishop.png")},
																		{Piece_type::Rook, new Fl_PNG_Image("./Black_Rook.png")},
																		{Piece_type::Queen, new Fl_PNG_Image("./Black_Queen.png")},
																		{Piece_type::King, new Fl_PNG_Image("./Black_King.png")}} },
														{ Team::white, {{Piece_type::Pawn, new Fl_PNG_Image("./White_Pawn.png")},
																		{Piece_type::Knight, new Fl_PNG_Image("./White_Knight.png")},
																		{Piece_type::Bishop, new Fl_PNG_Image("./White_Bishop.png")},
																		{Piece_type::Rook, new Fl_PNG_Image("./White_Rook.png")},
																		{Piece_type::Queen, new Fl_PNG_Image("./White_Queen.png")},
																		{Piece_type::King, new Fl_PNG_Image("./White_King.png")}} }  };

//map<Piece_type, Fl_PNG_Image*> test = piece_pngs[Team::black];

class Fl_Piece : public Fl_Box {
	public:
		Fl_Piece(Piece* p):Fl_Box(p->position.col * 55, p->position.row * 55, 55, 55), piece(p) { 
			//cout << text(p->team);
			//cout << check_type(p->type);
			image(
					piece_pngs[p->team][p->type]
				 ); 
			color(0xff000000);
		}
		Piece* piece;
	private:
		int deltax = 0;
		int deltay = 0;
		//bool following_mouse = false;
		int handle(int event) override;};

int cntr = 0;

class Board_Window : public Fl_Window {
	public:
		Board_Window():Fl_Window(440,440,"Chess") {}
		Fl_Piece* moving = nullptr;
		vector<Coordinate> damaged {};
		void draw() override {
			cntr++;
			cout << "\nJust entered Board_Window's draw function. " << cntr << "\n";
			if (moving) {
				cout << "fl_not_clipped returned " << ( fl_not_clipped(moving->x(),moving->y(),moving->w(),moving->h()) ? "true.\n" : "false.\n" );
			}
			int cnt = 0;
			for (int row = 0; row <= 7 /*&& cnt < damaged.size()*/; ++row) {
				for (int col = 0; col <= 7 /*&& cnt < damaged.size()*/; ++col) {
					fl_color( Fl_Color(((row+col)%2) == 0 ? 60 : 38) );
					fl_rectf(row*55,col*55,55,55);
					++cnt;
				}
			}
			cout << "About to call draw_children() from within Board_Window's draw function.\n";
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			draw_children();
			cout << "Back from draw_children() from within Board_Window's draw function.\n";
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
};

Board_Window* W = new Board_Window();

Piece* create_piece(Piece_type pt, Coordinate pos, Team tm) {
	Piece* p;
	switch (pt) {
		case Piece_type::Rook:
			p = new Rook(pos, tm);
			break;
		case Piece_type::Bishop:
			p = new Bishop(pos, tm);
			break;
		case Piece_type::Knight:
			p = new Knight(pos, tm);
			break;
		case Piece_type::Queen:
			p = new Queen(pos, tm);
			break;
		case Piece_type::King:
			p = new King(pos, tm);
			break;
		case Piece_type::Pawn:
			p = new Pawn(pos, tm);
			break;
	}
	Fl_Piece* flp = new Fl_Piece(p);
	flp->show();
	W->add(flp);
	return p;
}

Board B;

string text(Team t, bool cap = true) {
	return (t == Team::white ? (cap ? "White" : "white") : (cap ? "Black" : "black"));
}

/*string check_type(Piece_type pt) {
	switch (pt) {
		case Piece_type::Knight:
			cout << "Knight";
			break;
		case Piece_type::Bishop:
			cout << "Bishop";
			break;
		case Piece_type::Queen:
			cout << "Queen";
			break;
		case Piece_type::King:
			cout << "King";
			break;
		case Piece_type::Pawn:
			cout << "Pawn";
			break;
		case Piece_type::Rook:
			cout << "Rook";
			break;
	}
}*/



Path Board::search_path(Path_type path_type, Coordinate search_from, Team mover_team, bool finding_threat, bool need_path) {
	int count;
	vector<Delta> vd{};
	vector<Delta> diagonals {{1,1},{1,-1},{-1,1},{-1,-1}};
	vector<Delta> non_diagonals {{1,0},{0,1},{-1,0},{0,-1}};
	switch (path_type) {
		case Path_type::Pawn:
			{
			int forward = (mover_team == Team::black ? 1 : -1);
			vd = {{forward,0},{forward*2,0},{forward,-1},{forward,1}};
			// Remove {forward*2,0} and make count conditional.
			count = 1;
			break;
			}
		case Path_type::Knight:
			vd = {{2,1},{1,2},{-2,1},{-1,2},{2,-1},{1,-2},{-2,-1},{-1,-2}};
			count = 1;
			break;
		case Path_type::Bishop:
			vd = diagonals;
			count = 7;
			break;
		case Path_type::Rook:
			vd = non_diagonals;
			count = 7;
			break;
		case Path_type::Queen:
			vd = combine(diagonals, non_diagonals);
			count = 7;
			break;
		case Path_type::King:
			vd = combine(diagonals, non_diagonals);
			count = 1;
			break;
	}
	vector<Coordinate> vr (vd.size(), search_from);
	vector<Path> paths (vd.size());
	bool remove = false;
	for (int cnt = 0; cnt < count; ++cnt) {
		for (int idx = 0; idx < static_cast<int>(vd.size()); ++idx, remove = false) {
			vr[idx] += vd[idx];
			if (vr[idx].on_board()) {
				Piece* piece = get_at(vr[idx]);
				if (finding_threat) {
					if (need_path) {
						paths[idx].push_back(vr[idx]);
					}
					if (piece) {
						if (piece->team != mover_team
							||
							(piece->type != path_type 
							 &&
							 ((path_type != Piece_type::Rook && path_type != Piece_type::Bishop) || piece->type != Piece_type::Queen))) {
							remove = true;
						}
						else {
							return (need_path ? paths[idx] : Path{vr[idx]});
						}
					}
				} else {
					if (piece && piece->team == mover_team) {
						remove = true;
					} else {
						pair<Piece*, bool> move_return = move(search_from, vr[idx]);
						Piece* moved_piece = get_at(vr[idx]);
						bool result_valid_return = moved_piece->result_valid(false);
						undo_move(search_from, vr[idx], move_return.first, move_return.second);
						if (result_valid_return) {
							return Path{vr[idx]};
						}
					}
				}
			} else {
				remove = true;
			}
			if (remove) {
				vr.erase(vr.begin()+idx);
				vd.erase(vd.begin()+idx);
				paths.erase(paths.begin()+idx);
				--idx;
			}
		}
	}
	return Path{};
}

bool Piece::path_valid(Path p) {
	auto p_b = p.begin();
	auto p_e = p.end();
	auto l = p.back();
	Delta d = distance_to(l);
	Piece* dest_occ = B[l.row][l.col];
	if ((dest_occ == nullptr && any_of(barred_if_not_opp->begin(), barred_if_not_opp->end(), [&](Delta el) { return el == d; }))
	   ||
	   (dest_occ != nullptr && (dest_occ->team == team || any_of(barred_if_opp->begin(), barred_if_opp->end(), [&](Delta el) { return el == d; })))) {
		return false;
	}
	p.pop_back();
	for (auto coord : p) {
		Piece* occupant = B.get_at(coord);
		if (occupant != nullptr) {
			return false;
		}
	}
	return true;
}

bool Piece::result_valid(bool modify_lane) {
	bool ret = false;
	Piece* first_piece = (B.check_lane.size() > 0 ? B.get_at(B.check_lane.front()) : nullptr);
	if (B.check_lane.empty()) {
		if (B.is_threatened(!team, B.get_king(team), false).empty()) {
			ret = true;
		}
	} else if ((not (B.check_lane.size() > 1 && first_piece && first_piece->type == Piece_type::King && first_piece->team == team))
			   &&
			   (any_of(B.check_lane.begin(), B.check_lane.end(), [&](Coordinate el) { return (B.get_at(el) && B.get_at(el)->team == team); }) 
			    ||
			    B.get_at(B.check_lane.front()) == nullptr)
		       &&
		       B.is_threatened(!team, B.get_king(team), false).empty()) {
		ret = true;
	}
	if (ret && modify_lane) {
		B.check_lane = B.is_threatened(team, B.get_king(!team), true);
	}
	return ret;
}

int extract(int idx, smatch sm) {
	return stoi(sm[idx].str());
}

istream& my_getline(string& s)
{
retry:
	// If getline fails due to eof and cin isn't already connected to tty,
	// switch to it.
	if (!getline(cin, s) && !cin.bad() && cin.eof() && !isatty(STDIN_FILENO)) {
		static ifstream is;
		// Open stream on process' tty.
		is.open("/dev/tty", ios_base::in);
		// Redirect cin to the tty.
		cin.rdbuf(is.rdbuf());
		// getline should work now.
		goto retry;
	}
	// Idiosyncrasy: The stream we create sets eofbit but not failbit when
	// user hits Ctrl-D. Also, Ctrl-D has to be hit twice, and can't be the
	// first thing on the line...
	// TODO: Figure out why, but for now, just live with it...
	//if (cin.eof())
		//cin.setstate(ios_base::failbit);
	return cin;
}

int Fl_Piece::handle(int event) {
	static int oldx, oldy;
	int newx, newy;
	switch (event) {
		case FL_PUSH:
			deltax = Fl::event_x() - x();
			deltay = Fl::event_y() - y();
			W->moving = this;
			return 1;
		case FL_DRAG:
			//W->damaged = { {x()/55,y()/55}, {(x()+54)/55,y()/55}, {(x()+54)/55,(y()+54)/55}, {(x()+54)/55,(y()+54)/55} };
			//remove(W->damaged.begin(), W->damaged.end(), piece->position);
			// BPS: Damage the board...
			newx = Fl::event_x() - deltax;
			newy = Fl::event_y() - deltay;
			W->damage(FL_DAMAGE_ALL, newx, newy, 55, 55);
			W->damage(FL_DAMAGE_ALL, oldx, oldy, 55, 55);
			position(newx, newy);
			//cout << "About to call redraw() from within Fl_Piece's handle method.\n";
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			redraw();
			//cout << "Finished call to redraw() from within Fl_Piece's handle method.\n";
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			oldx = newx;
			oldy = newy;
			return 1;
		case FL_RELEASE:
			from = Coordinate(piece->position);
			to = Coordinate(Fl::event_x()%55,Fl::event_y()%55);
			Piece* piece = B.get_at(from);
			Piece* dest = B.get_at(to);
			Path r_d_v = piece->delta_valid(to);
			if (piece && piece->team == turn && !r_d_v.empty() && piece->path_valid(r_d_v)) {
				pair<Piece*, bool> r_m = B.move(from, to);
				if (piece->result_valid(true)) {
					int centerx = (x() + w()) / 2;
					int centery = (y() + h()) / 2;
					position((centerx/55)*55,(centery/55)*55);
					turn = !turn;
				} else {
					position(from.row*55,from.col*55);
					B.undo_move(from, to, r_m.first, r_m.second);
					return 1;
				}
			}
			if (B.check_lane.empty()) {
				if (B.stalemate_check(!turn)) {
					cout << "You've reached a stalemate. No one wins (or loses).";
					return 0;
				}
			} else if (B.checkmate_check(!turn)) {
				cout << "Checkmate. " << text(turn) << " wins.";
				return 0;
			} else {
				return 1;
			}
	}
}

int main(int argc, const char* argv[]) {
	W->end();
	W->show();
	Fl::run();
}

// vim:ts=4:sw=4:noet
