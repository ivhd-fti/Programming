#import requests  #from flask import Flask, request
#import json #import datetime #import time
import tkinter as tk
from tkinter import *
import threading
import socket
from time import sleep
import enum

import imageio
from PIL import Image, ImageTk

#import random as rnd
#import os

from pygame import mixer
from winsound import *

S={
"connected":    False,
"logged_in":    False,
"ready":        False,
"in_game":      False,
"game_do_trun": False,
"playerlist":   [],
0:[[[0,0] for x in range(10)] for y in range(10)],
1:[[[0,0] for x in range(10)] for y in range(10)], # "pfld": [],         #[[], [] .. ]    "ofld"
"fieldobjlinks":[0,0],
"ok":           False,
"fail":         "---",
"anim_do":      True,
"fld_amounts":  [ [-1,2,2,0,0], [-1,4,3,2,1]],
"video_run":    True,
"stop_threads": False,
"current_screen": 0,
"screens":      [],         # 0-start screen, 1-constructon, 1-game, 2-win\lose\stats
"fieldsize":    10,
"ground_index": 80,
"res":          [0,0,-1],
"game_end_result": "loss",
"s_WaitingOpponentJoin":False,
}

#anim_do=True
#video_run=True
#stop_threads = False

class Packet(enum.IntEnum):     # пакеты от Клиента к Серверу  =enum.auto()
    p_Ok = 1               #1+  T4- Успешная операция
    p_Fail = 2             #2+ - Операция не удалась
    p_Yes = 3              #3
    p_No = 4               #4
    p_UnReady = 5		   #5+  T4- Триггер: готов\не готов играть
    cp_Ping_request = 6    #+-6 - пинг запрос
    cp_Ping_reply = 7      #7  T4- пинг ответ
    p_Surrender = 8        #8  T4- закончить игру и сдаться (выводит в общий канал)
    p_Logoff = 9		   #+9  Т4- покинуть сервер -> отрабатывает P_Surrender, если во время партии. del token
    p_DelAcc = 10		   #+10 T4- БЕЗ ПОДТВЕРЖДЕНИЯ!! удалить игровой account
    p_Login =11		       #+11 T4-T4-login-T4-password  коннект к серверу, используя login:password						  new token
    p_AlreadyConnected = 12#+12 can't connect. disconnect first and login again
    p_ConnectOk = 13       #+13
    p_Connect_Fail =14     #+14
    p_Reg = 15             #+15 первичная регистрация на сервере.								  new token
    p_TurnRes = 16         #16+ - Выстрел игроком - результаты хода
    p_GetServerData = 17   #17+ plr: запрос сколько игроков и их имена
    p_ChatMessage = 18     #18 9-tmp посылка сообщения в чат (общий канал или сопернику)
    s_ObjChange = 19       #19 какие-то изменения какого-то объекта
    s_SendServerData = 20  #20+ srv: ответ сколько игроков и их имена

    s_ThisUserLoggedIn=21  #+21 такой пользователь уже законнекчен
    s_AccCreated=22        #+22 Создан новый экаунт
    s_AccInUse = 23		   #+23 экаунт занят
    s_YouAreNotConnected=24#+24 попытка отконнектиться, когда не залогинен
    s_AccountWasDeleted=25 #+25 Экаунт удален БЕЗ подтверждения
    s_AccountNotDeleted=26 #+26 Не могу удалить экаунт: логин или пароль не верные

    s_SendField=27		   #+27 Отсылка пустого поля игроку (поля для игрока)
    p_FieldRequest=28	   #+28 Отсылка сгенерированного пустого поля игроку
    s_LoginFirst=29		   #+29 Сначала залогиньтесь для этого действия
    p_SetFieldSize5 = 30   #+30 Размер поля 5x5
    p_SetFieldSize10 = 31  #+31 Размер поля 10x10
    s_FieldChangeRefuse = 32, #32 В данный момент изменения поля невожможны

    s_ShootFailed = 33,			# Выстрел не принят сервером: нет логина, или игры или ещё что-то
    #s_IncomingSoot = 34,		# N\A int4T- -входящий выстрел от сервера Miss = 0,	Wounded,	Killed , WrongShoot, WinGameShoot
    s_UnreadyFailInGame = 35,	# !!! REP: смена статуса невозможна : вы в игре> surrender
    s_UnreadyFailNoShips = 36,	#+ REP: смена на Ready невозможна: не все корабли расставлены
    s_YouAreNotInGame = 37,		#+ REP: вы не в игре: смена статуса
    s_YouAreInGame = 38,		#+ REP: Вы в игре	: смена статуса
    s_FailedToFindGame = 39,	#+ REP: если игра почему-то не найдена
    s_YourTurn = 40,			#+ COM: ваш ход, ходите
    s_YouWait = 41,				#+ COM: ожидайте своего хода
    s_YouWin = 42,				#+ COM: завершает игру. отсылается победителю
    s_YouLose = 43,				#+ COM: завершает игру. отсылается проигравшему

    s_SendCell_P = 52,			#+ COM: установить значение клетки у клиента на основном поле   x, y, val
    s_SendCell_O = 53,			#+ COM: установить значение клетки у клиента на запасном поле   x, y, val
    p_SendShip = 54,			#+ Игрок расставляет корабли, сервер проверяет
    s_CantPlaceShipHere = 55,	#+ Ошибка установки корабля
    s_SendShip = 56,			#+ Установить у Клиента корабль на поле клиента  x,y,type
    #p_DelShip = 57,				# удаление корабля с поля						  x,y,type
    #s_DelShip = 58,				#												  x,y,type
    #s_CantDelShip = 59,			# REP: ошибка удаления корабля
    p_SetGroundPercent = 60,	#+ pCOM: установить % суши на карте.   T4-val4
    s_SetGroundError = 61,		# !!!   REP: посылается, в случае, если IN-Game ИЛИ val<0 or val>100. Иначе в ответ посылается новое поле
    s_SendScore = 62,			#+ отсылает игроку его текущие достижения
    s_SurrenderFail = 63,		#+ REP: сдача не возможна. вы не в игре.
    s_SurrenderWin = 64,		#+REP: посылается игроку, одержавшему победу выходом опонента по surrender
    s_SendOponentField=65,      #+Присылается в момент старта игры: расположение суши противника
    s_WaitingOpponentJoin=66,   #+REP: ожидаем пока найдется противник
    s_RefreshField_P = 67,		#+ COM: обновление поля игрока
    s_RefreshField_O = 68,		#+ COM: обновление поля оппонента

