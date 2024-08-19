#include "widget.h"
#include <QApplication>
#include <QDebug>



void decodeUTF8( char *str )
{
    while( str )
    {
        int tmep = ( *str & 0xF8 )>>3;
        int i = 0;
        for (int i = 0; i < 5 ; i++ )
        {
            if ((tmep>>i)&0x01 == 0 ){

                str = str+(4-i);
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug()<<"123";

    //Widget w;
    //w.show();

    return a.exec();
}
