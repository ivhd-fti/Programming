//
// Сервер для игры в "Морской бой" с измененными правилами.
// Для игры так же необходим клиент на Python.
//
// Автор: Мазлов Иван. КФУ ФТИ Симферополь. 201-1.
//
//
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
#include <ctime>


#define ERRor  -1
#define EMPTY -1
#define WAITING 1
#define RUNNING 2

using json = nlohmann::json; json users; //using namespace std;
std::string users_filename = "users.json";
std::string	SERVER_IP = "127.0.0.1"; int SERVER_PORT = 3000; // если и сервер и клиенты на одном компьютере (для тестирования)
//std::string	SERVER_IP = "192.168.0.107"; int SERVER_PORT = 3000; // если играть по локальной сети

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
	s_SendOponentField = 65,	// Присылается в момент старта игры: расположение суши противника
	s_WaitingOpponentJoin = 66, // REP: ожидаем пока найдется противник
	s_RefreshField_P = 67,		// COM: обновление поля игрока
	s_RefreshField_O = 68,		// COM: обновление поля оппонента
};

bool SendMsg(std::string&, int); bool SendShort(int, Packet); bool SendLong(int, int, int, int); int GetPfldCellType(int, int, int);
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
	return ERRor; // User not found
}

bool CheckLogin(std::string login, std::string password) {
	int res = FindUserInDB(login);
	if (res == ERRor) return false;												// login не найден в базе данных
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
	if (res != ERRor) {
		std::cout << "User:" << users["Users"][res].dump()
			<< " has been deleted from database.\n";
		users["Users"].json::erase(res);
		ConfigSave(users);
	}
}
// CELL
enum cell_types {	// 0b  ship_mask=1
	Free = 0,		// 0 не занятая клетка
	Ship,			// 1 неповрежденный корабль
	Broken_ship,	// 2 поврежденный корабль
	Cross = 4,		// 4 место куда не стреляли (Free), но нет смысла уже стрелять
	Hit = 8,		// 8 Место куда стреляли но не попали
	ErrCell = 128	// ошибка получения типа клетки
};// Возможные типы клеток:

class cell {
public:
	bool grnd;					// Тип поверхности: true=ground, false=sea
	cell_types type;			// Тип клетки
	int ship_id = EMPTY;		// указатель на корабль, если его часть есть в клетке или -1, если нет корабля
	cell(bool _grnd = true, cell_types _type = Free) { this->grnd = _grnd; this->type = _type; }
}; // виден только внутри класса field  -> v_t

// SHIP
struct offset { int add_x; int add_y; };					// смещение (x,y) от головной клетки корабля, которая считается координатой корабля

struct ships_schem { int len; bool grnd; offset s[4]; };	// len-занимет клеток , суша\вода gnd-true, {смещения на занятую клетку}

ships_schem ship_list[22] = {
	1, false, {{0,0}, {0,0}, {0,0}, {0,0}},		//0 1-палубный вода
	1, true,  {{0,0}, {0,0}, {0,0}, {0,0}},		//1 1-палубный суша
	2, false, {{0,0}, {1,0}, {0,0}, {0,0}},		//2 
	2, false, {{0,0}, {0,1}, {0,0}, {0,0}},		//3
	2, true,  {{0,0}, {1,0}, {0,0}, {0,0}},		//4
	2, true,  {{0,0}, {0,1}, {0,0}, {0,0}},		//5
	3, false, {{0,0}, {0,1}, {0,2}, {0,0}},		//7    6w-###   8g-## 9g-##
	3, false, {{0,0}, {1,0}, {2,0}, {0,0}},		//6    7w-#         #    #
	3, true,  {{0,0}, {-1,0}, {0,1}, {0,0}},	//8       #
	3, true,  {{0,0}, {1,0}, {0,1}, {0,0}},		//9       #      
	3, true,  {{0,0}, {0,-1}, {1,0}, {0,0}},	//10            10g-#   11g- #
	3, true,  {{0,0}, {-1,0}, {0,-1}, {0,0}},	//11                ##      ##

	4, false, {{0,0}, {0,1}, {0,2}, {0,3}},		//12 13w- #      12w- ####
	4, false, {{0,0}, {1,0}, {2,0}, {3,0}},		//13      #   14g- ##  16g- #  18g- ##   20g- #
	4, true,  {{0,0}, {1,0}, {0,1}, {0,2}},		//14      #        #        #        #        #
	4, true,  {{0,0}, {0,1}, {-1,0}, {-2,0}},	//15      #        #       ##        #        ##
	4, true,  {{0,0}, {-1,0}, {0,-1}, {0,-2}},	//16
	4, true,  {{0,0}, {0,-1}, {1,0}, {2,0}},	//17 15g- ###   17g - #    19g-  #     21g - ###
	4, true,  {{0,0}, {-1,0}, {0,1}, {0,2}},	//18        #         ###      ###           #
	4, true,  {{0,0}, {0,-1}, {-1,0}, {-2,0}},	//19
	4, true,  {{0,0}, {1,0}, {0,-1}, {0,-2}},	//20
	4, true,  {{0,0}, {0,1}, {1,0}, {2,0}},		//21
};

struct ship { int _type = ERRor; int _x; int _y; int len;  int idx; cell* cells[4]; }; //ship_list[_type].len