class AT(enum.IntEnum): # ActionTypes
    prn = 1     # a_msg_print
    avc = 2     # a_var_change
    ale = 3     # a_load_and_eval_var_change
    run = 4     # run sub
    read3 = 5   # читаем 3 int и печатем
    read3i =6   # читаем 3 int и зансим в результат
    status = 7  # меняем текст статуса

def button_status(screen=1, btn_name="", state=True):
    if not btn_name or not 0<=screen<=2: return
    S["screens"][screen].buttons[btn_name]["state"] = "normal" if state else "disabled"

def change_status(before="", msg="", after="", clr="red"):
    S["screens"][1].change_status(before=before, msg=msg, after=after, clr=clr)

def screen_server(**kargs):
    global S, root, client

    if "new_screen" in kargs:
        print("screen=", kargs["new_screen"])
        S["screens"][ S["current_screen"] ].hide() # hide ald screen

        S["current_screen"] = kargs["new_screen"]

        if S["current_screen"]==1:
            S["fieldsize"]=    10
            S["ground_index"]= 80
            S["res"]=          [-1,0,-1]
            S["game_end_result"]= "---"
            S["ok"]=           False
            S["fail"]=         "---"
            S["ready"]=        False
            S["in_game"]=      False
            S["game_do_trun"]= False
            S["screens"][1].b8["state"] = "disabled"
            S["screens"][1].b8["text"] = "Готово"
            S["screens"][1].reset_self()
            S["screens"][1].getnewfield()

        elif S["current_screen"]==2:
            S["video_run"] = True
            #S["screens"][2].refresh()

        if S["current_screen"]==0:  S["video_run"] = True
        else:                       S["video_run"] = False
        S["screens"][ S["current_screen"] ].show()
    else:
        print("hello") #
        return

def field_build(**kargs):
    if kargs['field']==1:   S["screens"][1].ofld.build_self()
    else:                   S["screens"][1].pfld.build_self()

def cell_setter(**kargs):
    if kargs['field']==1:   S["screens"][1].ofld.set_cell()
    else:                   S["screens"][1].pfld.set_cell()

PacketActions={
Packet.s_SendScore:         [[AT.prn, "Результат игры:"], [AT.read3i] ],
Packet.s_YouWin:            [[AT.prn, "Поздравляем, Вы выиграли!"], [AT.avc, "in_game", False], [AT.avc, "game_end_result", "win"],
                            [AT.run, screen_server, {'new_screen':2}] ],
Packet.s_YouLose:           [[AT.prn, "Вы проиграли."], [AT.avc, "in_game", False], [AT.avc, "game_end_result", "loss"],
                            [AT.run, screen_server, {'new_screen':2}] ],
Packet.s_SurrenderWin:      [[AT.prn, "Поздравляем, Вы выиграли! Противник сдался."], [AT.avc, "in_game", False],
                            [AT.avc, "game_end_result", "win_sur"], [AT.run, screen_server, {'new_screen':2}] ],
Packet.s_SurrenderFail:     [[AT.prn, "Сдаться невозможно."]],
Packet.s_SendCell_O:        [[AT.read3i], [AT.run, cell_setter, {'field':1}] ], #[AT.prn, "Результат хода"],
Packet.s_SendCell_P:        [[AT.read3i], [AT.run, cell_setter, {'field':0}] ], #[AT.prn, "Выстрел по вам"],
Packet.s_RefreshField_P:    [[AT.read3i], [AT.run, field_build, {'field':0}]],
Packet.s_RefreshField_O:    [[AT.read3i], [AT.run, field_build, {'field':1}]],

Packet.p_Ok:                [[AT.prn, "Ok"], [AT.avc, "ok", "+++"]],
Packet.p_Fail:              [[AT.prn, "Fail"], [AT.avc, "fail", True]],
Packet.s_CantPlaceShipHere: [[AT.prn, "Невозможно установить корабль"], [AT.avc, "fail", "+++"]],
Packet.s_SendShip:          [[AT.read3], [AT.avc, "fail", "---"]], # [AT.prn, "Корабль прошел проверку"],
Packet.s_FailedToFindGame:  [[AT.prn, "Невозможно найти игру"], [AT.avc, "fail", "+++"], [AT.avc, "in_game", False] ],
Packet.s_YouAreInGame:      [[AT.prn, "Вы в игре"], [AT.avc, {"fail":"---", "in_game":True, "ready":True}],
                            [AT.run, change_status, {'before':"Вы в игре. ", "clr":"green"}] ],
Packet.s_WaitingOpponentJoin: [[AT.run, change_status, {'after':" Ищем Вам противника. Ожидайте. "}], [AT.avc, "s_WaitingOpponentJoin", True]],
Packet.s_YourTurn:          [[AT.prn, "Ваш ход"], [AT.avc, {"fail":"---", "in_game":True, "game_do_trun":True, "s_WaitingOpponentJoin":False}],
                            [AT.run, change_status, {'msg':"Ваш ход.", 'clr':'green'}] ],
Packet.s_YouWait:           [[AT.prn, "Ожидайте."], [AT.avc, {"fail":"---", "in_game":True, "game_do_trun":False, "s_WaitingOpponentJoin":False}],
                            [AT.run, change_status, {'msg':"Ожидайте ход противника."}]],
Packet.p_AlreadyConnected:  [[AT.prn, "Вы уже залогинены под другим экаунтом. Отконектитесь и попробуйте ещё раз."]],
Packet.p_ConnectOk:         [[AT.prn, "Поздравляем, Вы залогинились."], [AT.avc, "logged_in", True], [AT.run, screen_server, {'new_screen':1}] ],
Packet.p_Connect_Fail:      [[AT.prn, "Логин и пароль не верны."]],
Packet.p_Logoff:            [[AT.prn, "Вы отлогинились."], [AT.avc, "connected", False], [AT.avc, "logged_in", False], [AT.run, screen_server, {'new_screen':0}] ],
Packet.s_ThisUserLoggedIn:  [[AT.prn, "Такой пользователь уже законнекчен."]],
Packet.s_AccCreated:        [[AT.prn, "Новый экаунт создан. Залогиньтесь."]],
Packet.s_AccInUse:          [[AT.prn, "Этот логин уже занят."]],
Packet.s_YouAreNotConnected:[[AT.prn, "Вы не залогинены."]],
Packet.s_AccountWasDeleted: [[AT.prn, "Экаунт удален."]],
Packet.s_AccountNotDeleted: [[AT.prn, "Невозможно удалить экаунт."]],
Packet.s_SendServerData:    [[AT.prn, "Player List:"], [AT.ale, "playerlist"]],
Packet.s_SendField:         [[AT.ale, 0]], #[AT.prn, "New Field:"],
Packet.s_SendOponentField:  [[AT.ale, 1]], #[AT.prn, "New Oponent Field:"],
Packet.s_LoginFirst:        [[AT.prn, "Для этого необходимо залогиниться."]],
Packet.s_FieldChangeRefuse: [[AT.prn, "Изменения поля невозможны"], [AT.avc, "fail", True]],
Packet.s_UnreadyFailNoShips:[[AT.prn, "Не достаточно кораблей для игры"], [AT.avc, "fail", True]],
Packet.s_YouAreNotInGame:   [[AT.prn, "Вы отменили статус Готовности"], [AT.avc, {"fail":"---", "s_WaitingOpponentJoin":False, "in_game":False, "game_do_trun":False, "ready":False}],
                            [AT.run, button_status, {'screen':1, 'btn_name':'b8', 'state':True}], [AT.run, change_status, {'msg':"Вы вышли из игры. "}]],
Packet.s_UnreadyFailInGame: [[AT.prn, "Идёт игра. Войти в режим редактирования нельзя."]],
}
#button_status(screen=1, btn_name="", state=True)
#
#print(int(Packet.p_ChatMessage))
PORT = 3000; FORMAT = 'utf-8'; DISCONNECT_MESSAGE = "!DISCONNECT";
#SERVER = "192.168.0.107" #SERVER = "127.0.0.1"  #SERVER = socket.gethostbyname(socket.gethostname())
SERVER = "127.0.0.1"
ADDR = (SERVER, PORT); print('connecting...', ADDR)
trash=[]
img=[]

