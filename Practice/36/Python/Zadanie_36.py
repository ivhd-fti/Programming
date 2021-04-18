from math import pi as pi
from math import cos as cos
from math import sin  as sin
from math import sqrt as sqrt
from math import atan2 as atan

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

### Test part ######################################################
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



















#
