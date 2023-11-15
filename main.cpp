#include <bits/stdc++.h>
#include "base.h"
using namespace std;
using namespace engine;
bool Block::available(int dir){ return available_base[(dir + rot) % 4]; }
void Block::do_rotate(int dir){ rot += dir + 4; }

Block* Board::coo_to_block(Coordinate pos){
	return &blocks[pos.x][pos.y];
}
bool Board::check_legal(Coordinate pos){
	return (pos.x >= 0 && pos.y < BOARD_SIZE && pos.y >= 0 && pos.y < BOARD_SIZE);
}
bool Board::check_adjacent(Coordinate pos1, Coordinate pos2){
	Block *block1 = coo_to_block(pos1);
	Block *block2 = coo_to_block(pos2);
	for (int dir = 0; dir < 4; dir++){
		int dir_op = (dir + 2) % 4;
		if (pos2 == pos1 + MOVE[dir] && block1->available(dir) && block2->available(dir_op)) return true;
	}
	return false;
}

Player::Player(){
}
bool Player::do_move(Coordinate target){
	//TODO:display something
	if (move_remain <= 0) return false;
	if (!board.check_legal(target) || !board.check_adjacent(pos, target)) return false;
	pos = target;
	move_remain--;
	return true;
}
bool Player::do_swap(Coordinate target){
	//TODO:display something
	if (swap_remain <= 0) return false;
	if (!board.check_legal(target)) return false;
	swap(*board.coo_to_block(pos), *board.coo_to_block(target));
	pos = target;
	swap_remain--;
	return true;
}
bool Player::do_rotate_clockwise(Coordinate target){
	//TODO:display something
	if (rotate_remain <= 0) return false;
	if (!(target == pos)) return false;
	board.coo_to_block(pos)->do_rotate(1);
	rotate_remain--;
	return true;
}
bool Player::do_rotate_anticlock(Coordinate target){
	//TODO:display something
	if (rotate_remain <= 0) return false;
	if (!(target == pos)) return false;
	board.coo_to_block(pos)->do_rotate(-1);
	rotate_remain--;
	return true;
}
void Player::generate_actions(){
	//TODO:
	static mt19937 mt_gen(time(0));
	static uniform_int_distribution<> dis(0, strength);
	move_remain = dis(mt_gen);
	swap_remain = dis(mt_gen);
	rotate_remain = dis(mt_gen);
	observe_remain = dis(mt_gen);
	push_remain = dis(mt_gen);
}

void MENU_PAGE::init(){
	page = stage.addSprite("page", "paget", {{1280, 720, 0xFFFFFF}});
	start_button = page.addButton("button", "start_game", {{250, 100, 0XC3C3C3}, {250, 100, 0X7F7F7F}}, "开始游戏");
	start_button.moveto(1280 / 2 - 125,320);
	start_button.addEventListener(MOUSE_CLICK, start_call_back);
	exit_button = page.addButton("button", "exit_game", {{250, 100, 0XC3C3C3}, {250, 100, 0X7F7F7F}}, "退出游戏");
	exit_button.moveto(1280 / 2 - 125,450);
	exit_button.addEventListener(MOUSE_CLICK, exit_call_back);
	page.setVisible(false);
}
void MENU_PAGE::main(){
	page.setVisible(true);
}
void MENU_PAGE::clear(){
	//TODO:clear page
	page.setVisible(false);
}
void MENU_PAGE::exit_call_back(displayObject *obj){
	clear();
	//TODO:quit()
}
void MENU_PAGE::start_call_back(displayObject *obj){
	clear();
	SETTING_PAGE::main();
}


void SETTING_PAGE::init(){
	//TODO:
	page = stage.addSprite("page", "setting page", {{1280, 720, 0xFFFFFF}});
	player_tab_width = 400, player_tab_height = 100;
	for (int i = 0; i < 3; i++){
		player_tab_pos[2 * i + 0] = Coordinate(640 - 90 - player_tab_width, 320 + i * 130);
		player_tab_pos[2 * i + 1] = Coordinate(640 + 90, 320 + i * 130);
	}
	for (int i = 0; i < MAX_PLAYER; i++){
		player_tab[i] = page.addSprite("player tab", "player tab " + to_string(i), {{player_tab_width, player_tab_height, 0XC3C3C3}});
		player_tab[i].moveto(player_tab_pos[i].x, player_tab_pos[i].y);
	}
	page.setVisible(false);
}
void SETTING_PAGE::main(){
	page.setVisible(true);
}
void SETTING_PAGE::clear(){
	//TODO:clear
}
void SETTING_PAGE::add_player_call_back(displayObject *button){
	PLAY_PAGE::player_list.push_back(new Player);
}
void SETTING_PAGE::dec_player_call_back(displayObject *button){
	PLAY_PAGE::player_list.push_back(new Player);
}
void SETTING_PAGE::start_call_back(displayObject *button){
	clear();
	PLAY_PAGE::start_game();
}


void PLAY_PAGE::init(){
}
void PLAY_PAGE::start_game(){
	for (Player *player : player_list){
		player->score = 2;
		player->strength = 2;
		player->luck = 2;
		player->pos = Coordinate((BOARD_SIZE + 1) / 2, (BOARD_SIZE + 1) / 2);
	}
	player_in_turn = --player_list.end();
	enter_next_turn();
}
void PLAY_PAGE::enter_next_turn(){
	//TODO:display something
	//TODO:delete old eventlistener
	if (++player_in_turn == player_list.end()) player_in_turn = player_list.begin();
	//TODO:add new eventlistener
}
void PLAY_PAGE::out_game(Player *loser){
	//TODO:display something
	player_list.erase(loser->iter);
	if (player_list.size() == 1) win(*player_list.begin());
}
void PLAY_PAGE::win(Player *winner){
	//TODO:display something
}
void PLAY_PAGE::clear(){
	for (Player *i : player_list) delete i;
	//TODO:
}
Block* PLAY_PAGE::select_block(std::function<bool(Block*)> filter){
	//TODO:add eventlistener
	//TODO:list of eventlistener
	//static list<event> event_list;
	Block *target = nullptr;
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if (filter(&board.blocks[i][j])){
				//TODO:display something
				//TODO:add eventlistener
				//belike:add_eventlistener([&]->return void(displayObject* obj){ target = xxx; });
			}
		}
	}
	//TODO:cancel selecting
	while (target == nullptr) std::this_thread::sleep_for(chrono::milliseconds(20));
	//TODO:clear eventlistener
	return target;
}
Player* PLAY_PAGE::select_player(std::function<bool(Player*)> filter){
	//TODO:add eventlistener
	//TODO:list of eventlistener
	//static list<event> event_list;
	Player *target = nullptr;
	for (Player *player_i : player_list){
		if (filter(player_i)){
			//TODO:display something
			//TODO:add eventlistener
		}
	}
	//TODO:cancel selecting
	while (target == nullptr) std::this_thread::sleep_for(chrono::milliseconds(20));
	//TODO:clear eventlistener
	return target;
}
int main(){
	init();
	MENU_PAGE::init();
	SETTING_PAGE::init();
	PLAY_PAGE::init();
	MENU_PAGE::main();
	run();
	return 0;
}