try:
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM); client.connect(ADDR)
    print('done.')
except:
    print('Connection failed. Server is probably down.')
    exit()
out_data = [4, 5, "login", 4 , "pass"] ; out_flag = False; in_data = ""  ; in_flag  = False

def prepare_snd_packet(data): # out_packet = [Packet.p_Login, 0, "кул", 0 , "да"]
    res = bytes()
    for d in range(len(data)):
        if isinstance(data[d], int):
            if (data[d]==0 and d+1<len(data) and isinstance(data[d+1], str)):
                data[d]=len(data[d+1].encode(FORMAT))
            res+=data[d].to_bytes(4, byteorder="little")
        elif isinstance(data[d], str):
            res+=data[d].encode(FORMAT)
    return res

def out_handle(n):
    print('out processor started')
    global S, out_data, out_flag, client
    while not S["stop_threads"]:
        if not out_flag:
            sleep(0.05);continue
        #print('     > snd start:', out_data)       ;print(prepare_snd_packet(out_data))
        try:
            client.send(prepare_snd_packet(out_data))
        except:
            print("Server is down (send). Quit.")
            client.close()
            stop_threads = True
            video_run = False
            #root.quit
            #root.destroy()
            #exit()
        out_flag = False

def in_handle(n):
    global S, in_data, in_flag, client, PacketActions
    while not S["stop_threads"]:
        if in_flag:
            sleep(0.05); continue # ждем пока освободится спул
        try:
            packet_type = int.from_bytes(client.recv(4), "little")
            in_flag = True
        except:
            print("Server is down (rec). Quit.")
            client.close()
            S["video_run"]      = False
            S["stop_threads"]   = True
            break

        print("IN packet#:", packet_type)

        if packet_type in PacketActions:
            #print("actions for packet:", PacketActions[packet_type])
            for act in PacketActions[packet_type]:
                if act[0]==AT.prn:
                    print(act[1])
                elif act[0]==AT.avc: # или: com, varname, value   или  com, {varname:val, ..., }
                    if isinstance(act[1], dict): S.update(act[1]) # обработка пакетной замены переменных
                    else: S[act[1]]=act[2]

                elif act[0]==AT.ale:
                    msg_length = int.from_bytes(client.recv(4), "little")
                    if msg_length:
                        in_data = client.recv(msg_length).decode(FORMAT) # чтение самих данных
                        S[act[1]]=eval(in_data)
                        #print("> msg_length=",msg_length, type(S[act[1]]))

                        #for i in S[act[1]]: # распечатка поля
                        #    print(i)

                        #S["screens"][1].build_self()

                elif act[0]==AT.run:
                    act[1](**act[2])

                elif act[0]==AT.read3:
                    in_data = client.recv(12).decode(FORMAT)
                    #print(in_data)

                elif act[0]==AT.read3i:
                    S["res"][0] = int.from_bytes(client.recv(4), "little")
                    S["res"][1] = int.from_bytes(client.recv(4), "little")
                    S["res"][2] = int.from_bytes(client.recv(4), "little")
                    print('S["res"]=', S["res"])

        in_flag = False
    #client.close()

def CleanTrash():
    global S, trash
    for id in trash:
        C.delete(id)
def anim(n):
    global S, trash, out_flag, out_data, in_flag, in_data
    if S["stop_threads"]:
        root.destroy() # #root.quit
        exit()

    if not anim_do: return

    if in_flag:
        print('in_data>', in_data)
        in_flag  = False

    root.after(500, anim, n)
def logoff(*arg, **karg):
    global out_data, out_flag
    while out_flag: sleep(0.05)
    out_data = [Packet.p_Logoff]
    out_flag = True
def getserverdata(*arg, **karg):
    global S, out_data, out_flag
    while out_flag: sleep(0.05)
    out_data = [Packet.p_GetServerData]
    out_flag = True

class cell_types(enum.IntEnum):
	Free = 0       # 0 не занятая клетка
	Ship = 1       # 1 неповрежденный корабль
	Broken_ship=2  # 2 поврежденный корабль
	Cross = 4      # 4 место куда не стреляли (Free), но нет смысла уже стрелять
	Hit = 8        # 8 Место куда стреляли но не попали
class Cell():
    def __init__(self, root, x, y, dat=[True, cell_types.Free], shipid=-1): #root-link to field # cell size = 38x38
        self.root           = root # link to field
        self.grnd           = dat[0]
        self.type           = dat[1]
        self.ship           = shipid  # принадлежность к конкретному кораблю (-1 нет связи)
        self.id_on_canvas   = -1      # id Объекта на canvas
        self.x              = x
        self.y              = y

    def clean(self, *arg, **karg):
        self.root.C.delete(self.id_on_canvas)                 # удаляем старую картинку

    def update(self, new_type=""):
        if not new_type: new_type = self.type
        if new_type not in images_P[self.grnd]: return False
        self.clean()
        self.img  = images_P[self.grnd][self.type][1]
        self.id_on_canvas = self.root.C.create_image(self.x, self.y, image = self.img, anchor = tk.NW, tag='cell')
        if not self.root.ofld: self.root.C.tag_bind(self.id_on_canvas, "<ButtonPress-3>",     self.root.setter)
        else:                  self.root.C.tag_bind(self.id_on_canvas, "<ButtonPress-1>",     self.root.shooter)
        self.root.C.tag_bind(self.id_on_canvas, "<ButtonPress-2>",     self.update)
        self.type = new_type

