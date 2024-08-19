#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_openFileBt_clicked();
    void  GetFindstate(QString msg);
    void on_openExcelFileBk_clicked();

signals:
    void fineshFile(QString msg);
    void fineshExcel(QString msg,int type);



private:
    Ui::Widget *ui;
};


#endif // WIDGET_H
