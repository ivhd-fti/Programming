from math import pi as pi;from math import cos as cos;from math import sin  as sin
from math import sqrt as sqrt;from math import atan2 as atan
from math import hypot as hypot
from math import gcd as gcd
from math import nan as nan
from math import isnan as isnan
from math import isinf as isinf
from math import copysign as copysign

#Метод __init__. Кроме self должен принимать ещё два параметра: numerator и denominator - целые числа:
#    Если аргументы не заданы. То создаётся рациональное число инициализированное нулём;
#    Если задан только numerator. То создаётся рациональное число инициализированное
#Если заданы оба аргумента. То создаётся рациональное число инициализированное

class Rational():
    def __init__(self, numerator=None, denominator=None):
        if numerator==None and denominator==None: # no arguments=> set to 0/1=0
            self.numerator_, self.denominator_ = 0, 1
            self.float = 0
        elif not isinstance(numerator, int) or numerator == denominator == 0:   # нет numerator или два нуля? уходим в 0,0=nan
            self.numerator_, self.denominator_ = 0, 0
            self.float = nan
        elif isinstance(denominator, int):                                      #  оба аргумента - int
            self.numerator_, self.denominator_ = numerator, denominator
            if not denominator:
                self.float = copysign(float("inf"), numerator)                  #float("inf")*abs(numerator)/numerator
            else:
                self.float = numerator / denominator
        else:                                                                   # numerator-int, denominator-trash
            self.numerator_, self.denominator_ = numerator, 1
            self.float = float(numerator)

        if self.denominator_<0: ##// избавляемся от минуса в знаменателе
            self.denominator_, self.numerator_ = -self.denominator_, -self.numerator_

    def gcd(a, b): return gcd(a, b)

    def __repr__(self):
        return "[" + str(self.numerator_) + "/" + str(self.denominator_) + " > " + str(self.float) + "]"

    def __str__(self):
        return "[" + str(self.numerator_) + "/" + str(self.denominator_) + " > " + str(self.float) + "]"

    def __eq__(self, Other): # Метод __eq__ (оператор ==). Сравнивает два объекта класса Rational на равенство.
        return self.float == Other.float # and not isinf(self.float)
        # Обратите внимание NaN, с точки зрения математики - это неопределённое число, то есть любое. Поэтому он не равен ни чему, даже самому себе.

    def __add__(self, Other): #Метод __add__ (бинарный +). Левый и правый Rational.     Возвращает рациональное число являющееся суммой левого и правого операндов.
        if isinf(self.float) and isinf(Other.float):
            if isnan(self.float + Other.float):
                return  Rational(numerator = 0, denominator = 0)
            return      Rational(numerator = int(copysign(1, self.float + Other.float)), denominator = 0)
        return          Rational(numerator = self.numerator_   * Other.denominator_ + Other.numerator_ * self.denominator_, \
                               denominator = self.denominator_ * Other.denominator_)

    def __sub__(self, Other): #Метод __sub__ (бинарный -). Левый и правый Rational.     Возвращает рациональное число являющееся разностью левого и правого операндов.
        if isinf(self.float) and isinf(Other.float):
            if isnan(self.float - Other.float):
                return  Rational(numerator = 0, denominator = 0)
            return      Rational(numerator = int(copysign(1, self.float - Other.float)), denominator = 0)
        return          Rational(numerator = self.numerator_   * Other.denominator_ - Other.numerator_ * self.denominator_, \
                               denominator = self.denominator_ * Other.denominator_)

    def __mul__(self, Other): #Метод __mul__ (оператор *). Левый и правый Rational.     Возвращает рациональное число являющееся произведением левого и правого операндов.
        return Rational(numerator = self.numerator_   * Other.numerator_,     \
                      denominator = self.denominator_ * Other.denominator_)

    def __truediv__(self, Other):  #Метод __truediv__ (оператор /). Левый и правый Rational. Возвращает рациональное число являющееся частным левого и правого операндов.
        if isinf(self.float) and not isnan(Other.float) and not isinf(Other.float):
            return  Rational(numerator = int(copysign(1, self.numerator_ * Other.float)), denominator = 0)
        return      Rational(numerator = self.numerator_   * Other.denominator_, \
                           denominator = self.denominator_ * Other.numerator_)

    def __bool__(self):
        return not (self.numerator_ == 0 and self.denominator_ != 0)  #__bool__ - возвращает false если число равно нулю и true в остальных случаях;
        #return bool(self.numerator_ or not self.denominator_)
    def __float__(self):        #__float__ - возвращает вещественное число соответствующее рациональному;
        return self.float
    def numerator(self):        #Метод numerator - возвращающий числитель;
        return self.numerator_
    def denominator(self):      #Метод denominator - возвращающий знаменатель;
        return self.denominator_
    def isNaN(self):            #Метод isNaN - возвращающий True, если число соответствует 0/0
        return isnan(self.float)
        #return self.numerator_ == self.denominator_ == 0

