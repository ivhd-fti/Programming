﻿//#include "stdafx.h" //#include <date.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#define ERR   -1
#define EMPTY -1
#define WAITING 1
#define RUNNING 2

using json					= nlohmann::json; json users; //using namespace std;
std::string users_filename	= "users.json";
std::string	SERVER_IP		= "127.0.0.1"; int SERVER_PORT = 3000;
std::string GREET_MSG		= "You have been connected to the server";
std::string ALL_SLOTS_BUSY	= "Please try again later.";

int const max_players_on_server = 100;
int const max_games_on_server = max_players_on_server / 2;

enum Packet { //uint32_t
	p_Ok = 1,					//1  T4- Успешная операция
	p_Fail = 2,					//2  T4- Ошибка. проверка игровых данных не пройдена
	p_Yes = 3,					//3  T4-
	p_No = 4,					//4  T4-
	p_UnReady = 5,				//+5  T4- отменить готовность играть// он же Ready
	cp_Ping_request = 6,		//+6  T4- пинг запрос
	cp_Ping_reply = 7,			//+7  T4- пинг ответ
	p_Surrender = 8,			//8  T4- закончить игру и сдаться (выводит в общий канал)
	p_Logoff = 9,				//+9  Т4- покинуть сервер -> отрабатывает P_Surrender, если во время партии. del token
	p_DelAcc = 10,				//10 T4- БЕЗ ПОДТВЕРЖДЕНИЯ!! удалить игровой account
	p_Login = 11,				//+11 T4-T4-login-T4-password  коннект к серверу, используя login:password						  new token
	p_AlreadyConnected = 12,	//+12 can't connect. disconnect first and login again
	p_ConnectOk = 13,			//+13 
	p_Connect_Fail = 14,		//+14

	p_Reg = 15,					//+15 первичная регистрация на сервере.								  new token
	p_TurnRes = 16,				//16 - Выстрел игроком - результаты хода или ответ сервера на выстрел
	p_GetServerData = 17,		//+17 plr: запрос сколько игроков и их имена
	p_ChatMessage = 18,			//18 9-tmp посылка сообщения в чат (общий канал или сопернику)
	s_ObjChange = 19,			//19 какие-то изменения какого-то объекта
	s_SendServerData = 20,		//+20 srv: ответ сколько игроков и их имена
	s_ThisPlayerIsAlreadyConnected = 21, //+21 такой пользователь уже законнекчен
	s_AccCreated = 22,			//+22 экаунт создан
	s_AccInUse = 23,			//+23 экаунт занят
	s_YouAreNotConnected = 24,	//+24 экаунт занят
	s_AccountWasDeleted = 25,	//+25 экаунт удален
	s_AccountNotDeleted = 26,	//+26 Экаунт не удален
	s_SendField = 27,			//+27 Отсылка пустого поля игроку
	p_FieldRequest = 28,		//+28 Отсылка сгенерированного пустого поля игроку
	s_LoginFirst = 29,			//+29 Сначала залогиньтесь для этого действия
	p_SetFieldSize5 = 30,		//30 Перключиться на поле 5х5 и выслать его
	p_SetFieldSize10 = 31,		//31 Перключиться на поле 10х10 и выслать его
	s_FieldChangeRefuse = 32,	//32 В данный момент изменения поля невожзможны
	s_ShootFailed = 33,			// Выстрел не принят сервером: нет логина, или игры или ещё что-то
	//s_IncomingSoot = 34,		// int4T- -входящий выстрел от сервера Miss = 0,	Wounded,	Killed , WrongShoot, WinGameShoot
	s_UnreadyFailInGame = 35,	// REP: смена статуса невозможна : вы в игре> surrender
	s_UnreadyFailNoShips = 36,	// REP: смена на Ready невозможна: не все корабли расставлены
	s_YouAreNotInGame = 37,		// REP: вы не в игре: смена статуса
	s_YouAreInGame = 38,		// REP: Вы в игре	: смена статуса
	s_FailedToFindGame = 39,	// REP: если игра почему-то не найдена
	s_YourTurn = 40,			// COM: ваш ход, ходите
	s_YouWait = 41,				// COM: ожидайте своего хода
	s_YouWin = 42,				// COM: завершает игру. отсылается победителю
	s_YouLose = 43,				// COM: завершает игру. отсылается проигравшему

	s_SendCell_P = 52,			// COM: установить значение клетки у клиента на основном поле   x, y, val
	s_SendCell_O = 53,			// COM: установить значение клетки у клиента на запасном поле   x, y, val
	p_SendShip = 54,			//+ Игрок расставляет корабли, сервер проверяет
	s_CantPlaceShipHere = 55,	//+ Ошибка установки корабля
	s_SendShip = 56,			//+ Установить у Клиента корабль на поле клиента  x,y,type
	p_DelShip = 57,				// удаление корабля с поля						  x,y,type
	s_DelShip = 58,				//												  x,y,type
	s_CantDelShip = 59,			// REP: ошибка удаления корабля
	p_SetGroundPercent = 60,	//+ pCOM: установить % суши на карте.   T4-val4
	s_SetGroundError = 61,		//+ REP: посылается, в случае, если IN-Game ИЛИ val<0 or val>100. Иначе в ответ посылается новое поле
	s_SendScore = 62,			// отсылает игроку его текущие достижения
	s_SurrenderFail = 63,		// REP: сдача не возможна. вы не в игре.
	s_SurrenderWin = 64,		// REP: посылается игроку, одержавшему победу выходом опонента по surrender
};

