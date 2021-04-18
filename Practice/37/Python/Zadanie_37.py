from math import pi as pi;from math import cos as cos;from math import sin  as sin
from math import sqrt as sqrt;from math import atan2 as atan
from math import hypot as hypot
import enum
class Cords(enum.IntEnum):
    Cartesian = 1
    Polar = enum.auto()
class Point():

    def __init__(self, a1=0, a2=0, coord_system=Cords.Cartesian):

        self.R, self.EPS = None, 10**-10 # self.R=None индикатор, что его надо рассчитать

        if isinstance(a1, str):
            try: self.x,  self.y = map(float, a1[1:-1].split(','))
            except: #raise TypeError("Only integers are allowed")
                self.x = self.y = 0
                print("Неверный формат координат. Взяты предустановленные значения.")
        elif isinstance(a1, (int, float)) and isinstance(a2, (int, float)):
            if coord_system==Cords.Cartesian: # not 'Polar'
                self.x,  self.y = a1, a2
            else:
                self.x, self.y, self.R, self.ang = a1*cos(a2), a1*sin(a2), a1, a2
        else:
            self.x = self.y = 0
            print("Неверный формат координат. Взяты предустановленные значения.")

        if self.R == None: self.CartesianToPolar()

    def PolarToCartesian(self):
        self.x, self.y = self.R*cos(self.ang), self.R*sin(self.ang)

    def CartesianToPolar(self):
        self.R   = sqrt(self.x**2 + self.y**2)
        self.ang = atan(self.y, self.x)

    def __eq__(self, Other): #==
        return abs(self.x - Other.x)<=self.EPS and abs(self.y - Other.y)<=self.EPS

    def __ne__(self, Other): #!=
        return abs(self.x - Other.x)>self.EPS or abs(self.y - Other.y)>self.EPS

    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + ")"

    def __repr__(self):
        return "(" + str(self.x) + "," + str(self.y) + ")"

    def get_x(self):
        return self.x

    def get_y(self):
        return self.y

    def get_r(self):
        return self.R

    def get_phi(self):
        return self.ang

    def set_x(self, x):
        self.x = x
        self.CartesianToPolar()

    def set_y(self, y):
        self.y = y
        self.CartesianToPolar()

    def set_r(self, R):
        self.R = R
        self.PolarToCartesian()

    def set_phi(self, ang):
        self.ang = ang
        self.PolarToCartesian()