struct ships_left { int ships[5] = { ERRor,0,0,0,0 }; };	// ответ по количеству живых кораблей

enum shot_result { Miss = 0, Wounded, Killed, WrongShot, WinGameShot, LoseGameShot }; // результаты выстрела

ships_left fld_amounts[2] = { { ERRor,2,2,0,0 } , { ERRor,4,3,2,1 } }; // количество кораблей на полях 5х5 и 10х10

// {{смещения координат крестиков по диагоналям-4шт}, {координаты по вертикали и горизонтали} }
offset crosses_map[2][4] = { { {-1,-1}, {-1,1}, {1,-1}, {1,1} }, { {0,1}, {0,-1}, {1,0}, {-1,0} } };

// FIELD
class field {
public:
	std::vector <std::vector<cell>> Field;
	std::vector<ship> ships;
	int field_size = 5;
	int ground_index = 0; // 0

	field(int _ground_index = 50, bool fld_size = true) {
		if (fld_size) this->field_size = 10; // определяем размер поля
		this->ground_index = _ground_index;
		gen_fileld(); // clean_self();
	}
	
	bool not_in_field(int x, int y) {
		return !( (x >= 0) and (y >= 0) and (x < field_size) and (y < field_size));
	}
	
	int alive(int idx) { // подсчитывает сколько осталось живых клеток корабля у корбля с индексом idx
		if (idx > (int)ships.size()) return 0;
		int res = 0;
		for (int i = 0; i < ship_list[ships[idx]._type].len; i++) {
			if (ships[idx].cells[i]->type == Ship) res++;
		} return res;
	}

