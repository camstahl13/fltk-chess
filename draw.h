#include <map>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl.H>

/*
using namespace std;


map<Team, map<Piece_type, char*>> piece_pngs {  { Team::black, {{Piece_type::Pawn, "black_checker.png"},
																{Piece_type::Knight, "black_checker.png"},
																{Piece_type::Bishop, "black_checker.png"},
																{Piece_type::Rook, "black_checker.png"},
																{Piece_type::Queen, "black_checker.png"},
																{Piece_type::King, "black_checker.png"}} },
												{ Team::white, {{Piece_type::Pawn, "black_checker.png"},
																{Piece_type::Knight, "black_checker.png"},
																{Piece_type::Bishop, "black_checker.png"},
																{Piece_type::Rook, "black_checker.png"},
																{Piece_type::Queen, "black_checker.png"},
																{Piece_type::King, "black_checker.png"}} }  };
class Fl_Piece : Fl_PNG_Image {
	Fl_Piece(Piece* p, int abscissa, int ordinate):Fl_PNG_Image(piece_pngs[p->team][p->type]), x(abscissa), y(ordinate), piece(p) {};
	int x;
	int y;
	Piece* piece;
	bool following_mouse = false;
	int handle(int event) {
		if (handling) {
			switch (event) {
				case FL_PUSH:
					following_mouse = true;
					return 1;
				case FL_DRAG:
					if (following_mouse) {
						x = Fl::event_x();
						y = Fl::event_y();
					}
					return 1;
				case FL_RELEASE:
					following_mouse = false;
					from = Coordinate(p->position);
					to = Coordinate(x%50,y%50);
					handling = false;
					return 1;
			}
	}
}
*/
// vim:ts=4:sw=4:noet