bool SendMsg(std::string&, int); bool SendShort(int, Packet); bool SendLong(int, int,int,int);
///////////////////////////////////////////////////////////////////////////
// Json Users Database
bool ConfigSave(json& users_data) {
	std::ofstream users_file(users_filename);
	if (!users_file.is_open()) {
		std::cout << "Failed to save config.\n";    // some file create error
		return false;
	}
	users_file << users_data; users_file.close(); std::cout << "done.\n";
	return true;
}
bool JsonRead(json& users_data) {
	std::cout << "Opening file: " << users_filename << " ... ";
	std::ifstream json_file(users_filename);
	if (!json_file.is_open()) {
		std::cout << "Users database not found.";
		std::cout << " ... creating new ... ";
		users_data = json::parse(R"({"LatsUpdate": "17dec2020.4:15","Users" : [	["ivlord", "123", "17dec2020.4:15", 1000, 20	] ], "version": 7.0	})");

		json_file.close();	return ConfigSave(users_data);
	}

	std::cout << "reading ... ";
	try { json_file >> users_data; }
	catch (json::parse_error& e) {
		std::cout << "\nCritical error reading users file!" << e.what() << '\n';
		json_file.close();	return false;
	}
	json_file.close(); std::cout << "done.\n";
	std::cout << users_data.dump(4);
	return true;
}
int  FindUserInDB(std::string login = "") { // ищет пользователя в базе данных
	if (login != "") { // ["ivlord","123","17dec2020.4:15",1000,20]
		for (int i = 0; i < (int)users["Users"].size(); i++) {
			if (users["Users"][i][0].get<std::string>() == login) { return i; }
		}
	}
	return ERR; // User not found
}
bool CheckLogin(std::string login, std::string password) {
	int res = FindUserInDB(login);
	if (res == ERR) return false;												// login не найден в базе данных
	return (users["Users"][res][1].get<std::string>() == password);
}
bool AddUserToDB(std::string login, std::string password, int points = 0, int games = 0) {
	if (FindUserInDB(login) != -1) return false; // такой пользователь уже есть в базе данных
	json new_user = json::parse(R"(["", "", "17dec2020.4:15", 0, 0])");
	new_user[0] = login; new_user[1] = password;
	users["Users"].push_back(new_user);
	ConfigSave(users);
	std::cout << "User:" << new_user.dump() << " has been added to database.\n"; return true;
}
void DelUserFromDB(std::string login) {
	int res = FindUserInDB(login);
	if (res != ERR) {
		std::cout << "User:" << users["Users"][res].dump() 
			      << " has been deleted from database.\n";
		users["Users"].json::erase(res);
		ConfigSave(users);
	}
}
// CELL
enum cell_types { // 0b  ship_mask=1
	Free = 0,		// 0 не занятая клетка
	Ship,			// 1 неповрежденный корабль
	Broken_ship,	// 2 поврежденный корабль
	Cross = 4,		// 4 место куда не стреляли (Free), но нет смысла уже стрелять
	Hit = 8			// 8 Место куда стреляли но не попали
};// Возможные типы клеток:
class cell {
public:
	bool grnd;					// Тип поверхности: true=ground, false=sea
	cell_types type;			// Тип клетки
	int ship_id = EMPTY;		// указатель на корабль, если его часть есть в клетке или -1, если нет корабля
	cell(bool _grnd = true, cell_types _type = Free) { this->grnd = _grnd; this->type = _type; }
}; // виден только внутри класса field  -> v_t
std::ostream& operator<< (std::ostream& out, cell& _cell) { out << ((_cell.grnd) ? "=" : "~") << _cell.type; return out; }

// SHIP
struct offset { int add_x; int add_y; }; // смещение (x,y) от головной клетки корабля
struct ships_schem { int len; bool grnd; offset s[4]; }; // gnd-суша\вода, len-клеток, смещения на занятую клетку
ships_schem ship_list[22] = {
	1, false, {{0,0}, {0,0}, {0,0}, {0,0}}, //0 1-палубный вода
	1, true,  {{0,0}, {0,0}, {0,0}, {0,0}}, //1 1-палубный суша
	2, false, {{0,0}, {1,0}, {0,0}, {0,0}}, //2 
	2, false, {{0,0}, {0,1}, {0,0}, {0,0}}, //3
	2, true,  {{0,0}, {1,0}, {0,0}, {0,0}}, //4
	2, true,  {{0,0}, {0,1}, {0,0}, {0,0}}, //5

	3, false, {{0,0}, {0,1}, {0,2}, {0,0}}, //6    6w- ###   8g-## 9g-##
	3, false, {{0,0}, {1,0}, {2,0}, {0,0}}, //7    7w-#          #    #
	3, true,  {{0,0}, {1,0}, {1,1}, {0,0}}, //8       #
	3, true,  {{0,0}, {1,0}, {0,1}, {0,0}}, //9       #
	3, true,  {{0,0}, {0,1}, {1,1}, {0,0}}, //10            10g-#   11g- #
	3, true,  {{1,0}, {0,1}, {1,1}, {0,0}}, //11                ##      ##

	4, false, {{0,0}, {0,1}, {0,2}, {0,3}}, //12 13w- #      12w- ####
	4, false, {{0,0}, {1,0}, {2,0}, {3,0}}, //13      #   14g- ##  16g- #  18g- ##   20g- #
	4, true,  {{0,0}, {1,0}, {0,1}, {0,2}}, //14      #        #        #        #        #
	4, true,  {{0,0}, {1,0}, {2,0}, {2,1}}, //15      #        #       ##        #        ##
	4, true,  {{0,2}, {1,0}, {1,1}, {1,2}}, //16
	4, true,  {{0,0}, {0,1}, {1,1}, {2,1}}, //17 15g- ###   17g - #    19g-  #     21g - ###
	4, true,  {{0,0}, {1,0}, {1,1}, {1,2}}, //18        #         ###      ###           #
	4, true,  {{0,1}, {1,1}, {2,1}, {2,0}}, //19
	4, true,  {{0,0}, {0,1}, {0,2}, {1,2}}, //20
	4, true,  {{0,0}, {0,1}, {1,0}, {2,0}}, //21
};
struct ship { int _type = ERR; int _x; int _y; int len;  int idx; cell* cells[4]; }; //ship_list[_type].len
std::ostream& operator<< (std::ostream& out, ship& _ship) {
	out << "Ship type: " << _ship._type << " [" << _ship._x << "," << _ship._y << "] " << _ship.len << ", idx:" << _ship.idx << ", cells>> ";
	for (int i = 0; i < _ship.len; i++) { std::cout << *_ship.cells[i] << " "; }	std::cout << std::endl;	return out;
}
struct ships_left { int ships[5] = {ERR,0,0,0,0}; };	// ответ по количеству живых кораблей

enum shot_result { Miss = 0, Wounded, Killed, WrongShot, WinGameShot, LoseGameShot }; // результаты выстрела
ships_left fld_amounts[2] = { { ERR,2,2,0,0 } , { ERR,4,3,2,1 } }; // количество кораблей на полях 5х5 и 10х10

offset crosses_map[2][4] = { { {-1,-1}, {-1,1}, {1,-1}, {1,1} }, { {0,1}, {0,-1}, {1,0}, {-1,0} } };

// FIELD
class field {
	std::vector <std::vector<cell>> Field;

public:
	std::vector<ship> ships;
	int field_size = 5;
	int ground_index = 0; // 0

