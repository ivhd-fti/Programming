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
#from pygame import mixer
#from winsound import *

S={
"connected":    False,
"logged_in":    False,
"playerlist":   [],
0:[ [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
    [[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
],
1:[
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [1,0], [1,0], [1,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0], [0,0] ],
[[0,0], [0,0], [0,0], [0,0], [0,0], [1,2], [0,8], [0,4], [0,1], [0,0] ],
], # "pfld": [],         #[[], [] .. ]    "ofld"
"fieldobjlinks":[0,0],
"ok":           False,
"fail":         False,
"anim_do":      True,
"video_run":    True,
"stop_threads": False,
"current_screen": 0,
"screens":      [],         # 0-start screen, 1-constructon, 2-game, 3-win\lose\stats
"fieldsize":    10,
"ground_index": 30
}

#anim_do=True
#video_run=True
#stop_threads = False

class Packet(enum.IntEnum):     # пакеты от Клиента к Серверу  =enum.auto()
    p_Ok = 1               #1  T4- Успешная операция
    p_Fail = 2             #2 - Операция не удалась
    p_Yes = 3              #3
    p_No = 4               #4
    p_UnReady = 5		   #5  T4- Триггер: готов\не готов играть
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
    p_TurnRes = 16         #16 - Выстрел игроком - результаты хода
    p_GetServerData = 17   #17 plr: запрос сколько игроков и их имена
    p_ChatMessage = 18     #18 9-tmp посылка сообщения в чат (общий канал или сопернику)
    s_ObjChange = 19       #19 какие-то изменения какого-то объекта
    s_SendServerData = 20  #20 srv: ответ сколько игроков и их имена
    s_ThisUserLoggedIn=21  #+21 такой пользователь уже законнекчен
    s_AccCreated=22        #+22 Создан новый экаунт
    s_AccInUse = 23		   #+23 экаунт занят
    s_YouAreNotConnected=24#+24 попытка отконнектиться, когда не залогинен
    s_AccountWasDeleted=25 #+25 Экаунт удален БЕЗ подтверждения
    s_AccountNotDeleted=26 #+26 Не могу удалить экаунт: логин или пароль не верные
    s_SendField=27		   #+27 Отсылка пустого поля игроку
    p_FieldRequest=28	   #+28 Отсылка сгенерированного пустого поля игроку
    s_LoginFirst=29		   #+29 Сначала залогиньтесь для этого действия
    p_SetFieldSize5 = 30   #+30 Размер поля 5x5
    p_SetFieldSize10 = 31  #+31 Размер поля 10x10
    s_FieldChangeRefuse = 32, #32 В данный момент изменения поля невожможны

    s_ShootFailed = 33,			# Выстрел не принят сервером: нет логина, или игры или ещё что-то
    #s_IncomingSoot = 34,		# N\A int4T- -входящий выстрел от сервера Miss = 0,	Wounded,	Killed , WrongShoot, WinGameShoot
    s_UnreadyFailInGame = 35,	# REP: смена статуса невозможна : вы в игре> surrender
    s_UnreadyFailNoShips = 36,	# REP: смена на Ready невозможна: не все корабли расставлены
    s_YouAreNotInGame = 37,		# REP: вы не в игре: смена статуса
    s_YouAreInGame = 38,		# REP: Вы в игре	: смена статуса
    s_FailedToFindGame = 39,	# REP: если игра почему-то не найдена
    s_YourTurn = 40,			# COM: ваш ход, ходите
    s_YouWait = 41,				# COM: ожидайте своего хода
    s_YouWin = 42,				# COM: завершает игру. отсылается победителю
    s_YouLose = 43,				# COM: завершает игру. отсылается проигравшему

    s_SendCell_P = 52,			# COM: установить значение клетки у клиента на основном поле   x, y, val
    s_SendCell_O = 53,			# COM: установить значение клетки у клиента на запасном поле   x, y, val
    p_SendShip = 54,			#+ Игрок расставляет корабли, сервер проверяет
    s_CantPlaceShipHere = 55,	#+ Ошибка установки корабля
    s_SendShip = 56,			#+ Установить у Клиента корабль на поле клиента  x,y,type
    p_DelShip = 57,				# удаление корабля с поля						  x,y,type
    s_DelShip = 58,				#												  x,y,type
    s_CantDelShip = 59,			# REP: ошибка удаления корабля
    p_SetGroundPercent = 60,	# pCOM: установить % суши на карте.   T4-val4
    s_SetGroundError = 61,		# REP: посылается, в случае, если IN-Game ИЛИ val<0 or val>100. Иначе в ответ посылается новое поле
    s_SendScore = 62,			# отсылает игроку его текущие достижения
    s_SurrenderFail = 63,		# REP: сдача не возможна. вы не в игре.
    s_SurrenderWin = 64,		# REP: посылается игроку, одержавшему победу выходом опонента по surrender
class AT(enum.IntEnum): # ActionTypes
    prn = 1     # a_msg_print
    avc = 2     # a_var_change
    ale = 3     # a_load_and_eval_var_change
    run = 4     # run sub
def screen_server(**kargs):
    global S, root, client
    if "new_screen" in kargs:
        print("new_screen", kargs["new_screen"])
        S["screens"][ S["current_screen"] ].hide()
        S["current_screen"] = kargs["new_screen"]
        S["screens"][ S["current_screen"] ].show()

        if S["current_screen"]==0:
            S["video_run"]      = True

        else:
            S["video_run"]      = False

        #S["screens"][0].hide()
        #S["screens"][1].show()
    else:
        print("hello") #
        return

PacketActions={
Packet.p_Ok:                [[AT.prn, "Ok"], [AT.avc, "ok", True]],
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
Packet.s_SendField:         [[AT.prn, "New Field:"],   [AT.ale, 0]],
Packet.s_LoginFirst:        [[AT.prn, "Для этого необходимо залогиниться."]],
Packet.s_FieldChangeRefuse: [[AT.prn, "Изменения поля невозможны"], [AT.avc, "fail", True]],
}

#print(int(Packet.p_ChatMessage))
PORT = 3000; FORMAT = 'utf-8'; DISCONNECT_MESSAGE = "!DISCONNECT"; SERVER = "127.0.0.1"  #SERVER = socket.gethostbyname(socket.gethostname())
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
    print('in processor started')
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

        print("packet_type:", packet_type)

        if packet_type in PacketActions:
            for act in PacketActions[packet_type]:
                if act[0]==AT.prn:
                    print(act[1])
                elif act[0]==AT.avc:
                    S[act[1]]=act[2]
                elif act[0]==AT.ale:
                    msg_length = int.from_bytes(client.recv(4), "little")
                    if msg_length:
                        in_data = client.recv(msg_length).decode(FORMAT) # чтение самих данных
                        S[act[1]]=eval(in_data)
                        print("> msg_length=",msg_length, type(S[act[1]]))
                        for i in S[act[1]]: # распечатка поля
                            print(i)
                        #S["screens"][1].build_self()

                elif act[0]==AT.run:
                    act[1](**act[2])

        in_flag = False
    #client.close()

def CleanTrash():
    global S, trash
    for id in trash:
        C.delete(id)
def anim(n):
    global S, trash, out_flag, out_data, in_flag, in_data
    if S["stop_threads"]:
        #root.quit
        root.destroy()
        exit()

    if not anim_do: return
    #n+=1
    #if not out_flag:
    #    out_data = [Packet.p_ChatMessage, 0, "#send from python#"+str(n)]
    #    out_flag = True

    if in_flag:
        print('in_data>', in_data)
        in_flag  = False

    #CleanTrash()
    #trash.append(C.create_text(100,100, text= n , fill="green",font="Times 12 italic bold"))
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
def getnewfield(*arg, **karg):
    global S, out_data, out_flag
    #print("NEW FIELD PROC!") #
    while out_flag: sleep(0.05)
    out_data = [Packet.p_SetGroundPercent, int(S["screens"][1].sbv.get()) ,Packet.p_FieldRequest]
    out_flag = True

def changefieldsize(*arg, **karg):
    global S, out_data, out_flag
    while out_flag: sleep(0.05)
    out_data = [Packet.p_SetFieldSize10 if S["screens"][1].op_var.get()=="10x10" else Packet.p_SetFieldSize5]
    out_flag = True
def UnReady(*arg, **karg): # для Ready\Unready p_UnReady
    global S
    if b8["state"] == "normal":
        b8["state"] = "disabled"
        b8["text"] = "enable"
    else:
        b8["state"] = "normal"
        b8["text"] = "disable"
def maketurn(*arg, **karg): #
    global S, out_data, out_flag
    while out_flag: sleep(0.05)
    out_data = [Packet.p_TurnRes]
    out_flag = True
def Surrender(*arg, **karg):
    pass

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
        #print(">>>>>>>>>>",dat)
        self.type           = dat[1]
        self.ship           = shipid  # принадлежность к конкретному кораблю (-1 нет связи)
        self.id_on_canvas   = -1      # id Объекта на canvas
        self.x              = x
        self.y              = y
        #print("cell",self.root.C)


    def clean(self):
        self.root.C.delete(self.id_on_canvas)                 # удаляем старую картинку

    def update(self, new_type=""):
        if not new_type: new_type = self.type
        if new_type not in images_P[self.grnd]: return False
        self.clean()
        self.img  = images_P[self.grnd][self.type][1]
        self.id_on_canvas = self.root.C.create_image(self.x, self.y, image = self.img, anchor = tk.NW)
        self.type = new_type

ship_list = [
	[1, False, [[0,0], [0,0], [0,0], [0,0]]], #0 1-палубный вода
	[1, True,  [[0,0], [0,0], [0,0], [0,0]]], #1 1-палубный суша
	[2, False, [[0,0], [1,0], [0,0], [0,0]]], #2
	[2, False, [[0,0], [0,1], [0,0], [0,0]]], #3
	[2, True,  [[0,0], [1,0], [0,0], [0,0]]], #4
	[2, True,  [[0,0], [0,1], [0,0], [0,0]]], #5

	[3, False, [[0,0], [0,1], [0,2], [0,0]]], #6    6w- ###   8g-## 9g-##
	[3, False, [[0,0], [1,0], [2,0], [0,0]]], #7    7w-#          #    #
	[3, True,  [[0,0], [1,0], [1,1], [0,0]]], #8       #
	[3, True,  [[0,0], [1,0], [0,1], [0,0]]], #9       #
	[3, True,  [[0,0], [0,1], [1,1], [0,0]]], #10            10g-#   11g- #
	[3, True,  [[1,0], [0,1], [1,1], [0,0]]], #11                ##      ##

	[4, False, [[0,0], [0,1], [0,2], [0,3]]], #12 13w- #      12w- ####
	[4, False, [[0,0], [1,0], [2,0], [3,0]]], #13      #   14g- ##  16g- #  18g- ##   20g- #
	[4, True,  [[0,0], [1,0], [0,1], [0,2]]], #14      #        #        #        #        #
	[4, True,  [[0,0], [1,0], [2,0], [2,1]]], #15      #        #       ##        #        ##
	[4, True,  [[0,2], [1,0], [1,1], [1,2]]], #16
	[4, True,  [[0,0], [0,1], [1,1], [2,1]]], #17 15g- ###   17g - #    19g-  #     21g - ###
	[4, True,  [[0,0], [1,0], [1,1], [1,2]]], #18        #         ###      ###           #
	[4, True,  [[0,1], [1,1], [2,1], [2,0]]], #19
	[4, True,  [[0,0], [0,1], [0,2], [1,2]]], #20
	[4, True,  [[0,0], [0,1], [1,0], [2,0]]], #21
]
class Ship():
    def __init__(self, root, x, y, _type, ofld=False): #
        pass
        #int _type = ERR; int _x; int _y; int len;  int idx; cell* cells[4];

class Field():
    def __init__(self, root, size=1, ofld=False): # root->Tk topmost level
        global S
        self.root       = root # ссылка на screen (canvas на нем => self.root.C)
        self.size       = size
        self.ofld       = ofld # False-основное поле, True-поле опонента
        self.x_offset   = (20 + (200 if self.size==5 else 0) if not self.ofld else 425)
        self.C          = self.root.C
        self.ships      = []
        self.cells      = []
        self.trash      = []

    def init_from_S(self): # при этом уже в S всё должно быть. проверки нет.
        global S
        if not ofld: self.Field=[[ Cell(self, grnd=S["pfld"][y][x], type=cell_types.Free, shipid=-1) for x in range(S["fieldsize"])] for y in range(S["fieldsize"])]
        else:        self.Field=[[ Cell(self, grnd=S["ofld"][y][x], type=cell_types.Free, shipid=-1) for x in range(S["fieldsize"])] for y in range(S["fieldsize"])]

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

class PlayWindow(object):
    def __init__(self, root):
        global S
        self.root = root #
        self.C = Canvas(self.root, width = 854, height = 482); self.C.place(x=1000, y=0) #(x=-2, y=0)
        b3=Button(self.C, text="Logoff");    b3.bind("<Button-1>", logoff);        b3.place(x=790,y=450)
        b7=Button(self.C, text="New field"); b7.bind("<Button-1>", getnewfield);   b7.place(x=720,y=450)
        b8=Button(self.C, text="Ready");     b8.bind("<Button-1>", UnReady);       b8.place(x=650,y=450)
        b9=Button(self.C, text="Сдаться");   b9.bind("<Button-1>", Surrender);     b9.place(x=20,y=450)

        self.C.create_text(515, 462, text="% суши:", fill="red", font="Times 12 italic bold")
        self.sbv = StringVar(root); self.sbv.set(S["ground_index"])
        self.sb = Spinbox(self.C, from_= 0, to = 100, textvariable=self.sbv, increment = 1, repeatdelay = 200, width=4)
        self.sb.place(x=550,y=455) #self.sbv.trace('w', lambda x,y,z: self.check_pat (x,y,z,widget=self.sb )) # отслеживаем изменение переменной в динамике

        self.C.create_text(360, 462, text="Тип поля:", fill="red", font="Times 12 italic bold")
        self.op_var = StringVar(root);  self.op_var.set("10x10")
        field_size_menu = OptionMenu(self.C, self.op_var, *["5x5", "10x10"]);
        field_size_menu.place(x=397, y=448)  #op_var.trace('w', lambda x,y,z: self.check_pat (x,y,z,widget=self.pw ))
        self.op_var.trace('w', changefieldsize)


        b7=Button(self.C, text="tst"); b7.bind("<Button-1>", self.tst);   b7.place(x=120,y=450)

        #changefieldsize()
        self.build_self()

        PacketActions[Packet.s_SendField].append([AT.run, self.pfld.build_self, {}])
        PacketActions[Packet.s_SendField].append([AT.run, self.ofld.build_self, {}])


    def tst(self, ev):
        self.pfld.clean_field()
        self.ofld.clean_field()

    def hide(self):
        self.C.place(x=1000, y=0)

    def show(self):
        self.C.place(x=-2, y=0) #(x=-2, y=0)

    def build_self(self):
        self.pfld = Field(self, size=S["fieldsize"]//5-1, ofld=False)
        self.pfld.build_self()
        self.ofld = Field(self, size=S["fieldsize"]//5-1, ofld=True)
        self.ofld.build_self()
        #852,480

        #b6=Button(root, text="PlayerList");     b6.bind("<Button-1>", getserverdata);   b6.place(x=10,y=5)
        #b9=Button(root, text="MakeTurn");       b9.bind("<Button-1>", maketurn);        b9.place(x=10,y=95)  # height=5, width=7

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
        self.video_name = "MainScreen.avi"
        self.video = imageio.get_reader(self.video_name)
        self.build_self()

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

        thread = threading.Thread(target=self.stream, args=(self.C,));  thread.daemon = 1
        thread.start()

    def show(self):
        self.C.place(x=-2, y=0)
        S["video_run"]=True
        thread = threading.Thread(target=self.stream, args=(self.C,));  thread.daemon = 1
        thread.start()

    def hide(self):
        global S
        print("gonna shut down all")
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
            self.video.set_image_index(180)
            for i in range(200, 1380):
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
        print("stream quit")
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
    root = Tk(); root.geometry('%dx%d+%d+%d' % (852,480,200,80));  root.resizable(0, 0) # #root.overrideredirect(1) # ; root.iconbitmap("icon.ico")

    S["ground_img"]=ImageTk.PhotoImage(Image.open("ground.png").convert('RGBA'))
    S["water_img"]=ImageTk.PhotoImage(Image.open("water.png").convert('RGBA'))

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
            #images_raw[]    p=1    images_P[i][p]=[]   images_P[i][p][0]-name   images_raw[images_P[i][p][0]]-pic_raw
            #pic.paste(pic_over, (0,0), pic_over)


    #root.after(500, anim, 0)

    S["recive_thread"] = threading.Thread(target=in_handle, args=(1,));  S["recive_thread"].start()
    S["send_thread"]   = threading.Thread(target=out_handle, args=(1,)); S["send_thread"].start() # x.join()

    root.protocol("WM_DELETE_WINDOW", on_closing)
    S["screens"]=[]
    S["screens"].append(StartWindow(root))
    S["screens"].append(PlayWindow(root))

    root.mainloop()


if __name__ == "__main__":
    main()


# from tkinter import messagebox
# root = tk.Tk()
# def on_closing():
#     if messagebox.askokcancel("Quit", "Do you want to quit?"):
#         root.destroy()
# root.protocol("WM_DELETE_WINDOW", on_closing)


#print(f"[ACTIVE CONNECTIONS] {threading.activeCount() - 1}")
#print(f"[NEW CONNECTION] {addr} connected.")

#L=float(e_L.get())
#e_L['bg']='green'
#e_L.delete(0,END)  tk.END

'''

'''
