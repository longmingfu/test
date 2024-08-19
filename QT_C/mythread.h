#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class FindString : public QThread
{
    Q_OBJECT
public:
     explicit FindString(QObject *parent = nullptr);
     //void enableOpenFile(bool flag);
     bool  startFlag;

public slots:

     void GetStringMsg(QString msg);
     void GetExcelStringMsg( QString msg, int type );

signals:
     void  sendFindstate(QString msg);

private:
     QString filePath;


protected:
    void run() override;

};

#endif // MYTHREAD_H