	field(int _ground_index = 50, bool fld_size = true) {
		if (fld_size) this->field_size = 10; // определяем размер поля
		this->ground_index = _ground_index;
		gen_fileld(); // clean_self();
	}
	bool not_in_field(int x, int y) {
		if ((uint32_t)y > Field.size() - 1)    return false;
		if ((uint32_t)x > Field[y].size() - 1) return false;
		return (x<0 or y<0 or x>field_size - 1 or y>field_size - 1);
	}
	int alive(int idx) { // подсчитывает сколько осталось живых клеток корабля
		if (idx > (int)ships.size()) return 0;
		int res = 0;
		for (int i = 0; i < ship_list[ships[idx]._type].len; i++) {
			if (ships[idx].cells[i]->type == Ship) res++;
		} return res;
	}
	ships_left have_ships() {	// по основноу поля игрока ships_left { int ships[5] = {ERR,1-шек,2-шек,3,4}; }
		ships_left res;			// возвращает кол-во кораблей по палубности
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (alive(i)) { res.ships[ships[i].len]++; }	}return res;
	}
	ships_left need_ships() {
		ships_left res = fld_amounts[field_size/5-1];// => недостающее кол-во кораблей по палубности для карты
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (alive(i)) { res.ships[ships[i].len]--; } } return res;
	}
	bool field_have_any_alive() {
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (alive(i)) { return true; }		// нашли чё-то живое
		} return false;							// all dead
	}
	ship check_create_ship(int _type, int x, int y) {
		// возвращает объект ship: ship._type = ERR, если нельзя поставить или real type, если можно
		int _x, _y; ship new_ship;

		ships_left check = this->need_ships();
		int type_len = ship_list[_type].len;

		// проверка не превышено ли количество таких кораблей на поле
		if (check.ships[type_len] >= fld_amounts[field_size / 5 - 1].ships[type_len]) { // сколько должно быть таких кораблей
			return new_ship; // возвращаем пустышку - поставить не удалось. превышено количество
		}

		// проходим по списку клеток корабля:  2, false, {{0,0}, {1,0}, {0,0}, {0,0}}, //2 
		for (int i = 0; i < ship_list[_type].len; i++) {
			// вычисляем координаты проверяемой клетки корабля из схемы
			_x = x + ship_list[_type].s[i].add_x; _y = y + ship_list[_type].s[i].add_y;
			new_ship.cells[i] = this->get_cell(_x, _y); // сохраняем указатель на клетку или nullptr
			if (new_ship.cells[i] == nullptr) return new_ship;   // 1) если клетка за пределами поля
			// 1) или корабль не совпадает по наземный\водный или 3) клетка уже занята => облом!!
			if (new_ship.cells[i]->grnd != ship_list[_type].grnd or new_ship.cells[i]->type != Free) return new_ship;
		}
		new_ship._type = _type; new_ship._x = x; new_ship._y = y; new_ship.len = ship_list[_type].len;
		// прописываем значения корабля и возвращаем его НЕ СТАВИМ НА ПОЛЕ!!! только проверка возможности установки
		return new_ship;
	}
	int add_ship_to_field(ship new_ship) {
		// добавляет корабль в список, и проставляет id корабля во всех клетках корабля для обратного поиска
		if (new_ship._type == ERR) return ERR; // такой корабль не поставить
		new_ship.idx = ships.size() - 1; // текущая длина вектора кораблей ==> позиция позиция корабля в векторе
		for (int c = 0; c < new_ship.len; c++) {// обрабатываем все клетки корабля
			new_ship.cells[c]->ship_id = new_ship.idx; new_ship.cells[c]->type = Ship;
			}
		ships.push_back(new_ship); // добавляем корабль в вектор. убавление из вектора невозможно - только полная зачистка
		return new_ship.idx;
	}
	bool del_ship(int _type, int x, int y) {
		bool res = false;
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (_type == ships[i]._type and x == ships[i]._x and y == ships[i]._y) { // нашли такой корабль. убираем
				
				for (int c = 0; c < ships[i].len; c++) {// обрабатываем все клетки корабля
					ships[i].cells[c]->ship_id = EMPTY; // затераем в клетках Id корабля
					ships[i].cells[c]->type = Free;		// меняем тип на Free - свободна
					}
				for (uint32_t sh = i; i < ships.size(); sh++) {  // идем по "поздним" кораблям в векторе
					ships[sh].idx = i;						// корректируем индекс корабля в векторе
					for (int c = 0; c < ships[sh].len; c++) {// обрабатываем все клетки корабля
						ships[i].cells[c]->ship_id = i;		// корректируем в клетках Id корабля
					}
				}

				ships.erase(ships.begin() + i); res = true; break;}	}
		return res;
	}
	ship print_ship(int idx) {
		std::cout << "type:" << ships[idx]._type << "\n"; return ships[idx];
	}
	void set_cell_type(int x, int y, cell_types _type) {
		if (not_in_field(x, y)) return;
		Field[y][x].type = _type;
	}
	void set_cell(int x, int y, bool _grnd, cell_types _type) {
		// установить новые значения для клетки поля
		if (not_in_field(x, y)) return;
		Field[y][x].grnd = _grnd; Field[y][x].type = _type;
	}
	cell* get_cell(int x, int y) {
		// возвращает указатель на клетку: //*cells[i] - print cell, if (cells[i]->type == Ship)
		if (not_in_field(x, y)) return nullptr;
		cell* tmp = &Field[y][x]; return tmp;
	}
	shot_result shot(int x, int y) {
		if (not_in_field(x, y)) return WrongShot;
		cell* _cell = this->get_cell(x, y); // указатель на клетку (x, y)
		cell_types ald_cell_type = _cell->type; shot_result ret_result = Miss;

		switch (ald_cell_type) {
		case Free: {} // сваливание на Cross, т.к. одинаковый результат хода
		case Cross: { this->set_cell(x, y, _cell->grnd, Hit);				break; }
		case Ship: {
			this->set_cell(x, y, _cell->grnd, Broken_ship);
			ret_result = (this->alive(_cell->ship_id)) ? Wounded : Killed;	break; }
		default: {} // попали в Hit, Broken_ship - ignore
		}
		if (ret_result != Miss) {
			//this->SetCrosses(); // ставим крестики если попали
			if (!this->field_have_any_alive()) {
				return WinGameShot; // конец игре 
			}
		}
		return ret_result; // Miss или Wounded/Killed. x,y есть, т.е. и сам ship в клетке есть
	}
	void print_field() {
		for (int y = 0; y < field_size; y++) {
			std::cout << "\n";
			for (int x = 0; x < field_size; x++) {
				std::cout << Field[y][x] << " ";
			}
		}
	}
	void copy_self_no_ships(field& fld) {
		fld.Field.clear();
		fld.Field.resize(field_size);
		for (int y = 0; y < field_size; y++) {
			fld.Field[y].resize(field_size);
			for (int x = 0; x < field_size; x++) {
				fld.set_cell(x, y, this->get_cell(x, y)->grnd, Free);
			}
		}
	}
	bool send_self_no_ships(int idx) {
		json snd_fld = json::array(); //json::object()
		for (int y = 0; y < field_size; y++) {
			json fld_row = json::array();
			for (int x = 0; x < field_size; x++) {
				fld_row.push_back((this->get_cell(x, y)->grnd) ? 1 : 0);
			}
			snd_fld.push_back(fld_row);
		}

		if (!SendShort(idx, s_SendField)) return false;
		std::string field_to_send = snd_fld.dump();
		if (!SendMsg(field_to_send, idx)) return false;
		return true;
	}
	// Крестики не отсылает пока клиенту
	void SetCrosses(int x, int y, bool full = false) {// прописывает вокруг x,y крестики там, куда уже нет смысла стрелять
		int _x, _y;									  // full=true, ставит ещё и вертикальным крестом
		if (Field[y][x].type != Free) return;
		bool set_cross = false;

		for (int fulls = 0; fulls < 2; fulls++) {
			for (int off = 0; off < 4; off++) {		//crosses_map[0][x].add_x 	crosses_map[0][x].add_y
				_x = x + crosses_map[fulls][off].add_x; _y = y + crosses_map[fulls][off].add_y;
				if (not_in_field(_x, _y)) continue;
				if ((Field[_y][_x].type == Broken_ship) or (Field[_y][_x].type == Ship)) {// есть объект
					set_cross = true; break; }
			}
			if (set_cross) { Field[y][x].type = Cross; break; }
			if (!full) break; // полная обвязка крестиками не нужна значит
		}
	}

	//private:
		// просматривает соседние клетки и рекомендует какой тип территорри поставить
	bool fld_count_around(int x, int y) {
		int gnd_cnt = 0; int wtr_cnt = 0;
		for (int _x = x - 1; _x < x + 2; _x++) {
			for (int _y = y - 1; _y < y + 2; _y++) {
				if (not_in_field(_x, _y)) continue;
				if (Field[y][x].type) gnd_cnt++;
				else wtr_cnt++;
			}
		}
		return gnd_cnt > wtr_cnt;
	}
	// генерация более-менее однородных территорий (острова и участки суши)
	void gen_fileld() {
		int total_ground = (field_size * field_size) * ground_index / 100;	// количество клеток суши
		int total_water = field_size * field_size - total_ground;			// количество клеток воды
		ships.clear();	Field.resize(field_size);
		bool gnd_type_set = true, res = true;
		for (int y = 0; y < field_size; y++) {
			Field[y].resize(field_size);
			for (int x = 0; x < field_size; x++) {
				res = fld_count_around(x, y);
				if (rand() % 100 <= 10) res = !res; // 10% шанс нарушить рекомендацию - можно менять
				if ((res and total_ground > 0) or (!res and total_water <= 0)) { gnd_type_set = true; total_ground--; }
				else { gnd_type_set = false; total_water--; }
				Field[y][x] = cell(gnd_type_set);
			}
		}
	}
};

