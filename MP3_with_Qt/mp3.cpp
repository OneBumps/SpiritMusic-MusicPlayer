#include "mp3.h"
#include "ui_mp3.h"

MP3::MP3(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MP3)
{
    ui->setupUi(this);
}

MP3::~MP3()
{
    delete ui;
}