ship_list = [
	[1, False, [[0,0], [0,0], [0,0], [0,0]], [0,0]],       # 0 1-палубный вода
	[1, True,  [[0,0], [0,0], [0,0], [0,0]], [1,1]],       # 1 1-палубный суша
	[2, False, [[0,0], [1,0], [0,0], [0,0]], [3,3]],       # 2
	[2, False, [[0,0], [0,1], [0,0], [0,0]], [2,2]],       # 3
	[2, True,  [[0,0], [1,0], [0,0], [0,0]], [5,5]],       # 4
	[2, True,  [[0,0], [0,1], [0,0], [0,0]], [4,4]],       # 5

    [3, False, [[0,0], [ 0, 1], [ 0, 2], [ 0, 0]], [ 7, 7]],  # 6    7w- ###   8g-## 9g-##
	[3, False, [[0,0], [ 1, 0], [ 2, 0], [ 0, 0]], [ 6, 6]],  # 7    6w-#          #    #
	[3, True,  [[0,0], [-1, 0], [ 0, 1], [ 0, 0]], [11, 9]],  # 8       #
	[3, True,  [[0,0], [ 1, 0], [ 0, 1], [ 0, 0]], [ 8,10]],  # 9       #
	[3, True,  [[0,0], [ 0,-1], [ 1, 0], [ 0, 0]], [ 9, 9]],  #10            10g-#   11g- #
	[3, True,  [[0,0], [-1, 0], [ 0,-1], [ 0, 0]], [10, 8]],  #11                ##      ##
	[4, False, [[0,0], [ 0, 1], [ 0, 2], [ 0, 3]], [13,13]],  #12 13w- #      12w- ####
	[4, False, [[0,0], [ 1, 0], [ 2, 0], [ 3, 0]], [12,12]],  #13      #   14g- ##  16g- #  18g- ##   20g- #
    [4, True,  [[0,0], [ 1, 0], [ 0, 1], [ 0, 2]], [15,21]],  #14      #        #        #        #        #    [15,17]
	[4, True,  [[0,0], [ 0, 1], [-1, 0], [-2, 0]], [16,14]],  #15      #        #       ##        #        ##
	[4, True,  [[0,0], [-1, 0], [ 0,-1], [ 0,-2]], [17,15]],  #16
	[4, True,  [[0,0], [ 0,-1], [ 1, 0], [ 2, 0]], [18,16]],  #17 15g- ###   17g - #    19g-  #     21g - ###   [14,16]
    [4, True,  [[0,0], [-1, 0], [ 0, 1], [ 0, 2]], [19,17]],  #18        #         ###      ###           #     [19,21]
	[4, True,  [[0,0], [ 0,-1], [-1, 0], [-2, 0]], [20,18]],  #19
	[4, True,  [[0,0], [ 1, 0], [ 0,-1], [ 0,-2]], [21,19]],  #20
	[4, True,  [[0,0], [ 0, 1], [ 1, 0], [ 2, 0]], [14,20]]   #21  [18,20]
]