// GAME
class game {
public:
	int gm_users[2] = { ERR,ERR };		// IDs законнектившихся пользователей
	bool fld_size;					// размер поля: true=10x10, false=5x5
	int whos_turn;					// Чей ход: индекс игрока 0 или 1
	int status = EMPTY;				// статус игры: EMPTY -1, WAITING 1, RUNNING 2

	game(bool _fld_size = true) {
		ReRoll();					// определяем, кто первый ходит
		this->fld_size = _fld_size;
	}
	void SwapTurn() { this->whos_turn = ((this->whos_turn) ? 0 : 1); }
	void ReRoll() { whos_turn = rand() % 2; }
	int whos_wait() {return ((whos_turn) ? 0 : 1);	}
	int HasFreePlace() {			// В игре есть свободное место? ==>index свободного места или ERR (-1)
		return (gm_users[0] != EMPTY) ? 0 : ((gm_users[1] != EMPTY) ? 1 : ERR);	}
	bool MyTurn(int idx) {			// проверяет сейчас ход idx игрока?
		return ((status == RUNNING) and (gm_users[whos_turn] == idx));	}
	bool ThisPlayerInThisGame(int idx) { // Проверяет игрок idx играет в этой игре?
		return ((gm_users[0] == idx) or (gm_users[1] == idx));	}
	int OponentID(int idx) {		// возвращает id опонента
		if (ThisPlayerInThisGame(idx)) {
			if      ((gm_users[0] != ERR) and (gm_users[0] != idx)) return gm_users[0];
			else if ((gm_users[1] != ERR) and (gm_users[1] != idx)) return gm_users[1];
		}
		return ERR;		// ==> ERR, если игрок вообще тут не играет
	}
	bool AddPlayer(int idx, bool _fld_size) {// Добавить игрока в игру. 
		// если ещё пустая игра ИЛИ размеры полей совпадают И есть место: добавляем
		if ((status = EMPTY) or (_fld_size == fld_size) ) {
			int plr_pos = this->HasFreePlace();
			if (plr_pos != ERR) {
				this->gm_users[plr_pos] = idx;
				this->fld_size = _fld_size;
				this->status = (this->status == EMPTY) ? WAITING : RUNNING;

				if (this->status == RUNNING) {		// инициализация старта игры
					this->ReRoll();					// жеребьёвка первого хода
					if (!SendShort(gm_users[whos_turn],   s_YourTurn)) return false;
					if (!SendShort(gm_users[whos_wait()], s_YouWait))  return false;
				}
			}
		}

		return false;
	}
};
game Gm[max_games_on_server];

///////////////////////////////////////////////////////////////////////////

enum plr_actions {
	Need_Login = 1,
	Wait_login,
};

class plr { //uint_fast16_t
public:
	int idx = EMPTY;						// индекс в базе законнектившихся
	bool need_login = true;					// Основные состояния клиента: залогинен ли или просто законнекчен
	bool in_game	= false;				// уже играет

	int field_size = 10, ground_index = 50; // Параметры мгрового поля: получаются от клиента

	std::string login = "";
	int battles = 0; int score = 0;
	game* gm = nullptr; //&game(true);

	SOCKET con;  // сокет обслуживания Клиента
	plr(SOCKET con = INVALID_SOCKET) { this->con = con; }	// (*this).con = con;
	field pfld = field(ground_index, field_size == 10);		// шаблон поля клиента
	field ofld = field(ground_index, field_size == 10);		// временное поле опонента

	bool CanShootNow() { // залогинен, готов, в игре, есть норм указатель на игру, и его ход
		return (!need_login and (in_game) and (gm != nullptr) and (gm->MyTurn(this->idx)));	}
	void CleanClient() {
		this->need_login = true;
		this->in_game = false;
		this->login = "";	this->battles = 0;	this->score = 0; this->field_size = 10;
	}

private:
	plr_actions act = Need_Login; // стартовое значение - требуется Login
};
plr Cli[max_players_on_server];

int  FindFreeIndex() {
	int res = ERR;
	for (int i = 0; i < max_players_on_server; i++) {
		if (Cli[i].idx == EMPTY) { res = i; break; }
	} return res;
}
int  FindFreeGame(bool game_size) {// возвращает или игру в которой уже есть один игрок и ждет, при этом
	int res = ERR;	int free = ERR;// совпадает размер поля игроков. Или просто пустую игру, в которой игрок будет первым
	for (int i = 0; i < max_games_on_server; i++) {
		if (Gm[i].status == EMPTY) { free = i; }
		if ((Gm[i].status == WAITING) and (Gm[i].fld_size == game_size)){
			res = i; break; }
	} 
	return ((res==ERR)? free : res); //если не нашлось игры с одним игроком, то новую открывает или ERR, если нет свободного слота (что вряд ли возможно)
}