	bool field_have_any_alive() {
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (this->alive(i)) { return true; }		// нашли чё-то живое
		} return false;							// all dead
	}

	ships_left have_ships() {	// по основному поля игрока ships_left { int ships[5] = {ERRor,1-шек,2-шек,3,4}; }
		ships_left res;			// возвращает кол-во кораблей по палубности // будет нужна для AI
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (this->alive(i)) { res.ships[ships[i].len]++; }
		}return res;
	}
	
	ships_left need_ships() {
		ships_left res = fld_amounts[field_size / 5 - 1];// => недостающее кол-во кораблей по палубности для карты
		for (uint32_t i = 0; i < ships.size(); i++) {
			if (alive(i)) { res.ships[ships[i].len]--; }
		} return res;
	}

	ship check_create_ship(int _type, int x, int y) {
		// возвращает объект ship: ship._type = ERRor, если нельзя поставить или real type, если можно
		int _x, _y; ship new_ship;

		ships_left check = this->need_ships();
		int type_len = ship_list[_type].len;
		if (!check.ships[type_len]) {
			return new_ship;
		} // возвращаем пустышку - поставить не удалось. превышено количество

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
		if (new_ship._type == ERRor) return ERRor; // такой корабль не поставить
		new_ship.idx = ships.size(); // текущая длина вектора кораблей ==> позиция позиция корабля в векторе
		for (int c = 0; c < new_ship.len; c++) {// обрабатываем все клетки корабля
			new_ship.cells[c]->ship_id = new_ship.idx; new_ship.cells[c]->type = Ship;
		}
		ships.push_back(new_ship); // добавляем корабль в вектор. убавление из вектора невозможно - только полная зачистка
		return new_ship.idx;
	}
	
	void clear_field() {
		int fsize = Field.size();
		for (int y = 0; y < fsize; y++) {
			for (int x = 0; x < fsize; x++) {
				Field[y][x].ship_id = ERRor;
				Field[y][x].type = Free;
			}
		}
	}
	
	// будет использоваться для AI
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

				ships.erase(ships.begin() + i); res = true; break;
			}
		}
		return res;
	}
	
	void set_cell_type(int x, int y, cell_types _type) {
		if (not_in_field(x, y)) return;
		Field[y][x].type = _type;
	}
	
	cell_types get_cell_type(int x, int y) {
		return ( (not_in_field(x, y))? ErrCell : Field[y][x].type);
	}
	
	int get_cell_ship_id(int x, int y) {
		return Field[y][x].ship_id; // тут уже не делаем проверку на вылет за пределы поля - уже делали (просто помним об этом)
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
		shot_result ret_result = Miss;
		cell_types ald_cell_type = this->get_cell_type(x,y); 
		switch (ald_cell_type) {								// попали в Hit, Broken_ship - ignore
		case Free: {}											// сваливание на Cross, т.к. одинаковый результат хода
		case Cross: { set_cell_type(x, y, Hit		 ); break; }
		case Ship:  { set_cell_type(x, y, Broken_ship);
			// сколько живых клеток (у корабля с id из клетки) >0 => Wonded иначе смотрим, осталсись ли вообще живые корабли. 
			// если да=> Killed  иначе WinGameShot-все мертвы
			return ((this->alive(get_cell_ship_id(x,y))) ? Wounded : ((this->field_have_any_alive()) ? Killed : WinGameShot)); }
		}	
		return ret_result; // Miss или Wounded/Killed. x,y есть, т.е. и сам ship в клетке есть
	}
	
	bool send_self_no_ships(int idx, Packet pack = s_SendField) {
		json snd_fld = json::array(); //json::object()
		for (int y = 0; y < field_size; y++) {
			json fld_row = json::array();
			for (int x = 0; x < field_size; x++) {
				fld_row.push_back((this->get_cell(x, y)->grnd) ? 1 : 0);
			}
			snd_fld.push_back(fld_row);
		}

		if (!SendShort(idx, pack)) return false;
		std::string field_to_send = snd_fld.dump();
		if (!SendMsg(field_to_send, idx)) return false;
		return true;
	}

	void SetCrosses(int x, int y, bool full, int op_idx, int pl_idx) {// прописывает вокруг x,y крестики там, куда уже нет смысла стрелять

		if (not_in_field(x, y)) return;
		if ((Field[y][x].type != Ship) and (Field[y][x].type != Broken_ship)) return; // если тут уже что-то стоит (клетка не свободна), то крестика не нужно

		int _x, _y;									  // full=true, ставит ещё и вертикальным крестом
		//offset crosses_map[2][4] = { { {-1,-1}, {-1,1}, {1,-1}, {1,1} }, { {0,1}, {0,-1}, {1,0}, {-1,0} } };
		for (int fulls = 0; fulls < 2; fulls++) { // цикл по двум схемам крестико: диагональ и КРЕСТ
			for (int off = 0; off < 4; off++) { // цикл по 4-м смещениям в каждой схеме
				//crosses_map[0][x].add_x 	crosses_map[0][x].add_y
				_x = x + crosses_map[fulls][off].add_x;
				_y = y + crosses_map[fulls][off].add_y;
				if (not_in_field(_x, _y)) continue;

				if (GetPfldCellType(op_idx, _x, _y) == Free) {// есть объект
					Field[_y][_x].type = Cross;
					SendShort(op_idx, s_SendCell_P); SendLong(op_idx, _x, _y, Cross);
					SendShort(pl_idx, s_SendCell_O); SendLong(pl_idx, _x, _y, Cross);
				}
			}
			if (!full) break; // полная обвязка крестиками не нужна значит
		}
	}

	// просматривает соседние клетки и рекомендует какой тип территорри поставить
	bool fld_count_around(int x, int y) {
		int gnd_cnt = 0; int wtr_cnt = 0;
		for (int _x = x - 1; _x < x + 2; _x++) {
			for (int _y = y - 1; _y < y + 2; _y++) {
				if (not_in_field(_x, _y)) continue;
				if (Field[y][x].type) gnd_cnt++;
				else				  wtr_cnt++;
			}
		}
		return gnd_cnt > wtr_cnt;
	}
	
	// генерация более-менее однородных территорий (острова и участки суши) чтобы не было рандомной мозаики вместо поля
	void gen_fileld() {
		int total_ground = (field_size * field_size) * ground_index / 100;	// количество клеток суши
		int total_water = field_size * field_size - total_ground;			// количество клеток воды
		ships.clear();	Field.resize(field_size);
		bool gnd_type_set = true, res = true;
		for (int y = 0; y < field_size; y++) {
			Field[y].resize(field_size);
			for (int x = 0; x < field_size; x++) {
				res = fld_count_around(x, y);
				if (rand() % 100 <= 15) res = !res;							// 15% шанс нарушить рекомендацию - можно менять
				if ((res and total_ground > 0) or (!res and total_water <= 0)) { gnd_type_set = true; total_ground--; }
				else { gnd_type_set = false; total_water--; }
				Field[y][x] = cell(gnd_type_set);
			}
		}
	}
	// использовалось на стадии тестирования.
	//ship print_ship(int idx) { std::cout << "type:" << ships[idx]._type << "\n"; return ships[idx]; } // для отладки
};

// GAME
class game {
public:
	int gm_users[2] = { ERRor,ERRor };							// IDs законнектившихся пользователей
	bool fld_size;												// размер поля: true=10x10, false=5x5
	int whos_turn;												// Чей ход: индекс игрока 0 или 1
	int status = EMPTY;											// статус игры: EMPTY -1, WAITING 1, RUNNING 2

