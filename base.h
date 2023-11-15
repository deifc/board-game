#include <bits/stdc++.h>
#include "engine.h"
using namespace std;
using namespace engine;
class Item;
class Board;
class Player;
class Block;
class Coordinate{
	public:
	int x, y;
	Coordinate(){
		x = y = 0;
	}
	Coordinate(int u, int v): x(u), y(v){}
};
Coordinate operator + (const Coordinate &u, const Coordinate &v){
	return Coordinate(u.x + v.x, u.y + v.y);
}
Coordinate operator - (const Coordinate &u, const Coordinate &v){
	return Coordinate(u.x - v.x, u.y - v.y);
}
bool operator == (const Coordinate &u, const Coordinate &v){
	return u.x == v.x && u.y == v.y;
}
class Block{
	public:
	string info;
	int rot = 0;// rot % 4 == now_dir
	bool available_base[4];//0-up 1-right 2-down 3-left
	bool movable = true;
	bool has_effect = false;
	std::function<void()> effect;
	bool available(int dir);
	void do_rotate(int dir);
};
const int BLOCKS_NUMBER = 1;
const int BOARD_SIZE = 5;
const int DIR_UP = 0, DIR_RIGHT = 1, DIR_DOWN = 2, DIR_LEFT = 3;
const Coordinate MOVE[4] = {Coordinate(-1, 0), Coordinate(0, 1), Coordinate(1, 0), Coordinate(0, -1)};
// const int MAX_PLAYERS = 4;
// const int SCORE_TO_0 = MAX_PLAYERS;
// int now_state;
class Board{
	public:
	Block blocks[BOARD_SIZE][BOARD_SIZE];
	bool visible[BOARD_SIZE][BOARD_SIZE];
	Block* coo_to_block(Coordinate pos);
	bool check_legal(Coordinate pos);
	bool check_adjacent(Coordinate pos1, Coordinate pos2);
} board;
class Player{
	public:
	string name;
	int score, strength, luck;
	int move_remain, swap_remain, rotate_remain, observe_remain, push_remain;
	Coordinate pos;
	std::vector<Item*> items;
	std::list<Player*>::iterator iter;
	Player();
	void generate_actions();
	bool do_move(Coordinate target);
	bool do_swap(Coordinate target);
	bool do_rotate_clockwise(Coordinate target);// target == pos
	bool do_rotate_anticlock(Coordinate target);// target == pos
	bool do_observe(Coordinate target);
	bool do_push(Player* target);
};
class Item{
	public:
	string info;
	std::function<void(Player*)> fun;
};
namespace MENU_PAGE{
	sprite page(nullptr);
	button start_button(nullptr), exit_button(nullptr);
	void init();
	void main();
	void start_call_back(displayObject *button);
	void exit_call_back(displayObject *button);
	void clear();
}
namespace SETTING_PAGE{
	int player_number;
	sprite page(nullptr);
	button add_player_button(nullptr), dec_player_button(nullptr);
	button start_button(nullptr);
	void init();
	void main();
	void start_call_back(displayObject *button);
	void add_player_call_back(displayObject *button);
	void dec_player_call_back(displayObject *button);
	void clear();
}
namespace PLAY_PAGE{
	list<Player*> player_list;
	list<Player*>::iterator player_in_turn;
	void init();
	void start_game();
	void enter_next_turn();
	void out_game(Player *loser);
	void win(Player *winner);
	void clear();
	Block* select_block(std::function<bool(Block*)>);
	Player* select_player(std::function<bool(Player*)>);
	void clear();
}