bool EndGame(int idx) { // idx - индекс победителя
	game* _gm = Cli[idx].gm;

	if (!_gm->ThisPlayerInThisGame(idx)) return false; // странно. нет игрока этого в игре
	int opponent_id = ((_gm->gm_users[0] == idx) ? _gm->gm_users[1] : _gm->gm_users[0]);
	if (opponent_id == ERR) { return false; } // странно опонета нет. как мы сюда попали? )

	// добавляем победителю одну выигранную игру, и 5 или 10 очков за партию (зависит от размера). Проигравшему просто одну игру
	int win_ = FindUserInDB(Cli[idx].login);
	int win_score = users[win_][3].get<int>() + Cli[idx].field_size;
	int win_games = users[win_][4].get<int>() + 1;
	users[win_][3] = win_score;
	users[win_][4] = win_games;

	int lose_ = FindUserInDB(Cli[opponent_id].login); //["ivlord", "123", "17dec2020.4:15", 1000, 20]
	int lose_score = users[lose_][3].get<int>() + 1; // одно поощрительное очко проигравшему
	int lose_games = users[lose_][4].get<int>() + 1;
	users[lose_][3] = lose_score;
	users[lose_][4] = lose_games;

	ConfigSave(users); // сохраняем данные в конфиге. ничё не проверяем :))

	// отдаем игровой слот назад - чистим его. Отключаем игроков от сети.
	_gm->status = EMPTY;
	_gm->gm_users[0] = ERR; _gm->gm_users[1] = ERR;

	Cli[idx].in_game = false;	Cli[opponent_id].in_game = false;
	Cli[idx].gm = nullptr;		Cli[opponent_id].gm = nullptr;

	if (!SendShort(idx, s_SendScore) or SendLong(idx, win_score, win_games, 0)) return false; 	// 0-поменять на количество ходов в игре
	if (!SendShort(opponent_id, s_SendScore) or SendLong(opponent_id, win_score, win_games, 0)) return false;
	if (!SendShort(idx, s_YouWin) or !SendShort(opponent_id, s_YouLose))			 return false;

	return true;
}

//EMPTY - 1, WAITING 1, RUNNING 2

bool SendMsg(std::string& msg, int idx) {
	int msg_size = msg.size();
	if (send(Cli[idx].con, (char*)&msg_size, sizeof(int), NULL) != SOCKET_ERROR) {
		if (send(Cli[idx].con, msg.c_str(), msg_size, NULL) != SOCKET_ERROR)
			return true;
	}
	printf("SendMsg failed: %d. Connection closed.\n", WSAGetLastError());
	return false;
}
bool SendShort(int idx, Packet _packettype) {
	Packet packettype = _packettype;
	return send(Cli[idx].con, (char*)&packettype, sizeof(packettype), NULL) == sizeof(packettype);
} //SOCKET_ERROR=-1
bool SendLong(int idx, int x, int y, int par) { //shot_result
	return (SendShort(idx, (Packet)x) and SendShort(idx, (Packet)y) and SendShort(idx, (Packet)par));
}
int  REC(int idx, int op_res = 1) { // добавляет ошибку // SOCKET_ERROR - after receive
	if (op_res > 0) { return op_res; } // всё нормально получено, возвращаем сколько байт получено
	Cli[idx].idx = ERR; return 0; // ошибка при получении - вырубаем Клиента
}
int  ReadLenString(int idx, char*& txt) {// читает от клиента: int и string (длиной первого int)
	int txt_size;
	if (!REC(idx, recv(Cli[idx].con, (char*)&txt_size, sizeof(int), NULL))) {
		Cli[idx].idx = EMPTY; return ERR;
	}
	txt = new char[txt_size + 1]; txt[txt_size] = '\0';
	int read_size = REC(idx, recv(Cli[idx].con, txt, txt_size, NULL));
	if (!read_size or read_size != txt_size) {
		Cli[idx].idx = EMPTY; return ERR;
	}
	return txt_size;
}
struct login_password { bool res = false; char* L; char* P; };
login_password GetLoginData(int idx) {
	login_password LP;
	if (!ReadLenString(idx, LP.L)) return LP;
	ReadLenString(idx, LP.P);
	return LP;}