import math
def equal(a, b, e=1E-10):
    if -e < a - b < e: return True
    else: return False

def test_38():
    if (Rational.gcd(91, 65) == 13 and
        Rational.gcd(10, 3) == 1 and
        Rational.gcd(-10, 3) == 1 and
        Rational.gcd(10, -3) == 1 and
        Rational.gcd(-10, -3) == 1 and
        Rational.gcd(30, 10) == 10 and
        Rational.gcd(10, 30) == 10 and
        Rational.gcd(0, 10) == 10 and
        Rational.gcd(10, 0) == 10
        ): print('gcd test passed')
    else: print('gcd test failed')

    if (not Rational(22, 0).isNaN() and
        not Rational(22, 9).isNaN() and
        not Rational(0, 9).isNaN() and
        not Rational(-22, 9).isNaN() and
        not Rational(-22, 0).isNaN() and
        Rational(0, 0).isNaN()
        ): print('isNaN test passed')
    else: print('isNaN test failed')

    if (Rational(22, 0) == Rational(22, 0) and
        Rational(22, 0) == Rational(9, 0) and
        not(Rational(22, 0) == Rational(22, 9)) and
        not(Rational(22, 0) == Rational(-22, 9)) and
        not(Rational(22, 0) == Rational(-22, 0)) and
        not(Rational(22, 0) == Rational(0, 9)) and
        not(Rational(22, 0) == Rational(0, 0)) and
        Rational(22, 9) == Rational(22, 9) and
        Rational(22, 9) == Rational(-22, -9) and
        Rational(22, 9) == Rational(110, 45) and
        Rational(22, 9) == Rational(-110, -45) and
        not(Rational(22, 9) == Rational(-22, 9)) and
        not(Rational(22, 9) == Rational(22, -9)) and
        not(Rational(22, 9) == Rational(9, 22)) and
        not(Rational(22, 9) == Rational(22, 0)) and
        not(Rational(22, 9) == Rational(-22, 0)) and
        not(Rational(22, 9) == Rational(0, 9)) and
        not(Rational(22, 9) == Rational(0, 0)) and
        Rational(0, 1) == Rational(0, 1) and
        Rational(0, 1) == Rational(0, 9)  and
        Rational(0, 1) == Rational(0, -9)  and
        not(Rational(0, 1) == Rational(22, 9))  and
        not(Rational(0, 1) == Rational(-22, 9))  and
        not(Rational(0, 1) == Rational(22, 0)) and
        not(Rational(0, 1) == Rational(-22, 0)) and
        not(Rational(0, 1) == Rational(0, 0)) and
        Rational(-22, 9) == Rational(-22, 9) and
        Rational(-22, 9) == Rational(22, -9) and
        Rational(-22, 9) == Rational(-110, 45) and
        Rational(-22, 9) == Rational(110, -45) and
        not(Rational(-22, 9) == Rational(-22, -9)) and
        not(Rational(-22, 9) == Rational(22, 9)) and
        not(Rational(-22, 9) == Rational(9, -22)) and
        not(Rational(-22, 9) == Rational(22, 0)) and
        not(Rational(-22, 9) == Rational(-22, 0)) and
        not(Rational(-22, 9) == Rational(0, 9)) and
        not(Rational(-22, 9) == Rational(0, 0)) and
        Rational(-22, 0) == Rational(-22, 0) and
        Rational(-22, 0) == Rational(-9, 0) and
        not(Rational(-22, 0) == Rational(22, 9)) and
        not(Rational(-22, 0) == Rational(-22, 9)) and
        not(Rational(-22, 0) == Rational(22, 0)) and
        not(Rational(-22, 0) == Rational(0, 9)) and
        not(Rational(-22, 0) == Rational(0, 0)) and
        not(Rational(0, 0) == Rational(0, 0))
        ): print('Equality test passed')
    else: print('Equality test failed')

    if (Rational(22, 0) + Rational(22, 0) == Rational(22, 0) and
        Rational(22, 9) + Rational(22, 0) == Rational(22, 0) and
        Rational(0, 9) + Rational(22, 0) == Rational(22, 0) and
        Rational(-22, 9) + Rational(22, 0) == Rational(22, 0) and
        (Rational(-22, 0) + Rational(22, 0)).isNaN() and
        Rational(22, 0) + Rational(22, 9) == Rational(22, 0) and
        Rational(22, 9) + Rational(22, 9) == Rational(44, 9) and
        Rational(0, 9) + Rational(22, 9) == Rational(22, 9) and
        Rational(-22, 9) + Rational(22, 9) == Rational(0, 9) and
        Rational(-22, 0) + Rational(22, 9) == Rational(-22, 0) and
        Rational(22, 0) + Rational(0, 1) == Rational(22, 0) and
        Rational(22, 9) + Rational(0, 1) == Rational(22, 9) and
        Rational(0, 9) + Rational(0, 1) == Rational(0, 9) and
        Rational(-22, 9) + Rational(0, 1) == Rational(-22, 9) and
        Rational(-22, 0) + Rational(0, 1) == Rational(-22, 0) and
        Rational(22, 0) + Rational(-22, 9) == Rational(22, 0) and
        Rational(22, 9) + Rational(-22, 9) == Rational(0, 9) and
        Rational(0, 9) + Rational(-22, 9) == Rational(-22, 9) and
        Rational(-22, 9) + Rational(-22, 9) == Rational(-44, 9) and
        Rational(-22, 0) + Rational(-22, 9) == Rational(-22, 0) and
        (Rational(22, 0) + Rational(-22, 0)).isNaN() and
        Rational(22, 9) + Rational(-22, 0) == Rational(-22, 0) and
        Rational(0, 9) + Rational(-22, 0) == Rational(-22, 0) and
        Rational(-22, 9) + Rational(-22, 0) == Rational(-22, 0) and
        Rational(-22, 0) + Rational(-22, 0) == Rational(-22, 0) and
        (Rational(22, 0) + Rational(0, 0)).isNaN() and
        (Rational(22, 9) + Rational(0, 0)).isNaN() and
        (Rational(0, 9) + Rational(0, 0)).isNaN() and
        (Rational(-22, 9) + Rational(0, 0)).isNaN() and
        (Rational(-22, 0) + Rational(0, 0)).isNaN()
        ): print('Summation test passed')
    else: print('Summation test failed')

    if ((Rational(22, 0) - Rational(22, 0)).isNaN() and
        Rational(22, 9) - Rational(22, 0) == Rational(-22, 0) and
        Rational(0, 9) - Rational(22, 0) == Rational(-22, 0) and
        Rational(-22, 9) - Rational(22, 0) == Rational(-22, 0) and
        Rational(-22, 0) - Rational(22, 0) == Rational(-22, 0) and
        Rational(22, 0) - Rational(22, 9) == Rational(22, 0) and
        Rational(22, 9) - Rational(22, 9) == Rational(0, 9) and
        Rational(0, 9) - Rational(22, 9) == Rational(-22, 9) and
        Rational(-22, 9) - Rational(22, 9) == Rational(-44, 9) and
        Rational(-22, 0) - Rational(22, 9) == Rational(-22, 0) and
        Rational(22, 0) - Rational(0, 1) == Rational(22, 0) and
        Rational(22, 9) - Rational(0, 1) == Rational(22, 9) and
        Rational(0, 9) - Rational(0, 1) == Rational(0, 9) and
        Rational(-22, 9) - Rational(0, 1) == Rational(-22, 9) and
        Rational(-22, 0) - Rational(0, 1) == Rational(-22, 0) and
        Rational(22, 0) - Rational(-22, 9) == Rational(22, 0) and
        Rational(22, 9) - Rational(-22, 9) == Rational(44, 9) and
        Rational(0, 9) - Rational(-22, 9) == Rational(22, 9) and
        Rational(-22, 9) - Rational(-22, 9) == Rational(0, 9) and
        Rational(-22, 0) - Rational(-22, 9) == Rational(-22, 0) and
        Rational(22, 0) - Rational(-22, 0) == Rational(22, 0) and
        Rational(22, 9) - Rational(-22, 0) == Rational(22, 0) and
        Rational(0, 9) - Rational(-22, 0) == Rational(22, 0) and
        Rational(-22, 9) - Rational(-22, 0) == Rational(22, 0) and
        (Rational(-22, 0) - Rational(-22, 0)).isNaN() and
        (Rational(22, 0) - Rational(0, 0)).isNaN() and
        (Rational(22, 9) - Rational(0, 0)).isNaN() and
        (Rational(0, 9) - Rational(0, 0)).isNaN() and
        (Rational(-22, 9) - Rational(0, 0)).isNaN() and
        (Rational(-22, 0) - Rational(0, 0)).isNaN()
        ): print('Subtraction test passed')
    else: print('Subtraction test failed')

    if (Rational(22, 0) * Rational(22, 0) == Rational(22, 0) and
        Rational(22, 9) * Rational(22, 0) == Rational(22, 0) and
        (Rational(0, 9) * Rational(22, 0)).isNaN() and
        Rational(-22, 9) * Rational(22, 0) == Rational(-22, 0) and
        Rational(-22, 0) * Rational(22, 0) == Rational(-22, 0) and
        Rational(22, 0) * Rational(22, 9) == Rational(22, 0) and
        Rational(22, 9) * Rational(22, 9) == Rational(22*22, 9*9) and
        Rational(0, 9) * Rational(22, 9) == Rational(0, 9) and
        Rational(-22, 9) * Rational(22, 9) == Rational(-22*22, 9*9) and
        Rational(-22, 0) * Rational(22, 9) == Rational(-22, 0) and
        (Rational(22, 0) * Rational(0, 1)).isNaN() and
        Rational(22, 9) * Rational(0, 1) == Rational(0, 9) and
        Rational(0, 9) * Rational(0, 1) == Rational(0, 9) and
        Rational(-22, 9) * Rational(0, 1) == Rational(0, 9) and
        (Rational(-22, 0) * Rational(0, 1)).isNaN() and
        Rational(22, 0) * Rational(-22, 9) == Rational(-22, 0) and
        Rational(22, 9) * Rational(-22, 9) == Rational(-22*22, 9*9) and
        Rational(0, 9) * Rational(-22, 9) == Rational(0, 9) and
        Rational(-22, 9) * Rational(-22, 9) == Rational(22*22, 9*9) and
        Rational(-22, 0) * Rational(-22, 9) == Rational(22, 0) and
        Rational(22, 0) * Rational(-22, 0) == Rational(-22, 0) and
        Rational(22, 9) * Rational(-22, 0) == Rational(-22, 0) and
        (Rational(0, 9) * Rational(-22, 0)).isNaN() and
        Rational(-22, 9) * Rational(-22, 0) == Rational(22, 0) and
        Rational(-22, 0) * Rational(-22, 0) == Rational(22, 0) and
        (Rational(22, 0) * Rational(0, 0)).isNaN() and
        (Rational(22, 9) * Rational(0, 0)).isNaN() and
        (Rational(0, 9) * Rational(0, 0)).isNaN() and
        (Rational(-22, 9) * Rational(0, 0)).isNaN() and
        (Rational(-22, 0) * Rational(0, 0)).isNaN()
        ): print('Multiplication test passed')
    else: print('Multiplication test failed')

    if ((Rational(22, 0) / Rational(22, 0)).isNaN() and
        Rational(22, 9) / Rational(22, 0) == Rational(0, 9) and
        Rational(0, 9) / Rational(22, 0) == Rational(0, 9) and
        Rational(-22, 9) / Rational(22, 0) == Rational(0, 9) and
        (Rational(-22, 0) / Rational(22, 0)).isNaN() and
        Rational(22, 0) / Rational(22, 9) == Rational(22, 0) and
        Rational(22, 9) / Rational(22, 9) == Rational(9, 9) and
        Rational(0, 9) / Rational(22, 9) == Rational(0, 9) and
        Rational(-22, 9) / Rational(22, 9) == Rational(-9, 9) and
        Rational(-22, 0) / Rational(22, 9) == Rational(-22, 0) and
        Rational(22, 0) / Rational(0, 1) == Rational(22, 0) and
        Rational(22, 9) / Rational(0, 1) == Rational(22, 0) and
        (Rational(0, 9) / Rational(0, 1)).isNaN() and
        Rational(-22, 9) / Rational(0, 1) == Rational(-22, 0) and
        Rational(-22, 0) / Rational(0, 1) == Rational(-22, 0) and
        Rational(22, 0) / Rational(-22, 9) == Rational(-22, 0) and
        Rational(22, 9) / Rational(-22, 9) == Rational(-9, 9) and
        Rational(0, 9) / Rational(-22, 9) == Rational(0, 9) and
        Rational(-22, 9) / Rational(-22, 9) == Rational(9, 9) and
        Rational(-22, 0) / Rational(-22, 9) == Rational(22, 0) and
        (Rational(22, 0) / Rational(-22, 0)).isNaN() and
        Rational(22, 9) / Rational(-22, 0) == Rational(0, 9) and
        Rational(0, 9) / Rational(-22, 0) == Rational(0, 9) and
        Rational(-22, 9) / Rational(-22, 0) == Rational(0, 9) and
        (Rational(-22, 0) / Rational(-22, 0)).isNaN() and
        (Rational(22, 0) / Rational(0, 0)).isNaN() and
        (Rational(22, 9) / Rational(0, 0)).isNaN() and
        (Rational(0, 9) / Rational(0, 0)).isNaN() and
        (Rational(-22, 9) / Rational(0, 0)).isNaN() and
        (Rational(-22, 0) / Rational(0, 0)).isNaN()
        ): print('Division test passed')
    else: print('Division test failed')

    if (equal(float(Rational(-22, -9)), 22/9.0) and
        equal(float(Rational(-9, -9)), 1) and
        equal(float(Rational(-6, -9)), 6/9.0) and
        equal(float(Rational(0, -9)), 0) and
        equal(float(Rational(6, -9)), -6/9.0) and
        equal(float(Rational(9, -9)), -1) and
        equal(float(Rational(22, -9)), -22/9.0) and
        math.isinf(float(Rational(-9, 0))) and
        math.isnan(float(Rational(0, 0))) and
        math.isinf(float(Rational(9, 0))) and
        equal(float(Rational(-22, 9)), -22/9.0) and
        equal(float(Rational(-9, 9)), -1) and
        equal(float(Rational(-6, 9)), -6/9.0) and
        equal(float(Rational(0, 9)), 0) and
        equal(float(Rational(6, 9)), 6/9.0) and
        equal(float(Rational(9, 9)), 1) and
        equal(float(Rational(22, 9)), 22/9.0)
        ): print('Conversion to double test passed')
    else: print('Conversion to double test failed')

    if (bool(Rational(-22, -9)) and
        bool(Rational(-9, -9)) and
        bool(Rational(-6, -9)) and
        not bool(Rational(0, -9)) and
        bool(Rational(6, -9)) and
        bool(Rational(9, -9)) and
        bool(Rational(22, -9)) and
        bool(Rational(-9, 0)) and
        bool(Rational(0, 0)) and
        bool(Rational(9, 0)) and
        bool(Rational(-22, 9)) and
        bool(Rational(-9, 9)) and
        bool(Rational(-6, 9)) and
        not bool(Rational(0, 9)) and
        bool(Rational(6, 9)) and
        bool(Rational(9, 9)) and
        bool(Rational(22, 9))
        ): print('Conversion to bool test passed')
    else: print('Conversion to bool test failed')

test_38()

#print(Rational(-22, 0) * Rational(22, -9) == Rational(22, 0))
#print(Rational(0, 1) * Rational(-22, 0) == Rational(22, 0))
#print(Rational(0, 9) / Rational(-22, 0) == Rational(0, 9))
#print(0/-float('inf'))


#