	game(bool _fld_size = true) {
		this->fld_size = _fld_size;
	}
	void SwapTurn() { this->whos_turn = ((this->whos_turn == 1) ? 0 : 1); }
	void ReRoll() { whos_turn = rand() % 2; }					// определяем, кто первый ходит
	int whos_wait() { return ((whos_turn == 1) ? 0 : 1); }
	int HasFreePlace() {										// В игре есть свободное место? ==>index свободного места или ERRor (-1)
		return (gm_users[0] == EMPTY) ? 0 : ((gm_users[1] == EMPTY) ? 1 : ERRor);
	}
	bool MyTurn(int idx) {										// проверяет сейчас ход idx игрока?
		return ((status == RUNNING) and (gm_users[whos_turn] == idx));
	}
	bool ThisPlayerInThisGame(int idx) {						// Проверяет игрок idx играет в этой игре?
		return ((gm_users[0] == idx) or (gm_users[1] == idx));
	}
	int OponentID(int idx) {									// возвращает id опонента
		if (ThisPlayerInThisGame(idx)) {
			if ((gm_users[0] != ERRor) and (gm_users[0] != idx)) return gm_users[0];
			else if ((gm_users[1] != ERRor) and (gm_users[1] != idx)) return gm_users[1];
		} return ERRor;											// ==> ERRor, если игрок вообще тут не играет
	}
	bool AddPlayer(int idx, bool _fld_size) {// Добавить игрока в игру. 
		// если ещё пустая игра ИЛИ размеры полей совпадают И есть место: добавляем
		if ((status == EMPTY) or (_fld_size == fld_size)) {
			int plr_pos = this->HasFreePlace();
			if (plr_pos != ERRor) {
				this->gm_users[plr_pos] = idx;
				this->fld_size = _fld_size;
				this->status = (this->status == EMPTY) ? WAITING : RUNNING;

				if (this->status == RUNNING) {					// инициализация старта игры
					this->ReRoll();								// жеребьёвка первого хода
					if (!SendShort(gm_users[whos_turn], s_YourTurn)) return false;
					if (!SendShort(gm_users[whos_wait()], s_YouWait))  return false;
				}
			}
		}
		return true;
	}
	void Clean_Self() {
		this->status = EMPTY;	this->gm_users[0] = ERRor;
		this->fld_size = true;	this->gm_users[1] = ERRor;
	}
};
game Gm[max_games_on_server];

///////////////////////////////////////////////////////////////////////////

class plr { //uint_fast16_t
public:
	int idx = EMPTY;								// индекс в базе законнектившихся
	bool need_login = true;									// Основные состояния клиента: залогинен ли или просто законнекчен
	bool in_game = false;								// уже в игре

	int field_size = 10, ground_index = 50;					// Параметры игрового поля: получаются от клиента

	std::string login = "";
	int battles = 0; int score = 0;
	game* gm = nullptr;										//&game(true);

	SOCKET con;												// сокет обслуживания Клиента
	plr(SOCKET con = INVALID_SOCKET) { this->con = con; }	// (*this).con = con;
	
	field pfld = field(ground_index, field_size == 10);		// шаблон поля клиента
	field ofld = field(ground_index, field_size == 10);		// временное поле опонента

	bool CanShootNow(int plr_idx) {							// залогинен, готов, в игре, есть норм указатель на игру, и его ход
		return (!need_login and (in_game) and (gm != nullptr) and (gm->MyTurn(plr_idx)));
	}
	void CleanClient(bool disconnect = true) {
		if (disconnect) { this->need_login = true; this->login = ""; }
		this->in_game = false;
		this->gm = nullptr;
		this->battles = 0;
		this->score = 0;
		this->pfld.clear_field();
		this->ofld.clear_field();
		this->field_size = 10;
		this->pfld.field_size = 10;
		this->ofld.field_size = 10;
		this->pfld.ships.clear();
		this->ofld.ships.clear();
	}
};
plr Cli[max_players_on_server];

int GetPfldCellType(int idx, int x, int y) {
	return Cli[idx].pfld.Field[y][x].type;
}

// поиск свободного слота для только что законнектившегося игрока
int  FindFreeIndex() {
	int res = ERRor;
	for (int i = 0; i < max_players_on_server; i++) {
		if (Cli[i].idx == EMPTY) { res = i; break; }
	} return res;
}

// поиск свободного слота игры для игрока (это или новая игра или подходящая, в которой уже ждет другой игрок)
int  FindFreeGame(bool game_size) {			// возвращает или игру в которой уже есть один игрок и ждет, при этом
	int res = ERRor;	int free = ERRor;	// совпадает размер поля игроков. Или просто пустую игру, в которой игрок будет первым
	for (int i = 0; i < max_games_on_server; i++) {
		if (Gm[i].status == EMPTY) { free = i; }
		if ((Gm[i].status == WAITING) and (Gm[i].fld_size == game_size)) { res = i; break; }
	}
	return ((res == ERRor) ? free : res);	// если не нашлось игры с одним игроком, то новую открывает или ERRor, если нет свободного слота (что вряд ли возможно)
}