bool ProcessPacket(int idx, Packet packettype) {
	int msg_size;

	switch (packettype) {
	case p_SetGroundPercent: { // установить % суши на карте.   T4-val4
		int percent;
		if (!REC(idx, recv(Cli[idx].con, (char*)&percent, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (Cli[idx].in_game or Cli[idx].need_login or (percent<0) or (percent>100)) { // %-неправильно введен ИЛИ не залогинен или в игре уже. установка корабля невозможна
			if (!SendShort(idx, s_SetGroundError)) { return false; }}
		Cli[idx].ground_index = percent;
		Cli[idx].pfld.ground_index = percent;
		Cli[idx].ofld.ground_index = percent;
		Cli[idx].pfld.gen_fileld();
		if (!Cli[idx].pfld.send_self_no_ships(idx)) { return false; }
		std::cout <<"#"<< idx <<": Ground idx changed to:"<< percent << ". Field has been sent." << "\n";
		break;
	}
	case p_DelShip: {
		int x, y, type;
		if (!REC(idx, recv(Cli[idx].con, (char*)&x, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (!REC(idx, recv(Cli[idx].con, (char*)&y, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (!REC(idx, recv(Cli[idx].con, (char*)&type, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (Cli[idx].in_game or Cli[idx].need_login) { // не залогинен или в игре уже. установка корабля невозможна
			if (!SendShort(idx, p_Fail)) { return false; } }

		if (!Cli[idx].pfld.del_ship(type, x, y)) {
			if (!SendShort(idx, s_CantDelShip)) { return false; break; }}// не получилось удалить. может нет такого
		
		if (!SendShort(idx, s_DelShip)) { return false; }
		if (!SendLong(idx, x, y, type)) return false;
	}
	case p_SendShip: { // установка клиентом корабля на поле
		int x, y, type;
		if (!REC(idx, recv(Cli[idx].con, (char*)&x,    sizeof(int), NULL))) {Cli[idx].idx = EMPTY; return false;}
		if (!REC(idx, recv(Cli[idx].con, (char*)&y,    sizeof(int), NULL))) {Cli[idx].idx = EMPTY; return false;}
		if (!REC(idx, recv(Cli[idx].con, (char*)&type, sizeof(int), NULL))) {Cli[idx].idx = EMPTY; return false;}

		if (Cli[idx].in_game or Cli[idx].need_login) { // не залогинен или в игре уже. установка корабля невозможна
			if (!SendShort(idx, p_Fail)) { return false; }	}

		ship new_plr_ship = Cli[idx].pfld.check_create_ship(type, x, y);
		if (Cli[idx].pfld.add_ship_to_field(new_plr_ship)==ERR) {			// не можем поставить тут
			if (!SendShort(idx, s_CantPlaceShipHere)) { return false; }	}
		
		if (!SendShort(idx, s_SendShip)) {return false;}
		if (!SendLong(idx, x, y, type)) return false;
	}
	case p_UnReady: {
		if (Cli[idx].in_game) { // статус: в игре => два варианта уже есть противник и ещё нет
			if (Cli[idx].gm->OponentID(idx) != ERR) { // если в игре и есть уже опонент, то выход только через surender
				if (!SendShort(idx, s_UnreadyFailInGame)) return false; break; // отсылаем ошибку и выходим
			}
			// опонента нет пока можно выйти из игры.
			Cli[idx].gm->status   = EMPTY;
			Cli[idx].gm->gm_users[0] = ERR; Cli[idx].gm->gm_users[1] = ERR;
			Cli[idx].gm = nullptr;
			if (!SendShort(idx, s_YouAreNotInGame)) return false; 
			break; // отсылаем изменение статуса s_YouAreNotInGame
		}
		else { // НЕ в игре, подбираем ему игру
			ships_left check = Cli[idx].pfld.need_ships();
			if (check.ships[1] or check.ships[2] or check.ships[3] or check.ships[4]) { // не все корабли стоят
				if (!SendShort(idx, s_UnreadyFailNoShips)) { return false; }
			}

			int new_game = FindFreeGame(Cli[idx].field_size == 10);
			if (new_game == ERR) {return false;}
			if (!Gm[new_game].AddPlayer(idx, Cli[idx].field_size == 10)) {return false;}
			Cli[idx].gm = &Gm[new_game];
			Cli[idx].in_game = true;
		}
		break;
	}
	case cp_Ping_request: { // ответ на пинг со стороны Клиента
		if (!SendShort(idx, cp_Ping_reply)) return false; break; }
	case p_SetFieldSize5: {}
	case p_SetFieldSize10: {
		if ((Cli[idx].login == "") or (Cli[idx].in_game)) {
			if (!SendShort(idx, s_FieldChangeRefuse)) return false;
		}
		else {
			Cli[idx].field_size = (packettype == p_SetFieldSize10) ? 10 : 5;
			Cli[idx].pfld.field_size = (packettype == p_SetFieldSize10) ? 10 : 5;
			Cli[idx].ofld.field_size = (packettype == p_SetFieldSize10) ? 10 : 5;
			Cli[idx].pfld.gen_fileld();
			if (Cli[idx].pfld.send_self_no_ships(idx)) {
				std::cout << "Field has been sent." << "\n"; return true;
			}
		}
	}
	case p_FieldRequest: {
		if (Cli[idx].login != "") {
			Cli[idx].pfld.gen_fileld();
			if (Cli[idx].pfld.send_self_no_ships(idx)) {
				std::cout << "Field has been sent." << "\n"; return true;
			}
		}
		else {
			if (!SendShort(idx, s_LoginFirst)) return false;
			return true;
		}
		return false;
	}
	case p_GetServerData: {
		json cur_users = json::parse(R"([])");
		json one_user = json::parse(R"(["",0,0])");
		for (int i = 0; i < max_players_on_server; i++) {
			if (Cli[i].idx != EMPTY) {
				one_user[0] = Cli[i].login;
				one_user[1] = Cli[i].score;
				one_user[2] = Cli[i].battles;
				cur_users.push_back(one_user);
			}
		}
		if (!SendShort(idx, s_SendServerData)) return false;
		std::string player_list = cur_users.dump();
		if (!SendMsg(player_list, idx)) return false;
		std::cout << "Player list has been sent. Total connected: " << cur_users.size() << "\n";
		break;
	}
	case p_Logoff: { // обрывание игры!! надо
		if (Cli[idx].login != "") {
			if (!SendShort(idx, p_Logoff)) return false;
			std::cout << "Logoff ok: " << Cli[idx].login << "\n";
			Cli[idx].need_login = true;
		}
		else {
			if (!SendShort(idx, s_YouAreNotConnected)) return false;
			std::cout << "Try to logoff while not logged in. \n";
		}
		Cli[idx].CleanClient(); break; } // unlogin
	case p_DelAcc: {
		login_password LP = GetLoginData(idx);
		std::cout << "Del: " << LP.L << ":" << LP.P << "\n";
		if (CheckLogin(LP.L, LP.P)) {
			DelUserFromDB(LP.L);
			if (!SendShort(idx, s_AccountWasDeleted)) { delete[] LP.L; delete[] LP.P; return false; }
			std::cout << "Account has been deleted: " << LP.L << "\n";
			if (Cli[idx].login == LP.L) { // если ещё был законнекчен на этот экаунт
				if (!SendShort(idx, p_Logoff)) { delete[] LP.L; delete[] LP.P; return false; }
				std::cout << "Logoff ok: " << Cli[idx].login << "\n";
				Cli[idx].CleanClient();
			}
			delete[] LP.L; delete[] LP.P; return true;
		}
		else {
			SendShort(idx, s_AccountNotDeleted);
			delete[] LP.L; delete[] LP.P; return false;
		}
		break;
	}
	case p_Reg: { //s_AccInUse s_AccCreated
		login_password LP = GetLoginData(idx);
		//std::cout << LP.L << ":" << LP.P << "\n";

		if ((FindUserInDB(LP.L) == ERR) and (Cli[idx].idx != ERR)) { // login free
			AddUserToDB(LP.L, LP.P, 0, 0);
			if (!SendShort(idx, s_AccCreated)) { delete[] LP.L; delete[] LP.P; return false; }
			std::cout << "New account created: " << LP.L << ":" << LP.P << "\n";
		}
		else { // login busy
			if (!SendShort(idx, s_AccInUse)) { delete[] LP.L; delete[] LP.P; return false; }
			std::cout << "This login is busy: " << LP.L << "\n";
		}
		break;
	}
	case p_Login: { // логин Клиента
		int login_size;
		if (!REC(idx, recv(Cli[idx].con, (char*)&login_size, sizeof(int), NULL))) { Cli[idx].idx = -1; return true; } // длина login НЕ контролируется
		char* n_login = new char[login_size + 1]; n_login[login_size] = '\0';
		if (!REC(idx, recv(Cli[idx].con, n_login, login_size, NULL))) { delete[] n_login; Cli[idx].idx = -1; return true; }

		// Get password
		int password_size;
		if (!REC(idx, recv(Cli[idx].con, (char*)&password_size, sizeof(int), NULL))) { delete[] n_login; return true; } // длина login НЕ контролируется
		char* n_password = new char[password_size + 1]; n_password[password_size] = '\0';

		if (REC(idx, recv(Cli[idx].con, n_password, password_size, NULL))) {
			if (!Cli[idx].need_login) { msg_size = p_AlreadyConnected; } // посылаем что уже законнекчен
			else {

				bool no_connected = true; // нет такого законнекченного пользователя
				for (int i = 0; i < max_players_on_server; i++) {
					if (Cli[i].idx == -1 or Cli[i].login != n_login) continue;
					no_connected = false; break; // уже есть такой законнекченный пользователь
				}

				if (!no_connected) {
					msg_size = s_ThisPlayerIsAlreadyConnected;
					std::cout << "Double connect: " << n_login << ":" << n_password << "\n";
				}
				else if (CheckLogin(n_login, n_password)) {// логин и пароль ок
					Cli[idx].login = n_login; Cli[idx].need_login = false;
					msg_size = p_ConnectOk; // посылаем что логин успешен
					std::cout << "Login ok: " << n_login << ":" << n_password << "\n";
				}
				else {
					msg_size = p_Connect_Fail; // посылаем что логин не вышел
					std::cout << "Loign failed: " << n_login << ":" << n_password << "\n";
				}
			}
		}
		if (send(Cli[idx].con, (char*)&msg_size, sizeof(p_Fail), NULL) != SOCKET_ERROR) {}
		delete[] n_login; delete[] n_password;
		break;
	}
				
	case p_Surrender: {
		if (!Cli[idx].in_game or Cli[idx].gm->status == WAITING) { // не в игре или игра ещё не началась
			std::cout << "#" << idx << " surender not possible.\n";
			if (!SendShort(idx, s_SurrenderFail)) return false;
			break;
		}
		
		int opponent_id = (Cli[idx].gm)->OponentID(idx);
		if (opponent_id == ERR)				return false;	// чё-то нет противника
		if (Cli[opponent_id].idx == ERR)	return false;	// противник отпал с сервера?
		if (Cli[opponent_id].need_login)	return false;	// чё-то противник не залогинен вообще?

		if (!EndGame(opponent_id)) return false;
		break;
	}

	case p_TurnRes: {
		// не проиграл уже, находится в игре, залогинен, его ход сейчас.
		if (!Cli[idx].CanShootNow()) { if (!SendShort(idx, s_ShootFailed)) return false; break; }
		int x, y;						// size= 4 bytes получаем x,y координаты выстрела
		if (!REC(idx, recv(Cli[idx].con, (char*)&x, sizeof(int), NULL)) or (Cli[idx].idx != ERR)) { return false; break; }
		if (!REC(idx, recv(Cli[idx].con, (char*)&y, sizeof(int), NULL)) or (Cli[idx].idx != ERR)) { return false; break; }
		
		// Проверка опонента
		int opponent_id = Cli[idx].gm->OponentID(idx);
		if (opponent_id == ERR)				return false;	// чё-то нет противника
		if (Cli[opponent_id].idx == ERR)	return false;	// противник отпал с сервера?
		if (Cli[opponent_id].need_login)	return false;	// чё-то противник не залогинен вообще?

		shot_result turn_res = Cli[opponent_id].pfld.shot(x, y); //Miss/Wounded/Killed/WinGameShoot

		// отмечаем событие на своем запасном поле (поле опонента)
		if ((turn_res == Wounded) or (turn_res == Killed) or (turn_res == WinGameShot)) { // отмечаем, если попали
			Cli[idx].ofld.set_cell_type(x, y, Broken_ship);
			
			cell* _cell = Cli[opponent_id].pfld.get_cell(x, y); // ссылка не ту же клетку в pfld опонента
			ship shp = Cli[opponent_id].pfld.ships[_cell->ship_id]; // получаем корабль в pfld.ships противника
			int shp_type = shp._type; // получаем, что там за корабль
			
			int _x, _y; // крестики ставим ТОЛЬКО на полях ofld
			if (turn_res != Wounded){ // т.е. потоплен и нужно крестики по кругу ставить и по всем клеткам корабля
				for (int i = 0; shp.len; i++) {
					_x = x + ship_list[shp_type].s[i].add_x; _y = y + ship_list[shp_type].s[i].add_y;
					Cli[idx].ofld.SetCrosses(_x, _y, true);	} // если корабль потоплен, то по кругу корабля.
			}
			else {Cli[idx].ofld.SetCrosses(x, y, false); // если ранен крестики по диагоналям только
			}
			if (!SendShort(idx, s_SendCell_O) or (!SendLong(idx, x, y, Broken_ship))) return false;					// ofld idx клиента
			if (!SendShort(opponent_id, s_SendCell_P) or (!SendLong(opponent_id, x, y, Broken_ship))) return false; // ofld idx клиента
			if (!SendShort(opponent_id, s_YouWait)) return false;													// ход опонента
			if (!SendShort(idx, s_YourTurn))		return false;													// ход опонента
		}
		else {
			Cli[idx].ofld.set_cell_type(x, y, Hit); // Hit на своем запасном поле
			Cli[idx].gm->SwapTurn();				// смена хода
			if (!SendShort(idx, s_SendCell_O) or (!SendLong(idx, x, y, Hit))) return false;					// ofld idx клиента
			if (!SendShort(opponent_id, s_SendCell_P) or (!SendLong(opponent_id, x, y, Hit))) return false; // ofld idx клиента
			if (!SendShort(opponent_id, s_YourTurn)) return false;											// ход опонента
			if (!SendShort(idx, s_YouWait))			 return false;											// ход опонента
		} // расставляем крестики, регулируем, кто ходит

		// конец игры. закрываем игру. насчитываем очки
		if (turn_res == WinGameShot) {
			if (!EndGame(idx)) return false;
			break;
		}

		return true;
	}
	case p_ChatMessage: { int msg_size;
		if (!REC(idx, recv(Cli[idx].con, (char*)&msg_size, sizeof(int), NULL))) return false;
		std::cout << "P#" << packettype << ":" << msg_size << " bytes\n";
		char* msg = new char[msg_size + 1]; msg[msg_size] = '\0';
		if (!REC(idx, recv(Cli[idx].con, msg, msg_size, NULL))) { delete[] msg; return false; }
		std::cout << "chat> " << Cli[idx].login << "> " << msg << "\n";
		delete[] msg;
		break;
		//for (int i = 0; i < Counter; i++) {
		//	if (Cli[cli_index] == NULL) { continue; }
		//	send(Cli[i]->con, (char*)&msg_size, sizeof(int), NULL);
		//	send(Cli[i]->con, msg, msg_size, NULL);
		//}
	}
	default: {if (packettype < 0) return false; std::cout << "Unknown packet: " << packettype << std::endl; }
	}
	return true;
}

// HANDLER: Отдельный поток по обработке запросов Клиента
void ClientHandler(int idx) {
	Packet packettype;						// size= 4 bytes
	while (REC(idx, recv(Cli[idx].con, (char*)&packettype, sizeof(Packet), NULL)) and (Cli[idx].idx != EMPTY)) {
		ProcessPacket(idx, packettype);	std::cout << "P#" << packettype << " from:" << idx << "\n";
	}
	std::cout << "\n >connection closed for: " << idx << ", WSA_err=" << WSAGetLastError() << "\n===============\n\n";
	closesocket(Cli[idx].con); // закрывает соединение для Клиента  // delete Cli[cli_index]; // не обязательно
	Cli[idx].idx = EMPTY;	   // освобождает слот в списке соединений=>разрывает while, close thread&exit
}

void GameServer() {
	int max_games = max_players_on_server / 2;
	int gm_cnt = 0, plr_cnt = 0;
	using namespace std::chrono_literals;
	std::cout << "Game serve thread activated for " << max_games << " game slots.\n";
	while (true) {
		gm_cnt = 0; plr_cnt = 0;
		std::this_thread::sleep_for(2000ms);
		for (int i = 0; i < max_games; i++) {
			if (Gm->status != EMPTY) { gm_cnt++; }
		}
		for (int i = 0; i < max_players_on_server; i++) {
			if (Cli[i].idx != EMPTY) { plr_cnt++; }
		}
		//std::cout << "Connected: "<< plr_cnt << ", Active games: " << gm_cnt << "\n";
	}
}

std::ostream& operator<< (std::ostream& out, ships_left _ships) {
	out << "ships: ";
	for (int i = 0; i < 5; i++) {
		out << _ships.ships[i] << " ";
	}
	out << "\n";
	return out;
}


int main(int argc, char* argv[]) {

	//return 0;

	std::cout << "game :" << sizeof(game) << "\n\n";
	std::cout << "field:" << sizeof(field) << "\n\n";
	std::cout << "Cli  :" << sizeof(Cli) << "\n\n";
	std::cout << "plr  :" << sizeof(plr) << "\n\n";

	if (!JsonRead(users)) { std::cout << "Ошибка. База данных повреждена. Сервер остановлен."; return 1; }

	std::cout << "This copy R" << "" << "\n";
	//srand(time(NULL));

	WSAData wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData); // WORD DLLVersion = MAKEWORD(2, 1);
	if (res != NO_ERROR) {							// Ошибка запуска WSA
		printf("WSAStartup failed: %d\n", WSAGetLastError()); WSACleanup();	exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_family = AF_INET;

	SOCKET sListen = INVALID_SOCKET;
	sListen = socket(AF_INET, SOCK_STREAM, NULL);  // IPPROTO_TCP
	if (sListen == INVALID_SOCKET) {
		printf("Socket creation error: %u\n", WSAGetLastError()); WSACleanup(); exit(1);
	}

	res = bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	if (res == SOCKET_ERROR) { printf("Bind error %d\n", WSAGetLastError()); closesocket(sListen); WSACleanup(); exit(1); }

	std::cout << "Started Server@" << SERVER_IP << ":" << SERVER_PORT << "\n";
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) { printf("Listen fail error: %d\n", WSAGetLastError()); exit(1); }

	SOCKET In_Connection = INVALID_SOCKET;
	for (int i = 0; i < 100; i++) { Cli[i] = plr(INVALID_SOCKET); Cli[i].idx = -1; }

	HANDLE MainGameProcess = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)GameServer, (LPVOID)(NULL), NULL, NULL);
	int idx_new;
	while (true) {
		In_Connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr); // принимаем новое входящее соединение в новый сокет

		idx_new = FindFreeIndex();
		if ((In_Connection == INVALID_SOCKET) or (idx_new == -1)) { // ошибка соединения сокета или нет свободного слота для нового сокета
			printf("Accept connection failed. Error: %d\n", WSAGetLastError());
			closesocket(In_Connection);	continue;
		}					// закрываем сокет и продолжаем слушать входящие

		char* ip = inet_ntoa(addr.sin_addr);						// определяем IP адрес Клиента на всякий случай.
		printf("Accepted Connection for #%d, from : %s\n", idx_new, ip);

		Cli[idx_new].con = In_Connection;							// создаем Class нового игрока
		Cli[idx_new].idx = idx_new;
		Cli[idx_new].CleanClient();

		// создаем новый поток для обслуживания нового Клиента. В поток передаем только индекс слота Клиента
		// на всякий случай сохраняем Tread id
		HANDLE cc = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(idx_new), NULL, NULL);
	}	return 0; //system("pause");
}
//send only				// 10013 WSAEACCES		// 10055 WSAENOBUFS		// 10065 WSAEHOSTUNREACH
// recv&send  WSAGetLastError()
// 10004 WSAEINTR		// 10014 WSAEFAULT		// 10022 WSAEINVAL		// 10035 WSAEWOULDBLOCK	
// 10036 WSAEINPROGRESS	// 10038 WSAENOTSOCK	// 10040 WSAEMSGSIZE	// 10045 WSAEOPNOTSUPP
// 10050 WSAENETDOWN	// 10052 WSAENETRESET	// 10053 WSAECONNABORTED// 10054 WSAECONNRESET
// 10057 WSAENOTCONN	// 10058 WSAESHUTDOWN	// 10060 WSAETIMEDOUT	// 10093 WSANOTINITIALISED		
//cout << "\nadd cli:\n";
//for (int idx = 0; idx < 100; idx++) {
//	if (Cli[idx].idx!=-1) cout << "!" << Cli[idx].idx << "\n";
//}
//cout << "-----------------\n";
//class gameD {
//	int users[2];		// IDs законнектившихся пользователей
//	int ground_index;	// выбранный пользователями процент суши
//	bool fld_size;		// размер поля: true=10x10, false=5x5
//public:
//	field main_fld[2];
//	field opp_fld[2];
//
//	gameD(int u_id_1, int u_id_2, int _ground_index = 50, bool _fld_size = true) {
//		this->users[0] = u_id_1;			this->users[1] = u_id_2;
//		this->ground_index = _ground_index;	this->fld_size = _fld_size;;
//		this->main_fld[0] = field(ground_index, fld_size);
//		this->main_fld[1] = field(ground_index, fld_size);
//		this->opp_fld[0] = field(ground_index, fld_size);
//		this->opp_fld[1] = field(ground_index, fld_size);
//		main_fld[0].copy_self_no_ships(opp_fld[1]);
//		main_fld[1].copy_self_no_ships(opp_fld[0]);
//	}
//private:
//	field fld_no_ships(field fld) {
//		field oponent_fld = fld;
//		for (int y = 0; y < oponent_fld.field_size; y++) {
//			for (int x = 0; x < oponent_fld.field_size; x++) {
//				oponent_fld.set_cell(x, y, oponent_fld.get_cell(x, y)->grnd, Free);
//			}
//		}
//		return oponent_fld;
//	}
//};
// напрвляем сюда, после каждой операции recv: REC(cli_index, op_res, 1)
// Возвращает:  REC(cli_index, ANY, -1) ==> 0			set	Cli[cli_index].error_cnt = 0
//				REC(cli_index, ANY, 0)  ==> Cli[cli_index].error_cnt
//				REC(cli_index, op_res==0 or <0 , 1)  ==> drop-ает, если превышено кол-во ошибок
//                        или была критическая //10054
//							==> -1 если drop-нуло или кол-во ошибок

//login_password GetLoginData2(int idx) {
//	login_password LP;
//	int login_size;
//	if (!REC(idx, recv(Cli[idx].con, (char*)&login_size, sizeof(int), NULL))) {
//		Cli[idx].idx = EMPTY; return LP;
//	} // длина login НЕ контролируется
//	LP.L = new char[login_size + 1]; LP.L[login_size] = '\0';
//	if (!REC(idx, recv(Cli[idx].con, LP.L, login_size, NULL))) {
//		delete[] LP.L; Cli[idx].idx = EMPTY; return LP;
//	}
//	// Get password
//	int password_size;
//	if (!REC(idx, recv(Cli[idx].con, (char*)&password_size, sizeof(int), NULL))) {
//		delete[] LP.L; Cli[idx].idx = -1; return LP;
//	}
//	LP.P = new char[password_size + 1]; LP.P[password_size] = '\0';
//	if (!REC(idx, recv(Cli[idx].con, LP.P, password_size, NULL))) {
//		delete[] LP.L; delete[] LP.P; Cli[idx].idx = -1;
//	}
//	return LP;}
//int error_cnt = 0;	int max_errors = 10;

//// шлем стрелявшему выстрел
//if (!SendShort(idx, p_TurnRes)) return false;
//if (send(Cli[idx].con, (char*)&turn_res, sizeof(turn_res), NULL) != sizeof(turn_res)) return false;
//if (turn_res == Miss) Cli[idx].gm->SwapTurn(); // смена хода
//// шлем опоненту выстрел
//if (!SendShort(opponent_id, s_IncomingSoot)) return false; // тут перехват должен быть ИИ
//if (send(Cli[opponent_id].con, (char*)&turn_res, sizeof(turn_res), NULL) != sizeof(turn_res)) return false;

//crosses_map
//for (int y = 0; y < 2; y++) {
//	for (int x = 0; x < 4; x++) {
//		std::cout << "(" << crosses_map[y][x].add_x << "," << crosses_map[y][x].add_y << ")  ";
//	}
//	std::cout << "\n";
//}
//std::cout << "\n";