#a = Vector(Point(1, 2))
#b = Vector(Point(-2, 0), Point(-1, 2))
#Если аргументы не заданы, то создаётся вектор единичной длины в направлении оси
#Если задан один аргумент, создаётся вектор направленный из начала координат в точку переданную в качестве параметра.
#Если заданы оба аргументаbegin и end, то создаётся вектор направленный из begin в end.
class Vector():
    def __init__(self, a=None, b=None ):
        if not isinstance(a, Point) and not isinstance(b, Point):
            self.begin = Point(0.0, 0.0)
            self.end = Point(1.0, 0.0)
        elif not isinstance(b, Point):
            self.begin = Point(0.0, 0.0)
            self.end = a
        else:
            self.begin = a
            self.end = b

        self.cord = [self.end.x - self.begin.x, self.end.y - self.begin.y]  # vector coordinates
        self.len  = hypot(*self.cord)                                       # fast len calculation
        self.ang  = atan(self.cord[1], self.cord[0])                        # angel in radians

    def __repr__(self):
        return "[" + str(self.begin) + "," + str(self.end) + "]=(" + str(self.cord[0]) + "," + str(self.cord[1]) + ")"

    def __str__(self):
        return "[" + str(self.begin) + "," + str(self.end) + "]=(" + str(self.cord[0]) + "," + str(self.cord[1]) + ")"

    def __eq__(self, Other): #== #__eq__ (оператор ==). Сравнивает два объекта класса Vector на равенство. Точность сравнения 10-10.
        return self.cord == Other.cord

    def length(self): #length. Метод не принимает параметров и возвращает длину вектора;
        return self.len

    def __mul__(self, Other):
        if isinstance(Other, (int, float)):             #__mul__ (бинарный *). Левый операнд класса Vector, правый типа double.  Возвращает новый вектор компоненты которого умножены на правый операнд.
            return Vector(a=self.begin, b=Point(self.begin.x+self.cord[0]*Other, self.begin.y+self.cord[1]*Other))
        elif isinstance(Other, Vector):                 #__mul__ (бинарный *). Левый и правый операнды класса Vector.  Возвращает вещественное число вычисляющееся как скалярное произведение векторов-операндов.
            return self.len * Other.len * cos(self.ang-Other.ang)
        return self                             # просто возвращаем свой вектор без изменений: ошибки не идет.

    def __neg__(self): #__neg__ (унарный -). Возвращает новый вектор направление которого противоположно направлению вектора к которому применяется оператор.
        return self.__mul__(-1)
        #Vector(begin=self.begin, end=Point(self.begin.x-self.cord[0], self.begin.y-self.cord[1]))

    #__add__ (бинарный +). Левый и правый операнды класса Vector.  новый вектор являющийся покомпонентной суммой левого и правого операндов.
    def __add__(self, Other): # в условии не указано, чью точку begin брать за начало нового вектора. Берем self.begin
        return Vector(a=self.begin, b=Point(self.end.x+Other.cord[0], self.end.y+Other.cord[1]))

    #__sub__ (бинарный -). Левый и правый операнды класса Vector.  новый вектор являющийся покомпонентной разностью левого и правого операндов.
    def __sub__(self, Other):
        return Vector(a=self.begin, b=Point(self.end.x-Other.cord[0], self.end.y-Other.cord[1]))
        # return self.__add__(Other.__neg__())


### Test 37 part ######################################################
def test_37():
    import math
    def equal(a, b, e=1E-10):
        if -e < a - b < e: return True
        else: return False

    a = Vector(Point(1, 2))
    b = Vector(Point(-2, 0), Point(-1, 2))

    if a == b and b == a: print('Equality test passed')
    else: print('Equality test failed')

    na  = Vector(Point(-1, -2))
    ox  = Vector(Point( 1,  0))
    nox = Vector(Point(-1,  0))
    oy  = Vector(Point( 0,  1))
    noy = Vector(Point( 0, -1))

    if a == -na and na == -a and -ox == nox and -oy == noy: print('Invert test passed')
    else: print('Invert test failed')

    if ox + oy + oy == a and -ox == -a + oy + oy: print('Summation test passed')
    else: print('Summation test failed')

    if -ox + oy == oy - ox and -oy + ox == ox - oy: print('Subtraction test passed')
    else: print('Subtraction test failed')

    if (ox * 3 == ox + ox + ox and
        oy * 3 == oy + oy + oy and
        ox * (-3) == -ox - ox - ox and
        oy * (-3) == -oy - oy - oy): print('Multiplication by number test passed')
    else: print('Multiplication by number test failed')

    if (equal(ox.length(), 1) and
        equal(oy.length(), 1) and
        equal((ox * 3 + oy * 4).length(), 5)): print('Length test passed')
    else: print('Length test failed')

    if (equal(ox*ox, ox.length()**2) and
        equal(oy*oy, oy.length()**2) and
        equal((ox*3 + oy*4)*(ox*3 + oy*4), (ox*3 + oy*4).length()**2)): print('Multiplication by Vector test passed')
    else: print('Multiplication by Vector test failed')

### Test 36 part ######################################################
def test_36():
    import copy
    with open('data.txt') as fin:
        original = [Point(p) for p in fin.readline().split(', ')]
    simulacrum = copy.deepcopy(original)

    for p in simulacrum:
        print(p, end='')
        p.set_x(p.get_x() + 10)
        p.set_phi(p.get_phi() + 180*pi/180)
        p.set_y(-p.get_y())
        p.set_x(-p.get_x() - 10)
        print(p)

    print('\nIt works!\n' if simulacrum == original else '\nIt not works!\n')

test_37()
