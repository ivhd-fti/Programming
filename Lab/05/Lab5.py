# -*- coding: utf-8 -*-
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(549, 290)
        MainWindow.setAutoFillBackground(False)
        MainWindow.setStyleSheet(u"QPushButton:hover{color: blue; font: bold 10pt;}")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.btn_load = QPushButton(self.centralwidget)
        self.btn_load.setObjectName(u"btn_load")
        self.btn_load.setGeometry(QRect(400, 170, 141, 24))
        self.btn_load.setStyleSheet(u"")
        self.btn_save = QPushButton(self.centralwidget)
        self.btn_save.setObjectName(u"btn_save")
        self.btn_save.setEnabled(False)
        self.btn_save.setGeometry(QRect(400, 200, 141, 24))
        self.btn_encode = QPushButton(self.centralwidget)
        self.btn_encode.setObjectName(u"btn_encode")
        self.btn_encode.setEnabled(True)
        self.btn_encode.setGeometry(QRect(400, 230, 141, 24))
        self.btn_decode = QPushButton(self.centralwidget)
        self.btn_decode.setObjectName(u"btn_decode")
        self.btn_decode.setEnabled(False)
        self.btn_decode.setGeometry(QRect(400, 260, 141, 24))
        self.input = QPlainTextEdit(self.centralwidget)
        self.input.setObjectName(u"input")
        self.input.setGeometry(QRect(10, 70, 381, 211))
        self.label = QLabel(self.centralwidget)
        self.label.setObjectName(u"label")
        self.label.setGeometry(QRect(90, 10, 241, 41))
        self.info = QLabel(self.centralwidget)
        self.info.setObjectName(u"info")
        self.info.setGeometry(QRect(10, 50, 371, 16))
        self.label_2 = QLabel(self.centralwidget)
        self.label_2.setObjectName(u"label_2")
        self.label_2.setGeometry(QRect(400, 100, 61, 16))
        self.label_3 = QLabel(self.centralwidget)
        self.label_3.setObjectName(u"label_3")
        self.label_3.setGeometry(QRect(400, 140, 61, 16))
        self.ftotal = QLabel(self.centralwidget)
        self.ftotal.setObjectName(u"ftotal")
        self.ftotal.setGeometry(QRect(470, 140, 61, 16))
        self.ffree = QLabel(self.centralwidget)
        self.ffree.setObjectName(u"ffree")
        self.ffree.setGeometry(QRect(470, 100, 61, 16))
        self.fhave = QLabel(self.centralwidget)
        self.fhave.setObjectName(u"fhave")
        self.fhave.setGeometry(QRect(470, 120, 61, 16))
        self.label_6 = QLabel(self.centralwidget)
        self.label_6.setObjectName(u"label_6")
        self.label_6.setGeometry(QRect(400, 120, 61, 16))
        self.label_4 = QLabel(self.centralwidget)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setGeometry(QRect(400, 80, 161, 16))
        self.label_5 = QLabel(self.centralwidget)
        self.label_5.setObjectName(u"label_5")
        self.label_5.setGeometry(QRect(400, 10, 161, 16))
        self.label_7 = QLabel(self.centralwidget)
        self.label_7.setObjectName(u"label_7")
        self.label_7.setGeometry(QRect(400, 50, 61, 16))
        self.label_8 = QLabel(self.centralwidget)
        self.label_8.setObjectName(u"label_8")
        self.label_8.setGeometry(QRect(400, 30, 61, 16))
        self.inp_size = QLabel(self.centralwidget)
        self.inp_size.setObjectName(u"inp_size")
        self.inp_size.setGeometry(QRect(470, 50, 61, 16))
        self.inp_free = QLabel(self.centralwidget)
        self.inp_free.setObjectName(u"inp_free")
        self.inp_free.setGeometry(QRect(470, 30, 61, 16))
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.btn_load.setText(QCoreApplication.translate("MainWindow", u"\u0417\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044c \u043a\u0430\u0440\u0442\u0438\u043d\u043a\u0443", None))
        self.btn_save.setText(QCoreApplication.translate("MainWindow", u"\u0421\u043e\u0445\u0440\u0430\u043d\u0438\u0442\u044c \u043a\u0430\u0440\u0442\u0438\u043d\u043a\u0443", None))
        self.btn_encode.setText(QCoreApplication.translate("MainWindow", u"\u0417\u0430\u043a\u043e\u0434\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u043a\u0441\u0442", None))
        self.btn_decode.setText(QCoreApplication.translate("MainWindow", u"\u0414\u0435\u043a\u043e\u0434\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u043a\u0441\u0442", None))
        self.input.setPlaceholderText(QCoreApplication.translate("MainWindow", u"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u0442\u0435\u043a\u0441\u0442", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"<span style=\"font-size:22pt; color:#0000ff;\"><b> \u0421\u0442\u0435\u0433\u0430\u043d\u043e\u0433\u0440\u0430\u0444\u0438\u044f </b></span>", None))
        self.info.setText(QCoreApplication.translate("MainWindow", u"> \u0417\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u0435 \u0444\u0430\u0439\u043b.", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"\u0414\u043e\u0441\u0442\u0443\u043f\u043d\u043e:", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"\u0412\u0441\u0435\u0433\u043e:", None))
        self.ftotal.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.ffree.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.fhave.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_6.setText(QCoreApplication.translate("MainWindow", u"\u0421\u043e\u0434\u0435\u0440\u0436\u0438\u0442:", None))
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"\u0417\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u043d\u044b\u0439 \u0444\u0430\u0439\u043b:", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"\u0422\u0435\u043a\u0443\u0449\u0435\u0435 \u0441\u043e\u043e\u0431\u0449\u0435\u043d\u0438\u0435:", None))
        self.label_7.setText(QCoreApplication.translate("MainWindow", u"\u0421\u043e\u0434\u0435\u0440\u0436\u0438\u0442:", None))
        self.label_8.setText(QCoreApplication.translate("MainWindow", u"\u0414\u043e\u0441\u0442\u0443\u043f\u043d\u043e:", None))
        self.inp_size.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.inp_free.setText(QCoreApplication.translate("MainWindow", u"0", None))
    # retranslateUi

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.btn_load.clicked.connect(self.loadImage)
        self.ui.btn_save.clicked.connect(self.saveImage)
        self.ui.btn_encode.clicked.connect(self.encodeImage)
        self.ui.btn_encode.setEnabled(False)
        self.ui.btn_decode.clicked.connect(self.decodeImage)
        self.ui.btn_decode.clicked.connect(self.msgChanged)
        self.ui.input.textChanged.connect(self.msgChanged)

        self.img        = QImage()
        self.rb_pixel   = QColor()

        self.M          = 0xffffffff    # маркер длины, обозначает, что файл не загружен
        self.msg_max    = self.M        # общая ёмкость файла - 7 байт
        self.msg_in     = 0x0           # количество байт сообщения внутри файла

        self.input_size = 0x0           # размер поля input в байтах
        self.input_free = 0x0           # осталось свободных байт

        self.mark       = 0xe29da4      # 24-bit идентификатор наличия сообщения в файле //0b111000101001110110100100;

        self.cur_bit    = 3             # текущий бит в RGB
        self.rb_x       = -1            # x- координаты пикселя.  -1 самое начало
        self.rb_y       =  0            # y- координаты пикселя
        self.internal_change = False    # используется для отмены вывода сообщения "" при декодировании

    def writebyte(self, b):
        for bits in range(8):
            mask = (b & 0x80) >> 7;
            if self.cur_bit==3:
                self.cur_bit = 0;
                self.img.setPixelColor(self.rb_x, self.rb_y, self.rb_pixel);
                self.rb_x+=1
                if self.rb_x>=self.img.width():
                    self.rb_x=0; self.rb_y+=1
                self.rb_pixel = self.img.pixelColor(self.rb_x, self.rb_y);

            if self.cur_bit==0:
                self.rb_pixel.setRed(  (self.rb_pixel.red()  &0xfe) | mask )
            elif self.cur_bit==1:
                self.rb_pixel.setGreen((self.rb_pixel.green()&0xfe) | mask )
            else:
                self.rb_pixel.setBlue( (self.rb_pixel.blue() &0xfe) | mask )

            b=b<<1; self.cur_bit+=1;

    def readbyte(self):
        ret=0
        for bit in range(8):
            ret=ret<<1;
            if self.cur_bit==3:
                self.cur_bit = 0
                self.rb_x+=1
                if self.rb_x>=self.img.width():
                    self.rb_x=0
                    self.rb_y+=1
                self.rb_pixel = self.img.pixelColor(self.rb_x, self.rb_y);

            if self.cur_bit==0:    mask = self.rb_pixel.red()&1;
            elif self.cur_bit==1:  mask = self.rb_pixel.green()&1;
            else:                  mask = self.rb_pixel.blue()&1;

            ret|=mask
            self.cur_bit+=1
        return ret

    @Slot()
    def loadImage(self):
        fileName, _ = QFileDialog.getOpenFileName(self, self.tr("Open Image"), "", self.tr("Доступные форматы (*.png)"))
        if not fileName:
            if (self.msg_max==self.M): self.ui.info.setText("Загрузка отменена. Загрузите файл.")
            else:                      self.ui.info.setText("Загрузка отменена. Прежний файл.")
            return

        self.cur_bit = 3;
        self.rb_x = -1;self.rb_y = 0; res = 0;
        self.img.load(fileName)
        self.img.convertTo(QImage.Format_ARGB32)

        self.msg_max = self.img.width() * self.img.height() * 3 // 8;

        if (self.msg_max<=7):                       # файл очень маленький
            self.ui.info.setText("Файл загружен, но он слишком маленький.");
            self.msg_in = 0; self.msg_max=0; self.input_free=0;
            self.ui.btn_decode.setEnabled(False)
            self.ui.btn_encode.setEnabled(False)
            self.ui.btn_save.setEnabled(False)
            self.ui.inp_free.setText(0)
            self.ui.inp_size.setText(0)
            return
        self.msg_max-=7;

        for i in range(3):                      # проверка наличия подписи в маркера в файле
            res=(res<<8)|self.readbyte()

        self.msg_in=0
        if res==self.mark:
            for i in range(4):
                self.msg_in=(self.msg_in<<8)|self.readbyte()

        res_msg = "♥ " if self.msg_in else ""
        if self.msg_in>self.msg_max:            # ошибочная длина сообщения - больше, чем объем файла
            self.msg_in = self.msg_max          # корректируем на максимум
            res_msg = "?" + res_msg             # добавляем маркер, что была ошибка размера в заголовке

        res_msg+= "Файл загружен.";
        self.ui.info.setText(res_msg)

        self.ui.inp_free.setText(str(self.msg_max))
        self.ui.ffree.setText(   str(self.msg_max-self.msg_in))
        self.ui.ftotal.setText(  str(self.msg_max))
        self.ui.fhave.setText(   str(self.msg_in))

        if self.msg_in:     self.ui.btn_decode.setEnabled(True)
        else:               self.ui.btn_decode.setEnabled(False)

        if self.input_size: self.ui.btn_encode.setEnabled(True)
        else:               self.ui.btn_encode.setEnabled(False)

    @Slot()
    def saveImage(self):
        fileName, _ = QFileDialog.getSaveFileName(self, self.tr("Save Image"), "", self.tr("Доступные форматы (*.png)"))
        if fileName:
            self.img.save(fileName) #, 0, -1);
            self.ui.info.setText("Изображение успешно сохранено")
            self.ui.btn_save.setEnabled(False)
            self.ui.btn_decode.setEnabled(True)
        else: self.ui.info.setText("Сохранение отменено.")

    @Slot()
    def encodeImage(self):
        self.cur_bit = 0; self.rb_x = 0; self.rb_y = 0;                   # начало файла
        self.rb_pixel = self.img.pixelColor(self.rb_x, self.rb_y)
        self.writebyte(0xe2); self.writebyte(0x9d); self.writebyte(0xa4); # записываем маркер в начало файла

        m_size = self.input_size
        byte_write = 0

        for i in range(4):
            byte_write = 0;
            for j in range(8):
                byte_write = byte_write << 1;
                if (m_size&0x80000000): byte_write = byte_write | 1;
                m_size=m_size<<1;
            self.writebyte(byte_write);

        txt = self.ui.input.toPlainText().encode('utf-8')
        self.input_size = len(txt)

        for i in range(self.input_size):
            self.writebyte(txt[i])
        self.img.setPixelColor(self.rb_x, self.rb_y, self.rb_pixel)

        self.ui.btn_decode.setEnabled(True)
        self.ui.btn_save.setEnabled(True)

        self.ui.info.setText("Сообщение добавлено.")
        self.ui.ffree.setText(str(self.msg_max-self.input_size))
        self.ui.fhave.setText(str(self.input_size))

    @Slot()
    def decodeImage(self):
        if ((self.msg_max==self.M) or (not self.msg_in)): return

        # определение стартовой позиции
        self.rb_y = 56//(self.img.width()*3);                    # 7 байт заголовка * 8 бит = 56 бит
        self.rb_x = (56 - self.img.width()*3*self.rb_y) // 3;     # бит в строке

        self.cur_bit = 2;
        self.rb_pixel = self.img.pixelColor(self.rb_x, self.rb_y);

        msg=[]
        for i in range(self.msg_in):
            msg.append(self.readbyte())

        self.ui.input.setPlainText( bytearray(msg).decode("utf-8") )
        self.input_size=self.msg_in;

        self.internal_change = True
        self.ui.info.setText(("♥ " if self.msg_in else "") + "Сообщение декодировано.")

        self.ui.btn_encode.setEnabled(True)
        self.ui.btn_save.setEnabled(False)

        self.input_free = self.msg_max - self.input_size;
        self.ui.inp_size.setText(str(self.input_size))
        self.ui.inp_free.setText(str(self.input_free))

    @Slot()
    def msgChanged(self):
        txt = self.ui.input.toPlainText().encode('utf-8')

        self.input_size = len(txt)
        if self.msg_max==self.M:
            self.input_free = 0;
            self.ui.inp_size.setText("Загрузите файл.")
            self.ui.inp_size.setText(str(self.input_size))
            self.ui.inp_size.setText("0")
            self.ui.btn_encode.setEnabled(False)
            return

        if (self.input_size>self.msg_max):
            self.input_free = 0;
            out_text = "Размер текста превышен на: ";
            out_text += str(self.input_size-self.msg_max);
            self.ui.info.setText(out_text)
            self.ui.inp_size.setText(str(self.input_size))
            self.ui.inp_free.setText("0")
            self.ui.btn_encode.setEnabled(False)
            return

        self.input_free = self.msg_max - self.input_size
        if not self.internal_change: self.ui.info.setText("") # если это не ввод в поле input при декодировании
        self.internal_change=False

        self.ui.inp_size.setText(str(self.input_size))
        self.ui.inp_free.setText(str(self.input_free))
        self.ui.btn_encode.setEnabled(True)

if __name__ == "__main__":
    import sys
    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
