#ifndef MP3_H
#define MP3_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MP3; }
QT_END_NAMESPACE

class MP3 : public QMainWindow
{
    Q_OBJECT

public:
    MP3(QWidget *parent = nullptr);
    ~MP3();

private:
    Ui::MP3 *ui;
};
#endif // MP3_H
