#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    // file vars
    uint32_t M          = 0xffffffff; // маркер длины, обозначает, что файл не загружен
    uint32_t msg_max    = M; //0x0;        // общая ёмкость файла - 7 байт
    uint32_t msg_in     = 0x0;        // количество байт сообщения внутри файла

    uint32_t input_size = 0x0;        // размер поля input в байтах
    uint32_t input_free = 0x0;        // осталось свободных байт

    uint32_t const mark = 0xe29da4; // 24-bit идентификатор наличия сообщения в файле //0b111000101001110110100100;

    uint8_t cur_bit = 3; // текущий бит в RGB
    int rb_x = -1, rb_y = 0;

    QImage img;
    QColor rb_pixel;

    void writebyte(uint8_t);
    uint8_t readbyte();  // поточное чтение байт


public slots:
    void loadImage();
    void saveImage();
    void encode();
    void decode();
    void msgChanged();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
