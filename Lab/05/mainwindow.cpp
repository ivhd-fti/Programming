#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QByteArray>
#include <QString>

void MainWindow::writebyte(uint8_t b){
    uint8_t bits=8, mask;
    while (bits--){
        mask = (b & 0x80) >> 7;
        switch (cur_bit){
            case 3:
                cur_bit = 0;
                img.setPixelColor(rb_x, rb_y, rb_pixel);
                if (++rb_x>=img.width()){rb_x=0;rb_y++;}
                rb_pixel = img.pixelColor(rb_x, rb_y);
            case 0:
                rb_pixel.setRed(  (rb_pixel.red()  &0xfe) | mask );
                break;
            case 1:
                rb_pixel.setGreen((rb_pixel.green()&0xfe) | mask );
                break;
            default: // 2
                rb_pixel.setBlue( (rb_pixel.blue() &0xfe) | mask );
        }
    b=b<<1; cur_bit++;
    }
}

uint8_t MainWindow::readbyte(){
    uint8_t ret=0, bits=8, mask;
    while (bits--){
        ret=ret<<1;
        switch (cur_bit){
            case 3:
                cur_bit = 0;
                if (++rb_x>=img.width()){rb_x=0;rb_y++;}
                rb_pixel = img.pixelColor(rb_x, rb_y);
            case 0:
                mask = rb_pixel.red()&1;
                break;
            case 1:
                mask = rb_pixel.green()&1;
                break;
            default: // 2
                mask = rb_pixel.blue()&1;
        }
        ret|=mask; cur_bit++;
    }
    return ret;
}

void MainWindow::loadImage(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Допустимые форматы (*.png)"));
    if (fileName == ""){
        if (msg_max==M) ui->info->setText("Загрузка отменена. Загрузите файл.");
        else            ui->info->setText("Загрузка отменена. Прежний файл.");
        return;}

    cur_bit = 3; rb_x = -1, rb_y = 0; uint32_t res = 0;
    img.load(fileName);
    img.convertTo(QImage::Format_ARGB32);

    msg_max = img.width() * img.height() * 3 / 8;

    if (msg_max<=7){ // файл очень маленький
        ui->info->setText("Файл загружен, но он слишком маленький.");
        msg_in = 0; msg_max=0; input_free=0;
        ui->btn_decode->setEnabled(false);
        ui->btn_encode->setEnabled(false);
        ui->btn_save->setEnabled(false);

        ui->inp_free->setText(QString::number(0));
        ui->inp_size->setText(QString::number(0));
        return;}

    msg_max-=7;
    // проверка наличия подписи в маркера в файле
    for (int i=0;i<3;i++) res=(res<<8)|MainWindow::readbyte();
    msg_in=0;
    if (res==mark) {
        for (int i=0;i<4;i++) msg_in=(msg_in<<8)|MainWindow::readbyte();
    }

    QString res_msg = ((msg_in)? "♥ ":"");
    if (msg_in>msg_max){ // ошибочная длина сообщения - больше, чем объем файла
        msg_in=msg_max;  // корректируем на максимум
        res_msg = "?"+res_msg;
    }

    res_msg+= "Файл загружен.";
    ui->info->setText(res_msg);
    ui->inp_free->setText(QString::number(msg_max));

    ui->ffree->setText(QString::number(msg_max-msg_in));
    ui->ftotal->setText(QString::number(msg_max));
    ui->fhave->setText(QString::number(msg_in));

    if (msg_in) ui->btn_decode->setEnabled(true);
    else ui->btn_decode->setEnabled(false);

    if (input_size) ui->btn_encode->setEnabled(true);
    else ui->btn_encode->setEnabled(false);
}

void MainWindow::saveImage(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Допустимые форматы (*.png)"));
    if (fileName != ""){
        img.save(fileName, 0, -1);
        ui->info->setText("Изображение успешно сохранено");
        ui->btn_save->setEnabled(false);
        ui->btn_decode->setEnabled(true);
    }
    else{ ui->info->setText("Сохранение отменено.");}
}

void MainWindow::encode(){
    cur_bit = 0; rb_x = 0; rb_y = 0; rb_pixel = img.pixelColor(rb_x, rb_y);
    writebyte(0xe2); writebyte(0x9d); writebyte(0xa4); // записываем маркер в начало файла

    uint32_t m_size = input_size;
    uint8_t byte_write = 0;
    for (int i=0;i<4;i++){
        byte_write = 0;
        for (int j=0;j<8;j++){
            byte_write = byte_write << 1;
            if (m_size&0x80000000) byte_write = byte_write | 1;
            m_size=m_size<<1;
        }
        writebyte(byte_write);
    }

    QByteArray txt = ui->input->toPlainText().toUtf8();
    input_size = txt.size();

    for (uint32_t i=0; i<input_size;i++){
        writebyte(txt[i]);
    } img.setPixelColor(rb_x, rb_y, rb_pixel);

    ui->btn_decode->setEnabled(true);
    ui->btn_save->setEnabled(true);
    ui->info->setText("Сообщение добавлено.");
    ui->ffree->setText(QString::number(msg_max-input_size));
    ui->fhave->setText(QString::number(input_size));
}

void MainWindow::decode(){
    if ((msg_max==M) or (!msg_in)) return;

    // определение стартовой позиции
    rb_y = 56/(img.width()*3); // 7 байт заголовка * 8 бит = 56 бит
    rb_x = (56 - img.width()*3*rb_y)/3; // бит в строке

    cur_bit = 2; //rb_x = 18; rb_y = 0;
    rb_pixel = img.pixelColor(rb_x, rb_y);
    QByteArray msg;

    for (uint32_t i=0; i<msg_in; i++) msg.push_back(MainWindow::readbyte()) ;
    ui->input->setPlainText( QString::fromUtf8(msg) );
    input_size=msg_in;

    QString res_msg = ((msg_in)? "♥ ":"");
    res_msg+= "Сообщение декодировано.";
    ui->info->setText(res_msg);
    ui->btn_encode->setEnabled(true);
    ui->btn_save->setEnabled(false);

    input_free = msg_max - input_size;
    ui->inp_size->setText(QString::number(input_size));
    ui->inp_free->setText(QString::number(input_free));
}

void MainWindow::msgChanged(){
    QByteArray txt = ui->input->toPlainText().toUtf8();
    input_size = txt.size();
    if (msg_max==M) {
        input_free = 0;
        ui->info->setText("Загрузите файл.");
        ui->inp_size->setText(QString::number(input_size));
        ui->inp_free->setText("0");
        ui->btn_encode->setEnabled(false);
        return;
    }

    if (input_size>msg_max){
        input_free = 0;
        QString out_text = "Размер текста превышен на: ";
        out_text += QString::number(input_size-msg_max);
        ui->info->setText(out_text);
        ui->inp_size->setText(QString::number(input_size));
        ui->inp_free->setText("0");
        ui->btn_encode->setEnabled(false);
        return;
    }

    input_free = msg_max - input_size;
    ui->info->setText("");
    ui->inp_size->setText(QString::number(input_size));
    ui->inp_free->setText(QString::number(input_free));
    ui->btn_encode->setEnabled(true);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btn_load,   &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->btn_save,   &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(ui->btn_encode, &QPushButton::clicked, this, &MainWindow::encode);
    connect(ui->btn_decode, &QPushButton::clicked, this, &MainWindow::decode);
    connect(ui->input, &QPlainTextEdit::textChanged, this, &MainWindow::msgChanged);
}

MainWindow::~MainWindow(){
    delete ui;
}
//ui->label_3->setText(QString("Version%1").arg(version));