// процедура завершения игры, раздачи очков, и зачистки слота игры, рассылки окончательных сообщений.
bool EndGame(int idx, int sur_win = 0) {	// idx - индекс победителя
	game* _gm = Cli[idx].gm;

	if (!_gm->ThisPlayerInThisGame(idx)) return false;	// странно. нет игрока этого в игре
	int opponent_id = ((_gm->gm_users[0] == idx) ? _gm->gm_users[1] : _gm->gm_users[0]);
	if (opponent_id == ERRor) { return false; }			// странно опонета нет. как мы сюда попали? )
	// добавляем победителю одну выигранную игру, и 5 или 10 очков за партию (зависит от размера). Проигравшему просто одну игру
	int win_ = FindUserInDB(Cli[idx].login);
	int win_score = users["Users"][win_][3].get<int>() + Cli[idx].field_size; // даем столько очковЮ сколько размер поля.
	int win_games = users["Users"][win_][4].get<int>() + 1;
	users["Users"][win_][3] = win_score;	users["Users"][win_][4] = win_games;

	int lose_ = FindUserInDB(Cli[opponent_id].login);							//["ivlord", "123", "17dec2020.4:15", 1000, 20]
	int lose_score = (sur_win) ? 0 : (users["Users"][lose_][3].get<int>() + 1); // одно поощрительное очко проигравшему
	int lose_games = users["Users"][lose_][4].get<int>() + 1;
	users["Users"][lose_][3] = lose_score;	users["Users"][lose_][4] = lose_games;

	ConfigSave(users);															// сохраняем данные в конфиге. ничё не проверяем

	// отдаем игровой слот назад - чистим его. Отключаем игроков от сети.

	_gm->Clean_Self();
	Cli[idx].CleanClient(false); Cli[opponent_id].CleanClient(false);

	if (!SendShort(idx, s_SendScore) or !SendLong(idx, win_score, win_games, 60))    return false; 	// 15-поменять на количество ходов в игре

	if (sur_win) { if (!SendShort(idx, s_SurrenderWin)) return false; }
	else
	{
		if (!SendShort(opponent_id, s_SendScore) or !SendLong(opponent_id, lose_score, lose_games, 60)) return false;
		if (!SendShort(idx, s_YouWin) or !SendShort(opponent_id, s_YouLose))  return false;
	}

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

// отсылка однокомандных (4-х байтовых) пакетов
bool SendShort(int idx, Packet _packettype) {
	Packet packettype = _packettype;
	return send(Cli[idx].con, (char*)&packettype, sizeof(packettype), NULL) == sizeof(packettype);
} //SOCKET_ERROR=-1

// отсылка пакета с данными
bool SendLong(int idx, int x, int y, int par) { //shot_result
	return (SendShort(idx, (Packet)x) and SendShort(idx, (Packet)y) and SendShort(idx, (Packet)par));
}

// проверка адекватности приема данных
int  REC(int idx, int op_res = 1) { // добавляет ошибку // SOCKET_ERROR - after receive
	if (op_res > 0) { return op_res; } // всё нормально получено, возвращаем сколько байт получено
	Cli[idx].idx = ERRor; return 0; // ошибка при получении - вырубаем Клиента
}

// прием пакета большой длины (обычно текст). логины, пароли, сообщения чата.
int  ReadLenString(int idx, char*& txt) {// читает от клиента: int и string (длиной первого int)
	int txt_size;
	if (!REC(idx, recv(Cli[idx].con, (char*)&txt_size, sizeof(int), NULL))) {
		Cli[idx].idx = EMPTY; return ERRor;
	}
	txt = new char[txt_size + 1]; txt[txt_size] = '\0';
	int read_size = REC(idx, recv(Cli[idx].con, txt, txt_size, NULL));
	if (!read_size or read_size != txt_size) {
		Cli[idx].idx = EMPTY; return ERRor;
	}
	return txt_size;
}

struct login_password { bool res = false; char* L; char* P; };

login_password GetLoginData(int idx) {
	login_password LP;
	if (!ReadLenString(idx, LP.L)) return LP;
	ReadLenString(idx, LP.P);
	return LP;
}

// Обработчик пакетов, принимаемых сервером
bool ProcessPacket(int idx, Packet packettype) {
	int msg_size;

	switch (packettype) {
	case p_SetGroundPercent: { // установить % суши на карте.   T4-val4
		int percent;
		if (!REC(idx, recv(Cli[idx].con, (char*)&percent, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (Cli[idx].in_game or Cli[idx].need_login or (percent < 0) or (percent > 100)) { // %-неправильно введен ИЛИ не залогинен или в игре уже. установка корабля невозможна
			if (!SendShort(idx, s_SetGroundError)) { return false; }
		}
		Cli[idx].ground_index = percent;
		Cli[idx].pfld.ground_index = percent;
		Cli[idx].ofld.ground_index = percent;
		Cli[idx].pfld.gen_fileld();
		Cli[idx].pfld.clear_field();
		Cli[idx].ofld.clear_field();

		if (!Cli[idx].pfld.send_self_no_ships(idx)) { return false; }
		std::cout << "#" << idx << ": Ground idx changed to:" << percent << ". Field has been sent.3 " << "\n";
		break;
	}

	// не используется
	case p_DelShip: {
		int x, y, type;
		if (!REC(idx, recv(Cli[idx].con, (char*)&x, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (!REC(idx, recv(Cli[idx].con, (char*)&y, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (!REC(idx, recv(Cli[idx].con, (char*)&type, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		if (Cli[idx].in_game or Cli[idx].need_login) { // не залогинен или в игре уже. установка корабля невозможна
			if (!SendShort(idx, p_Fail)) { return false; }
		}

		if (!Cli[idx].pfld.del_ship(type, x, y)) {
			if (!SendShort(idx, s_CantDelShip)) { return false; break; }
		}// не получилось удалить. может нет такого

		if (!SendShort(idx, s_DelShip)) { return false; }
		if (!SendLong(idx, x, y, type)) return false;
	}

	case p_SendShip: { // установка клиентом корабля на поле 
		int x, y, type;
		std::cout << "Get ship form player:\n";
		if (!REC(idx, recv(Cli[idx].con, (char*)&x, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		std::cout << x << "+ ";
		if (!REC(idx, recv(Cli[idx].con, (char*)&y, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		std::cout << y << "+ ";
		if (!REC(idx, recv(Cli[idx].con, (char*)&type, sizeof(int), NULL))) { Cli[idx].idx = EMPTY; return false; }
		std::cout << type << "+";

		if (Cli[idx].in_game or Cli[idx].need_login) { // не залогинен или в игре уже. установка корабля невозможна
			if (!SendShort(idx, p_Fail)) { return false; }
			std::cout << "ERR:in_game or not_logged+\n";
			return true;
		}

		ship new_plr_ship = Cli[idx].pfld.check_create_ship(type, x, y);
		if (Cli[idx].pfld.add_ship_to_field(new_plr_ship) == ERRor) {			// не можем поставить тут
			if (!SendShort(idx, s_CantPlaceShipHere)) { return false; }
			std::cout << "bad place+\n";
			return true;
		}


		if (!SendShort(idx, s_SendShip)) { return false; }
		if (!SendLong(idx, x, y, type)) return false;
		std::cout << "ok\n";
		break;
	}

	case p_UnReady: {
		if (Cli[idx].in_game) {						  // статус: в игре => два варианта уже есть противник и ещё нет
			if (Cli[idx].gm->OponentID(idx) != ERRor) { // если в игре и есть уже опонент, то выход только через surender
				if (!SendShort(idx, s_UnreadyFailInGame)) return false; break; // отсылаем ошибку и выходим
			}
			// опонента нет пока можно выйти из игры.
			Cli[idx].gm->status = EMPTY;
			Cli[idx].gm->gm_users[0] = ERRor; Cli[idx].gm->gm_users[1] = ERRor;
			Cli[idx].gm = nullptr;
			if (!SendShort(idx, s_YouAreNotInGame)) return false;
			Cli[idx].pfld.clear_field();
			Cli[idx].pfld.ships.clear();
			Cli[idx].in_game = false;
			break; // отсылаем изменение статуса s_YouAreNotInGame
		}
		else { // НЕ в игре, подбираем игру
			ships_left check = Cli[idx].pfld.need_ships();
			if (check.ships[1] or check.ships[2] or check.ships[3] or check.ships[4]) { // не все корабли стоят
				if (!SendShort(idx, s_UnreadyFailNoShips)) { return false; }
			}

			int new_game = FindFreeGame(Cli[idx].field_size == 10);
			if (new_game == ERRor) { // не удалось найти новую игру: слоты заняты или нет подходящего размера поля. редкий случай
				SendShort(idx, s_FailedToFindGame); return false;
			}

			if (!Gm[new_game].AddPlayer(idx, Cli[idx].field_size == 10)) { return false; }

			Cli[idx].gm = &Gm[new_game];
			Cli[idx].in_game = true;

			if (!SendShort(idx, s_YouAreInGame)) { return false; }
			if (Cli[idx].gm->HasFreePlace() != ERRor) { // ждем ещё одного опонента
				if (!SendShort(idx, s_WaitingOpponentJoin)) { return false; }
			}
			else {

				//Cli[idx].gm->ReRoll();
				//if (!SendShort(Cli[idx].gm->whos_turn, s_YourTurn))  { return false; }
				//if (!SendShort(Cli[idx].gm->whos_wait(), s_YouWait)) { return false; }

				int opp = Cli[idx].gm->OponentID(idx);
				if (!Cli[idx].pfld.send_self_no_ships(opp, s_SendOponentField)) { return false; }
				if (!Cli[opp].pfld.send_self_no_ships(idx, s_SendOponentField)) { return false; }
			}
		}
		break; // 
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
				std::cout << "Field has been sent.1" << "\n"; return true;
			}


			if (!Cli[idx].pfld.send_self_no_ships(idx, s_SendOponentField)) { return false; }

		}
	}

	case p_FieldRequest: {
		if (Cli[idx].login != "") {
			Cli[idx].pfld.gen_fileld();
			if (Cli[idx].pfld.send_self_no_ships(idx)) {
				std::cout << "Field has been sent.2" << "\n"; return true;
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

			if (Cli[idx].in_game) {
				int opponent_id = ((Cli[idx].gm->gm_users[0] == idx) ? Cli[idx].gm->gm_users[1] : Cli[idx].gm->gm_users[0]);
				if (opponent_id != ERRor) { EndGame(opponent_id, 1); }
			}
			//Cli[idx].CleanClient(); 	//Cli[idx].need_login = true;
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

		if ((FindUserInDB(LP.L) == ERRor) and (Cli[idx].idx != ERRor)) { // login free
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

	case p_Login: {					// логин Клиента
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
		if (opponent_id == ERRor)				return false;	// чё-то нет противника
		if (Cli[opponent_id].idx == ERRor)	return false;	// противник отпал с сервера?
		if (Cli[opponent_id].need_login)	return false;	// чё-то противник не залогинен вообще?

		if (!EndGame(opponent_id)) return false;
		break;
	}

	case p_TurnRes: {
		// не проиграл уже, находится в игре, залогинен, его ход сейчас.
		if (!Cli[idx].CanShootNow(idx)) { if (!SendShort(idx, s_ShootFailed)) return false; break; }
		int x, y;						// size= 4 bytes получаем x,y координаты выстрела
		if (!REC(idx, recv(Cli[idx].con, (char*)&x, sizeof(int), NULL)) or (Cli[idx].idx == ERRor)) { return false; break; }
		if (!REC(idx, recv(Cli[idx].con, (char*)&y, sizeof(int), NULL)) or (Cli[idx].idx == ERRor)) { return false; break; }

		// Проверка опонента
		int opponent_id = Cli[idx].gm->OponentID(idx);
		if (opponent_id == ERRor)			return false;	// чё-то нет противника
		if (Cli[opponent_id].idx == ERRor)	return false;	// противник отпал с сервера?
		if (Cli[opponent_id].need_login)	return false;	// чё-то противник не залогинен вообще?

		shot_result turn_res = Cli[opponent_id].pfld.shot(x, y); //Miss/Wounded/Killed/WinGameShoot

		// отмечаем событие на своем запасном поле (поле опонента)
		if ((turn_res == Wounded) or (turn_res == Killed) or (turn_res == WinGameShot)) { // отмечаем, если попали
			Cli[idx].ofld.set_cell_type(x, y, Broken_ship);

			cell* _cell = Cli[opponent_id].pfld.get_cell(x, y);		// ссылка не ту же клетку в pfld опонента
			ship shp = Cli[opponent_id].pfld.ships[_cell->ship_id]; // получаем корабль в pfld.ships противника
			int shp_type = shp._type;								// получаем, что там за корабль

			int _x, _y; // крестики ставим ТОЛЬКО на полях ofld
			int ex = shp._x; int ey = shp._y;

			if (turn_res != Wounded) { // т.е. потоплен и нужно крестики по кругу ставить и по всем клеткам корабля
				for (int i = 0; i < shp.len; i++) {
					_x = ex + ship_list[shp_type].s[i].add_x; _y = ey + ship_list[shp_type].s[i].add_y;
					Cli[idx].ofld.SetCrosses(_x, _y, true, opponent_id, idx);
				} // если корабль потоплен, то по кругу корабля.
			}
			else { Cli[idx].ofld.SetCrosses(x, y, false, opponent_id, idx); } // если ранен крестики по диагоналям только

			if (!SendShort(idx, s_SendCell_O) or (!SendLong(idx, x, y, Broken_ship))) return false;					// ofld idx клиента
			if (!SendShort(opponent_id, s_SendCell_P) or (!SendLong(opponent_id, x, y, Broken_ship))) return false; // ofld idx клиента
			if (!SendShort(opponent_id, s_YouWait))  return false;					// опонента промазал и ждет
			if (!SendShort(idx, s_YourTurn)) return false;					// ход клиента
		}
		else {	//Field																	// расставляем крестики, регулируем, кто ходит
			if (Cli[idx].ofld.Field[y][x].type != Broken_ship) {
				Cli[idx].ofld.set_cell_type(x, y, Hit);									// Hit на своем запасном поле
				if (!SendShort(idx, s_SendCell_O) or (!SendLong(idx, x, y, Hit))) return false; // ofld idx клиента
				if (!SendShort(opponent_id, s_SendCell_P) or (!SendLong(opponent_id, x, y, Hit))) return false; // pfld опонента
			}
			Cli[idx].gm->SwapTurn();												// смена хода
			if (!SendShort(opponent_id, s_YourTurn)) return false;					// ход опонента
			if (!SendShort(idx, s_YouWait))	 return false;					// клиент промазал и ждет
		}

		// конец игры. закрываем игру. насчитываем очки
		if (turn_res == WinGameShot) {
			if (!EndGame(idx)) return false;
			break;
		}
		return true;
	}

	// чат так и не хватило времени (в основном оформить) доделать. Обработчик пакета и в с++ и 
	// в пайтоне есть. Так что этот пакет никем не отсылается и не принимается до следующей версии.
	case p_ChatMessage: { int msg_size; // так и не успел доделать общий чат. хотя тут немного осталось
		if (!REC(idx, recv(Cli[idx].con, (char*)&msg_size, sizeof(int), NULL))) return false;
		std::cout << "P#" << packettype << ":" << msg_size << " bytes\n";
		char* msg = new char[msg_size + 1]; msg[msg_size] = '\0';
		if (!REC(idx, recv(Cli[idx].con, msg, msg_size, NULL))) { delete[] msg; return false; }
		std::cout << "chat> " << Cli[idx].login << "> " << msg << "\n";
		delete[] msg;
		break;
		// отсылка сообщения всем законнекченым клиентам
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

	if (Cli[idx].in_game) { // отдает победу опоненту в случае выхода противника
		int opponent_id = ((Cli[idx].gm->gm_users[0] == idx) ? Cli[idx].gm->gm_users[1] : Cli[idx].gm->gm_users[0]);
		if (opponent_id != ERRor) { EndGame(opponent_id, 2); }
	}
	Cli[idx].idx = EMPTY;	   // освобождает слот в списке соединений=>разрывает while, close thread&exit
}

void GameServer() { // Раз в 10 секунд сообщает о статусе сервера: сколько игр идет и 
// сколько человек на сервере. При необходимости можно нацеплять сюда кучу полезного 
// функционала. Главное, проработан механизм, как это правильно делать.
	int max_games = max_players_on_server / 2;
	int gm_cnt = 0, plr_cnt = 0;
	using namespace std::chrono_literals;
	std::cout << "Game serve thread activated for " << max_games << " game slots.\n";
	while (true) {
		gm_cnt = 0; plr_cnt = 0;
		std::this_thread::sleep_for(10000ms);
		for (int i = 0; i < max_games; i++) {
			if (Gm->status != EMPTY) { gm_cnt++; }
		}
		for (int i = 0; i < max_players_on_server; i++) {
			if (Cli[i].idx != EMPTY) { plr_cnt++; }
		}		std::cout << "Connected: "<< plr_cnt << ", Active games: " << gm_cnt << "\n";
	}
}

int main(int argc, char* argv[]) {
	srand(time(NULL));
	if (!JsonRead(users)) { std::cout << "Ошибка. База данных повреждена. Сервер остановлен."; return 1; }

	WSAData wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData); // WORD DLLVersion = MAKEWORD(2, 1);
	if (res != NO_ERROR) {							// Ошибка запуска WSA
		printf("WSAStartup failed: %d\n", WSAGetLastError()); WSACleanup();	exit(1); }
	SOCKADDR_IN addr; 	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_family = AF_INET;

	SOCKET sListen = INVALID_SOCKET;
	sListen = socket(AF_INET, SOCK_STREAM, NULL);  // IPPROTO_TCP
	if (sListen == INVALID_SOCKET) { printf("Socket creation error: %u\n", WSAGetLastError()); WSACleanup(); exit(1); }

	res = bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	if (res == SOCKET_ERROR) { printf("Bind error %d\n", WSAGetLastError()); closesocket(sListen); WSACleanup(); exit(1); }

	std::cout << "Started Server@" << SERVER_IP << ":" << SERVER_PORT << "\n";
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) { printf("Listen fail error: %d\n", WSAGetLastError()); exit(1); }

	SOCKET In_Connection = INVALID_SOCKET;
	for (int i = 0; i < 100; i++) { Cli[i] = plr(INVALID_SOCKET); Cli[i].idx = ERRor; }

	HANDLE MainGameProcess = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)GameServer, (LPVOID)(NULL), NULL, NULL);
	int idx_new;
	while (true) {
		In_Connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr); // принимаем новое входящее соединение в новый сокет

		idx_new = FindFreeIndex();
		if ((In_Connection == INVALID_SOCKET) or (idx_new == -1)) { // ошибка соединения сокета или нет свободного слота для нового сокета
			printf("Accept connection failed. Error: %d\n", WSAGetLastError());
			closesocket(In_Connection);	continue;}					// закрываем сокет и продолжаем слушать входящие

		char* ip = inet_ntoa(addr.sin_addr);						// определяем IP адрес Клиента на всякий случай.
		printf("Accepted Connection for #%d, from : %s\n", idx_new, ip);

		Cli[idx_new].CleanClient();
		Cli[idx_new].con = In_Connection; Cli[idx_new].idx = idx_new; // создаем Class нового игрока

		// создаем новый поток для обслуживания нового Клиента. В поток передаем только индекс слота Клиента
		// на всякий случай сохраняем Tread id
		HANDLE cc = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(idx_new), NULL, NULL);
	}
	return 0;
}  //system("pause");


// список ошибок winsock:
   //send only				// 10013 WSAEACCES		// 10055 WSAENOBUFS		// 10065 WSAEHOSTUNREACH
// recv&send  WSAGetLastError()
// 10004 WSAEINTR		// 10014 WSAEFAULT		// 10022 WSAEINVAL		// 10035 WSAEWOULDBLOCK	
// 10036 WSAEINPROGRESS	// 10038 WSAENOTSOCK	// 10040 WSAEMSGSIZE	// 10045 WSAEOPNOTSUPP
// 10050 WSAENETDOWN	// 10052 WSAENETRESET	// 10053 WSAECONNABORTED// 10054 WSAECONNRESET
// 10057 WSAENOTCONN	// 10058 WSAESHUTDOWN	// 10060 WSAETIMEDOUT	// 10093 WSANOTINITIALISED		

// использовалось на стадии тестирования
//std::ostream& operator<< (std::ostream& out, ships_left _ships) {
//	out << "ships: ";
//	for (int i = 0; i < 5; i++) { out << _ships.ships[i] << " "; } out << "\n";
//	return out;
//}
//std::ostream& operator<< (std::ostream& out, ship& _ship) {
//	out << "Ship type: " << _ship._type << " [" << _ship._x << "," << _ship._y << "] " << _ship.len << ", idx:" << _ship.idx << ", cells>> ";
//	for (int i = 0; i < _ship.len; i++) { std::cout << *_ship.cells[i] << " "; }	std::cout << std::endl;	return out;
//}
//std::ostream& operator<< (std::ostream& out, cell& _cell) { out << ((_cell.grnd) ? "=" : "~") << _cell.type; return out; }


//template<typename T>
//std::string toString(const T& t) {
//	std::ostringstream oss;
//	oss << t;
//	return oss.str();
//}

//template<typename T>
//T fromString(const std::string& s) {
//	std::istringstream stream(s);
//	T t;
//	stream >> t;
//	return t;
//}

//time_t curr_time;
//curr_time = time(NULL);
//char* tm = ctime(&curr_time);
//std::string ss = std::string(tm);
//std::cout << "Today is : " << ss;
//std::cout << toString(curr_time);
