#include "widget.h"
#include "ui_widget.h"
#include "mythread.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>

FindString *findStringThread = new FindString;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this); 
    connect(this,&Widget::fineshFile, findStringThread, &FindString::GetStringMsg );
    connect(findStringThread, &FindString::sendFindstate,this,&Widget::GetFindstate );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_openFileBt_clicked()
{
    QString filepath;
    if ( ui->checkBox->isChecked()){
        QString filter("源码(*.ewp);;所有(*.*)");
        filepath = QFileDialog::getOpenFileName(this, "打开文件","./",filter);
    }
    else {//未选中
        //qDebug()<<"未选中框";
        QString filter("所有(*.*);;源码(*.h *.c *.cpp)");
        filepath = QFileDialog::getExistingDirectory( this, "打开文件", "./", QFileDialog::ShowDirsOnly );
    }
    //显示路径
    ui->pathFileEdit->setPlaceholderText(filepath); //发送路经显示
    findStringThread->start();                      //开始运行另一个线程
    emit Widget::fineshFile(filepath);              //触发信号

}

//查找文件时，线程返回的路径
void Widget::GetFindstate(QString msg)
{
 ui->textBrowser->setPlaceholderText(msg); //发送路经显示
}

// 打开excel
void Widget::on_openExcelFileBk_clicked()
{
    QString filepath;

    QString filter("源码(*.ewp);;所有(*.*)");
    filepath = QFileDialog::getOpenFileName(this, "打开文件","./",filter);

    //显示路径
    ui->pathFileEdit->setPlaceholderText(filepath); //发送路经显示
    findStringThread->start();                      //开始运行另一个线程
    emit Widget::fineshExcel( filepath, 0 );              //触发信号
}