class Field():
    def __init__(self, root, size=1, ofld=False): # root->Tk topmost level
        global S
        self.root       = root # ссылка на screen (canvas на нем => self.root.C)
        self.size       = size #1=10 0=5  self.size*5+5
        self.ofld       = ofld # False-основное поле, True-поле опонента
        self.x_offset   = (20 + (200 if self.size==5 else 0) if not self.ofld else 425)
        self.C          = self.root.C
        self.ships      = [] #[id,x,y,len, set[shp_cells],set[busy]]
        self.cells      = []
        self.trash      = []
        self.previous   = (0,0)
        self.cur_ship   = [] # S["fld_amounts"][0-1] [-1,4,3,2,1]
        self.cur_size   = 4
        self.busy       = set( [(y,x) for y in range(-2, self.size*5+5+2) for x in range(-2, self.size*5+5+2)] ) - set( [(y,x) for y in range(0, self.size*5+5) for x in range(0, self.size*5+5)] )

        if not self.ofld:
            self.root.C.bind("<MouseWheel>",        self.mouseRotate)
            self.root.C.bind("<ButtonPress-1>",     self.selectB1) #move_start)
            self.root.C.bind("<ButtonRelease-1>",   self.dropB1)
            self.root.C.bind("<B1-Motion>",         self.dragB1) #move_move)
        #self.root.C.tag_bind('tmp_ship', '<Enter>', self.mouseEnter)

    def set_cell(self, *args):
        x, y, v = S["res"][0],S["res"][1],S["res"][2]
        if v==-1: return
        S[self.ofld][y][x][1] = v
        self.cells[y][x]=Cell(self, self.cells[y][x].x, self.cells[y][x].y, [self.cells[y][x].grnd, v], -1)
        self.cells[y][x].update()

        S["res"]=[0,0,-1] #dat=[True, cell_types.Free]
        #self.build_self()

    def reset_field(self):
        self.cur_ship   = []
        self.ships      = []
        self.cur_size   = 4
        self.size       = S["fieldsize"]
        self.build_self()

    def clean_field(self):
        global S
        for y in range(len(self.cells)):
            for x in range(len(self.cells[y])):
                self.cells[y][x].clean()
        self.cells      = []

        for i in self.trash: self.root.C.delete(i)
        self.trash=[]

    def build_self(self):
        global S
        #print('Field build self', self.ofld)
        self.clean_field()

        self.size = len(S[0])//5 -1
        for y in range(5+5*self.size):
            self.cells.append([])
            self.trash.append(self.C.create_text(self.ofld*424+ 13+(not self.size and not self.ofld)*200, y*40+20+50, text=str(y+1), fill="red", font="Times 15 italic bold"))
            for x in range(5+5*self.size):
                if y==0:
                    self.trash.append(self.C.create_text(self.ofld*425+x+45+x*39+(not self.size and not self.ofld)*200, 38, text="АБВГДЕЖЗИК"[x], fill="red", font="Times 15 italic bold"))

                if not isinstance(S[self.ofld][y][x], list):
                    S[self.ofld][y][x]=[S[self.ofld][y][x], cell_types.Free]
                self.cells[y].append(Cell(self, 26+x*40+self.ofld*425+(not self.size and not self.ofld)*200, 50+y*40, S[self.ofld][y][x], -1))
                self.cells[y][-1].update()
                #C.tag_bind(imgg, "<Button-1>", wanish) #move
    def shooter(self, ev):
        global S, out_data, out_flag
        if not S["game_do_trun"]:
            print('no')
            return # если не ваш ход или просто ожидание
        _x, _y = self.xy_calc(ev.x, ev.y, self.size*5+5, self.ofld, t=False) # !!!###

        while out_flag: sleep(0.05)
        out_data = [Packet.p_TurnRes, _x, _y]
        print('turn:',out_data)
        out_flag = True

    def setter(self, ev): # расставляет корабли
        global S
        if S["current_screen"]!=1 or S["ready"]: return
        if not self.cur_ship:
            _x, _y = self.xy_calc(ev.x, ev.y, self.size*5+5, self.ofld, t=False)

            if S[0][_y][_x][1]==1: # корабль
                idx = [i for i in range(len(self.ships)) if (_y,_x) in self.ships[i][4]][0]
                for i in self.ships[idx][4]:
                    S[0][i[0]][i[1]][1]=0
                self.build_self()

                id = self.ships[idx][0]
                del self.ships[idx]
                S["screens"][1].b8["state"]="disabled"

                self.cur_ship = self.draw_tmp_ship(id, _x, _y, self.size*5+5, S[0][_y][_x][0], self.root.C, ofld=self.ofld)
                #print("add", self.cur_ship,_x, _y, ev.x, ev.y)
                return

            ##[id,x,y,len, set[shp_cells],set[busy]]

            if not any( [self.check_amount(i) for i in range(1,5)]  ):

                print('Все корабли расставлены! Жмите "Готово"')
                self.root.change_status(msg='Все корабли расставлены! Жмите "Готово"')
                S["screens"][1].b8["state"]="normal"
                return # все корабли стоят!!!!!!!!!!!!!!!!!!!!!

            S["screens"][1].b8["state"] = "disabled"

            self.cur_size=4
            while not self.check_amount(self.cur_size): self.cur_size-=1 # нашли нехватающий размер

            id=0
            while not (ship_list[id][1]==S[0][_y][_x][0] and self.cur_size==ship_list[id][0]):
                id+=1
                #print("id:", id)

            self.cur_ship = self.draw_tmp_ship(id, _x, _y, self.size*5+5, S[0][_y][_x][0], self.root.C, ofld=self.ofld)
            #print("add", self.cur_ship,_x, _y, ev.x, ev.y)
        else:
            if not self.tmp_ship_chk(): return
            self.cur_ship=[]
            self.root.C.delete('tmp_ship')

    def add_tmp_ship_to_field(self, *args):
        #print('args:',args)
        if self.cur_ship[7]=="red": return
        busy=set()
        shp_cells=set()
        sh=ship_list[self.cur_ship[0]]

        x=self.cur_ship[1]; y=self.cur_ship[2]; id=self.cur_ship[0]
        for i in range(sh[0]):
            _x,_y = x+sh[2][i][0], y+sh[2][i][1]
            S[0][ _y ][ _x ][1]=1
            #busy.add(tuple([_y, _x])  )
            crosses=[(yy,xx) for yy in range(_y-1, _y+2) for xx in range(_x-1, _x+2) if (0<=xx<self.size*5+5) and (0<=yy<self.size*5+5) ]
            busy.update(crosses)
            shp_cells.add(tuple([_y, _x]) )
        self.ships.append([id,x,y,sh[0], shp_cells, busy]) #[id,x,y,len, set[shp_cells],set[busy]]
        self.cur_ship=[]
        self.root.C.delete('tmp_ship')
        self.build_self()
        #print("ships-busy:", len(self.ships), len(self.busy))
        if not any( [self.check_amount(i) for i in range(1,5)]  ):
            S["screens"][1].b8["state"]="normal"
        #        0  1 2   3    4     5      6     7
        #return [id,x,y,size,grnd, canvas, ofld, clr]
    def get_busy(self):
        busy = set( [(y,x) for y in range(-2, self.size*5+5+2) for x in range(-2, self.size*5+5+2)] ) - set( [(y,x) for y in range(0, self.size*5+5) for x in range(0, self.size*5+5)] )
        for c_sh in self.ships:
            busy|= c_sh[5] #[id,x,y,len, set[shp_cells],set[busy]]
        return busy

    def get_ship_cells(self, id, x, y):
        sh=ship_list[id]
        #print("==============>",sh)
        return set([(y+sh[2][i][1], x+sh[2][i][0]) for i in range(sh[0])])

    def tmp_ship_chk(self):
        cur = self.root.C.find_withtag('current')
        shp = self.root.C.find_withtag('tmp_ship')
        if (not self.cur_ship) or (not cur): return False
        if (cur[0] not in shp): return False
        return True

    def selectB1(self, event):
        if not self.tmp_ship_chk(): return
        self.previous = (event.x, event.y)

    def dragB1(self, event):
        if not self.tmp_ship_chk(): return
        xc = event.x; yc = event.y
        dx, dy = xc-self.previous[0], yc-self.previous[1]
        self.root.C.move('tmp_ship', dx, dy)
        self.cur_ship[1], self.cur_ship[2] = self.xy_calc(xc,yc,self.cur_ship[3], self.cur_ship[6], t=False)
        self.previous = (xc, yc)

    def dropB1(self, event):
        #[id,x,y,size,grnd, canvas, ofld, clr]
        if not self.tmp_ship_chk(): return
        id,x,y,size=self.cur_ship[0], self.cur_ship[1], self.cur_ship[2], self.cur_ship[3] #[id,x,y,size,grnd, canvas, ofld, clr]
        sh=ship_list[id]
        while any([ x+sh[2][i][0]<0     for i in range(sh[0]) ]): x+=1
        while any([ size<=x+sh[2][i][0] for i in range(sh[0]) ]): x-=1
        while any([ y+sh[2][i][1]<0     for i in range(sh[0]) ]): y+=1
        while any([ size<=y+sh[2][i][1] for i in range(sh[0]) ]): y-=1
        self.cur_ship[1]=x; self.cur_ship[2]=y
        self.root.C.delete('tmp_ship')
        self.cur_ship = self.draw_tmp_ship(*self.cur_ship)

    def mouseRotate(self, event):
        if not self.tmp_ship_chk(): return
        id,x,y,size=self.cur_ship[0:4]
        if (event.delta > 0):   id = ship_list[id][3][0]
        elif (event.delta < 0): id = ship_list[id][3][1]

        sh=ship_list[id]
        not_in_field = not all([(0<=x+sh[2][i][0]<size and 0<=y+sh[2][i][1]<size) for i in range(sh[0])])
        if not_in_field: return

        dat=[id]+self.cur_ship[1:]
        self.root.C.delete('tmp_ship')
        self.cur_ship = self.draw_tmp_ship(*dat)

    def mouseEnter(self, event):              # не подключено. возможно в будущем.
        if not self.tmp_ship_chk(): return
        print('->', self.root.C.find_withtag('current'), cur_ship )

    def xy_calc(self, x, y, size, ofld, t=True):
        if t: return [26+x*40+ofld*425+(not (size//5-1) and not ofld)*200, 50+y*40]

        _x = (x-26-ofld*425-(not (size//5-1) and not ofld)*200)/40; _y=(y-50)/40

        return [int(_x)+1 if (_x-int(x)>0.5) else int(_x), int(_y)+1 if (_y-int(y)>0.5) else int(_y)]

    def check_amount(self, ship_size):
        current_ship_amount = [-1,0,0,0,0]
        for c_sh in self.ships: #[id,x,y,len, set[shp_cells],set[busy]]
            current_ship_amount[c_sh[3]]+=1
        if current_ship_amount[ship_size]+1>S["fld_amounts"][self.size][ship_size]: return False # не ставим т.к. и так много уже
        return True

    def draw_tmp_ship(self, id, x, y, size, grnd, canvas, ofld=False, clr=''):
        sh=ship_list[id]
        if not self.check_amount(sh[0]): return []

        not_in_field = not all([(0<=x+sh[2][i][0]<size and 0<=y+sh[2][i][1]<size) for i in range(sh[0])])
        if not_in_field: return [] # за пределами поля

        clr = "green" #id, x, y
        surf_much = all([( S[0][y+sh[2][i][1]][x+sh[2][i][0]][0]==sh[1] ) for i in range(sh[0])])
        if (self.get_ship_cells(id, x, y) & self.get_busy()) or not surf_much: clr = "red" # есть хотя бы одно совпадение занятой и клетки корабля

        for i in range(sh[0]): # идем по клеткам корабля
            _x, _y = self.xy_calc(x+sh[2][i][0], y+sh[2][i][1], size, ofld)
            ship = self.root.C.create_rectangle(_x, _y, _x+39, _y+39, fill=clr, tag='tmp_ship')
            self.root.C.tag_bind(ship, "<ButtonPress-3>",     self.setter)
            self.root.C.tag_bind(ship, "<ButtonPress-2>",     self.add_tmp_ship_to_field)
        #       0  1 2   3    4     5      6     7
        return [id,x,y,size,grnd, canvas, ofld, clr]

class EndWindow(object):
    def __init__(self, root):
        global S
        self.root = root #
        self.C = Canvas(self.root, width = 854, height = 482); self.C.place(x=1000, y=0) #(x=-2, y=0)
        self.trash = []
        b2=Button(self.C, text="PlayAgain")
        b2.bind("<Button-1>", lambda event: screen_server(ev=event, new_screen=1) );  b2.place(x=650,y=450)
        b3=Button(self.C, text="Выйти");    b3.bind("<Button-1>", logoff); b3.place(x=800,y=450)
        l1=Label(self.C, text="Счет:");          l1.place(x=100, y=400)
        self.l1a=Label(self.C, text="50");      self.l1a.place(x=100, y=430)

        l2=Label(self.C, text="Игр пройдено:"); l2.place(x=200, y=400)
        self.l2a=Label(self.C, text="10"); self.l2a.place(x=200, y=430)

    def hide(self):
        S["video_run"] = False
        self.channel1.stop()
        self.channel2.stop()
        S["video_run"]=False
        self.C.place(x=1000, y=0)

    def show(self):
        self.C.place(x=-2, y=0)
        self.C.delete(*self.trash)
        self.l1a["text"]=str(S["res"][0])
        self.l2a["text"]=str(S["res"][1])

        self.channel1 = mixer.Channel(0); self.channel2 = mixer.Channel(1)

        dat = {'win':     ['win_mus.ogg', 'win_snd.ogg', "win_vid.mp4", 0.0],
               'loss':    ['loss_mus.ogg', 'loss_snd.ogg', "loss_vid.mp4", 2.3],
               'win_sur': ['win_mus.ogg', 'win_snd.ogg', "win_vid.mp4", 0.0] }

        self.end_ch1  = mixer.Sound(dat[S["game_end_result"]][0])
        self.end_ch2 = mixer.Sound(dat[S["game_end_result"]][1])
        self.video_name = dat[S["game_end_result"]][2]

        self.video = imageio.get_reader(self.video_name)
        S["video_run"] = True
        self.thread = threading.Thread(target=self.stream, args=(self.C, dat[S["game_end_result"]][3])); self.thread.daemon = 1
        self.thread.start()

    def stream(self, cnv, delay=0.0):
        global S, img
        print(">>>", S["video_run"], S["stop_threads"])
        img=[]
        cnt=0
        c=-1; f= True  #self.players_on_server.insert(0, "")
        #title = Image.open("title.png").convert('RGBA')
        self.channel1.play( self.end_ch1, -1)
        while S["video_run"] and (not S["stop_threads"]): #
            #mixer.music.play(loops=-1)
            self.channel2.play( self.end_ch2, -1 )

            sleep(delay)
            self.video.set_image_index(0)
            for i in range(1, 600):
                image=self.video.get_next_data()
                if (not S["video_run"]) or S["stop_threads"]: break
                #if cnt==600: break
                c+=1
                if f:
                    img.append(ImageTk.PhotoImage(Image.fromarray(image)))
                    if c==9: f=False
                    continue
                else:
                    if c==10: c=0
                    pic = Image.fromarray(image).convert('RGBA')
                    #pic.paste(images_raw['header'],            (230, 20), images_raw['header'])

                    #pic.paste(images_raw['players_on_server'], (230,250), images_raw['players_on_server'])

                    img[c] = ImageTk.PhotoImage(pic)
                    k=c+1
                    if k>=10:   k=0
                    cnv.create_image(0, 0, image = img[c-1], anchor = tk.NW)
                #sleep(0.005)
        mixer.music.stop()

class PlayWindow(object):
    def __init__(self, root):
        global S
        self.root = root #
        self.C = Canvas(self.root, width = 854, height = 482); self.C.place(x=1000, y=0) #(x=-2, y=0)
        self.C.delete('all')
        b3=Button(self.C, text="Выйти");          b3.bind("<Button-1>", logoff);            b3.place(x=800,y=450)
        b7=Button(self.C, text="Новое поле");       b7.bind("<Button-1>", self.getnewfield);  b7.place(x=720,y=450)
        self.b8=Button(self.C, text="Готово"); self.b8.bind("<Button-1>", self.UnReady); self.b8.place(x=650,y=450)
        self.b8["state"] = "disabled"

        self.buttons={"b8":self.b8}
        b9=Button(self.C, text="Сдаться");   b9.bind("<Button-1>", self.Surrender);     b9.place(x=20,y=450)

        self.C.create_text(515, 462, text="% суши:", fill="red", font="Times 12 italic bold")
        self.sbv = StringVar(root); self.sbv.set(S["ground_index"])
        self.sb = Spinbox(self.C, from_= 0, to = 100, textvariable=self.sbv, increment = 1, repeatdelay = 200, width=4)
        self.sb.place(x=550,y=455) #self.sbv.trace('w', lambda x,y,z: self.check_pat (x,y,z,widget=self.sb )) # отслеживаем изменение переменной в динамике

        self.C.create_text(360, 462, text="Тип поля:", fill="red", font="Times 12 italic bold")
        self.op_var = StringVar(root);  self.op_var.set("10x10")
        field_size_menu = OptionMenu(self.C, self.op_var, *["5x5", "10x10"]);
        field_size_menu.place(x=397, y=448)  #op_var.trace('w', lambda x,y,z: self.check_pat (x,y,z,widget=self.pw ))
        self.op_var.trace('w', self.changefieldsize)

        self.status_msg  = ""
        self.status_msgC= self.C.create_text(200, 10, text=self.status_msg, fill="red", font="Times 12 italic bold")
        self.sbv_ald = 80

        self.build_self()

        PacketActions[Packet.s_SendField].append([AT.run, self.pfld.reset_field, {}])
        PacketActions[Packet.s_SendField].append([AT.run, self.ofld.reset_field, {}])
        PacketActions[Packet.s_SendOponentField].append([AT.run, self.ofld.reset_field, {}])

    def change_status(self, msg="", before="", after="", clr="red"): #before="", msg="", after=""
        self.status_msg = (before + self.status_msg + after) if (before!="" or after!="") else msg # значение msg игнорируется уди есть befor или after
        self.C.delete(self.status_msgC)
        self.status_msgC= self.C.create_text(450, 10, text=self.status_msg, fill=clr, font="Times 12 italic bold")

    def reset_self(self):
        S[0]=[[[0,0] for x in range(S["fieldsize"])] for y in range(S["fieldsize"])]
        S[1]=[[[0,0] for x in range(S["fieldsize"])] for y in range(S["fieldsize"])]
        self.pfld.reset_field()
        self.ofld.reset_field()
        self.change_status("")

    def getnewfield(self, *arg, **karg):
        global S, out_data, out_flag
        if S["in_game"] or S["ready"]: return
        while out_flag: sleep(0.05) #
        try:
            self.sbv_ald=int(self.sbv.get())
        except: pass
        if self.sbv_ald> 100: self.sbv_ald=100
        if self.sbv_ald< 0: self.sbv_ald=0

        self.sbv.set(self.sbv_ald)
        out_data = [Packet.p_SetGroundPercent, self.sbv_ald]
        out_flag = True

    def changefieldsize(self, *args, **kargs): #S["screens"][1]
        global S, out_data, out_flag
        print("ChSize")
        while out_flag: sleep(0.05)
        if self.op_var.get()=="10x10":
            out_pack = Packet.p_SetFieldSize10
            scr_size = 10
        else:
            out_pack = Packet.p_SetFieldSize5
            scr_size = 5

        self.pfld.clean_field(); self.ofld.clean_field();
        self.C.delete('all')
        self.C.create_text(515, 462, text="% суши:", fill="red", font="Times 12 italic bold")
        self.C.create_text(360, 462, text="Тип поля:", fill="red", font="Times 12 italic bold")

        self.pfld.size = scr_size; self.ofld.size = scr_size
        #sleep(0.5)
        #self.C.delete('cells')
        #self.C.delete('all')
        out_data = [out_pack]; out_flag = True

    def Surrender(self, *arg, **karg):
        global out_data, out_flag
        while out_flag: sleep(0.05)
        out_data = [Packet.p_Surrender]
        out_flag = True

    def UnReady(self, *arg, **karg): # для Ready\Unready p_UnReady
        global S, out_data, out_flag #"ready"] "in_game"
        print('S["in_game"]=', S["in_game"], len(S["screens"][1].pfld.ships))
        if S["in_game"]: #Идет игра. Uready невозможно
            if not S["s_WaitingOpponentJoin"]:
                print('Logoff or surrender to uready now.')
                self.change_status(msg="Вы в игре. Для выхода сдайтесь.")
                return
            while out_flag: sleep(0.05)
            out_data = [Packet.p_UnReady]; out_flag = True # отсылаем отмену готовности
            return

        if S["screens"][1].b8["state"]=="disabled": return # кнопка не активна

        #if S["ready"]:
        #    S["screens"][1].b8["text"] = "Не готов"
        #    S["ready"] = False
        #else:
        #    S["screens"][1].b8["text"] = "Готов"
        #S["ready"]=True

        for sh in S["screens"][1].pfld.ships: #[id,x,y,len, set[shp_cells],set[busy]]
            while out_flag: sleep(0.05)
            S["fail"] = "???"
            out_data = [Packet.p_SendShip, sh[1], sh[2], sh[0] ]; out_flag = True

            while S["fail"] == "???": sleep(0.05)
            if S["fail"]=="+++":
                print(">> ERR:", sh)
                break

        while out_flag: sleep(0.05)
        out_data = [Packet.p_UnReady]; out_flag = True

    def hide(self):
        self.C.place(x=1000, y=0)

    def show(self):
        self.C.place(x=-2, y=0) #(x=-2, y=0)
        #self.op_var.set("10x10")

    def build_self(self):
        print("Game rebuild")
        self.pfld = Field(self, size=S["fieldsize"]//5-1, ofld=False)
        #self.pfld.build_self()
        self.ofld = Field(self, size=S["fieldsize"]//5-1, ofld=True)
        #self.ofld.build_self()
        self.op_var.set("10x10")

class StartWindow(object):
    def __del__(self):
        global S
        print("Del. Закрыт основной экран", self)
        S["video_run"]=False
        #self.hide()

    def __init__(self, root): # root->Tk topmost level
        global S
        S["video_run"] = True
        self.root = root
        self.build_self()
        self.show()

    def build_self(self):
        self.C = Canvas(self.root, width = 854, height = 482); self.C.place(x=-2, y=0)

        self.vL = StringVar(self.root);
        self.e_L = Entry(self.C, textvariable=self.vL, width=13); self.e_L.insert(0, ""); self.e_L.bind('<Return>', self.login);
        self.e_L.place(x=10+20, y=200+100)

        self.vP = StringVar(self.root);
        self.e_P = Entry(self.C, textvariable=self.vP, width=13);self.e_P.insert(0, ""); self.e_P.bind('<Return>', self.login);
        self.e_P.place(x=10+20, y=225+100)

        self.b2=Button(self.C, text="Login", width = 12); self.b2.bind("<Button-1>", self.login);
        self.b2.place(x=120, y=320)

        self.b4=Button(self.C, text="Регистрация");         self.b4.bind("<Button-1>", self.regnew);
        self.b4.place(x=30,y=350)
        self.b5=Button(self.C, text="Удалить запись");         self.b5.bind("<Button-1>", self.delacc);
        self.b5.place(x=120,y=350)

        lbl = Label(self.C, text = "Игроков на сервере:", bd=0)
        lbl.place(x=30, y=390)
        self.players_on_server = Entry(self.C, width=5);
        self.players_on_server.insert(0, "1000")
        self.players_on_server.place(x=180, y=390)
        self.players_on_server.config(state=DISABLED)

    def show(self):
        self.C.place(x=-2, y=0)
        S["video_run"]=True
        mixer.music.load("src1.mp3") ; mixer.music.play(loops=-1) #loops=0
        mixer.music.set_volume(0.2)
        self.video_name = "MainScreen.avi"#  "perl_3.mp4"
        self.video = imageio.get_reader(self.video_name)
        thread = threading.Thread(target=self.stream, args=(self.C,));  thread.daemon = 1
        thread.start()

    def hide(self):
        global S
        #print("gonna shut down all")
        S["video_run"]=False
        sleep(0.5)
        self.C.place(x=2000, y=0)

    def stream(self, cnv):
        global S, img
        img=[]
        cnt=0
        c=-1; f= True  #self.players_on_server.insert(0, "")
        #title = Image.open("title.png").convert('RGBA')
        while S["video_run"] and (not S["stop_threads"]): #
            getserverdata()
            self.players_on_server.config(state=NORMAL)
            self.players_on_server.delete(0, 'end')
            self.players_on_server.insert(0, str(len(S["playerlist"])))
            self.players_on_server.config(state=DISABLED)
            self.video.set_image_index(20)
            #self.video.set_image_index(0)
            for i in range(200, 1380):
            #for i in range(0, 1380):
                #for image in self.video.iter_data():
                image=self.video.get_next_data()
                cnt+=1
                #if cnt<180: continue
                #print(cnt)
                if (not S["video_run"]) or S["stop_threads"]: break
                if cnt==1350: break
                c+=1
                if f:
                    img.append(ImageTk.PhotoImage(Image.fromarray(image)))
                    if c==9: f=False
                    continue
                else:
                    if c==10: c=0
                    pic = Image.fromarray(image).convert('RGBA')
                    pic.paste(images_raw['header'],            (230, 20), images_raw['header'])
                    #pic.paste(images_raw['players_on_server'], (230,250), images_raw['players_on_server'])

                    img[c] = ImageTk.PhotoImage(pic)
                    k=c+1
                    if k>=10:   k=0
                    cnv.create_image(0, 0, image = img[c-1], anchor = tk.NW)
                sleep(0.03)
        mixer.music.stop()
        #print("stream quit")
    def login(self, *arg, **karg):
        global S, out_data, out_flag
        if not str(self.vL.get()) or not str(self.vP.get()): return
        while out_flag: sleep(0.05)
        out_data = [Packet.p_Login, 0, str(self.vL.get()), 0, str(self.vP.get())]
        out_flag = True
    def regnew(self, *arg, **karg):
        global S, out_data, out_flag
        if not str(self.vL.get()) or not str(self.vP.get()): return
        while out_flag: sleep(0.05)
        out_data = [Packet.p_Reg, 0, str(self.vL.get()), 0, str(self.vP.get())]
        out_flag = True
    def delacc(self, *arg, **karg):
        global S, out_data, out_flag
        if not str(self.vL.get()) or not str(self.vP.get()): return
        while out_flag: sleep(0.05)
        out_data = [Packet.p_DelAcc, 0, str(self.vL.get()), 0, str(self.vP.get())]
        out_flag = True

def on_closing():
    global S, root, client
    client.close()
    S["video_run"]      = False
    S["stop_threads"]   = True
    S["recive_thread"].join()
    S["send_thread"].join()
    root.destroy()

def main():
    global S, root, images_raw, image_surf_raw, images_P

    mixer.init()
    root = Tk(); root.geometry('%dx%d+%d+%d' % (852,480,200,80));  root.resizable(0, 0) # #root.overrideredirect(1) # ; root.iconbitmap("icon.ico")

    images_cord={'header':(114,0,542,76), 'ship':(0,0,37,37), 'broken_ship':(38,0,75,37),
                 'cross':(76,0,113,37), 'hit':(0,38,37,75), 'players_on_server':(0,76,127,92)} #, 'ground':(38,38,75,75), 'water':(76,38,113,75)
    main_image=Image.open("images.png").convert('RGBA')

    images_raw  = {i:main_image.crop((images_cord[i])).convert('RGBA') for i in images_cord}
    image_surf_raw =[main_image.crop((i)).convert('RGBA') for i in [(76,38,113,75), (38,38,75,75)]] #[water, gnd]

    images_P = [{0:['free'], 1:['ship'], 2:['broken_ship'], 4:['cross'], 8:['hit']},
                {0:['free'], 1:['ship'], 2:['broken_ship'], 4:['cross'], 8:['hit']} ]

    for i in [0,1]:
        for p in images_P[i]:
            pic = Image.new("RGBA", (38,38))
            pic.paste(image_surf_raw[i], (0,0), image_surf_raw[i])
            if p!=0: # не free
                pic.paste(images_raw[images_P[i][p][0]], (0,0), images_raw[images_P[i][p][0]])
            images_P[i][p].append(ImageTk.PhotoImage(pic))
    #root.after(500, anim, 0)

    S["recive_thread"] = threading.Thread(target=in_handle, args=(1,));  S["recive_thread"].start()
    S["send_thread"]   = threading.Thread(target=out_handle, args=(1,)); S["send_thread"].start() # x.join()

    root.protocol("WM_DELETE_WINDOW", on_closing)
    S["screens"]=[]
    S["screens"].append(StartWindow(root))
    S["screens"].append(PlayWindow(root))
    S["screens"].append(EndWindow(root))
    #S["screens"][-1].show()

    root.mainloop()

if __name__ == "__main__":
    main()


# from tkinter import messagebox
#if messagebox.askokcancel("Quit", "Do you want to quit?"):
#  root.destroy()
# root.protocol("WM_DELETE_WINDOW", on_closing)
