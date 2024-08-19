/*************************************************/
/******************文件说明************************/
/*************************************************/

/*************************************************/
/******************引用文件************************/
/*************************************************/
#include "mythread.h"
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include "widget.h"
#include "QDir"
#include "QFileInfo"
/*************************************************/
/*****************宏定义***************************/
/*************************************************/


/*************************************************/
/******************静态变量************************/
/*************************************************/

QList<QPair<QString,QString>> FolderPath;     //不包含根目录的文件夹位置  文件位置
QFile *pFile;//(filePath+"/"+"out.txt");      //存储的文件

/*************************************************/
/******************静态函数************************/
/*************************************************/
FindString::FindString(QObject *parent) : QThread(parent)
{
    FindString::startFlag = false;
}

//查找数据
static bool isDataEnable( QString str , int index )
{    
    if ( str.startsWith("//") || str.startsWith("#include") ){
        return 0;
    }
    //去掉sprintf
    if( str.contains("sprintf", Qt::CaseSensitive) )
    {
        return 0;
    }

    static int strStrartIndex = 0;
    static int strAnnotation  = 0;   // /**/
    static int strAnnotation_1  = 0; // \'
    static int strAnnotation_2  = 0; // " \" "
    bool delAscii = false;

    static QString strfind;
    QString temp;
    QTextStream out(pFile);

    for (int i = 0; i < str.size(); i ++ ){

        temp = str.at(i);
#if 1   //查找为 /**/ 或为 // 的情况
        if ( strAnnotation == 0 )
        {
            if ( temp == '/' )
            {
                strAnnotation = 1;
            }
        }
        else if ( strAnnotation == 1 )
        {

            if ( temp == '*' )
            {
                strAnnotation = 2;
            }
            else if (  temp == '/' )
            {
                strAnnotation = 0;
                break;//退出
            }
            else strAnnotation = 0;
        }
        else if ( strAnnotation == 2 )
        {
            if ( temp == '*' )
            {
                strAnnotation = 3;
            }
        }
        else if ( strAnnotation == 3 )
        {
            if ( temp == '/' )
            {
                strAnnotation = 0;
            }
            else  strAnnotation = 2;
        }
#endif
        if ( strAnnotation == 0 )
        {
            //查找为'"'的情况
            if ( strAnnotation_1 == 0 )
            {
                if ( str.at(i) == '\'')
                {
                    strAnnotation_1 = 1;
                }
            }
            else if ( strAnnotation_1 == 1)
            {
                if ( str.at(i) == '\'')
                {
                    strAnnotation_1 = 0;
                }
            }



            if ( strAnnotation_1 == 0 )
            {

                if ( str.at(i) == '"' && strAnnotation_2 == 0 ) //找到字符串标识
                {
                    if ( strStrartIndex == 0 ){

                        strStrartIndex = 1;
                        continue;
                    }
                    else if ( strStrartIndex == 1){
                        bool ok;
                        strfind.toDouble(&ok);
                        //qDebug()<<strfind;
                        if(  str!="℃"
                             /*&& str!="Kpa"
                             && str!="bar"
                             && str!="r/min"
                             && str!="%"
                             && str!="L"
                             && str!="L/H"
                             && str!="mA"
                             && str!="h"
                             && str!="v"*/
                             && ok==false
                             && delAscii
                             )
                        {
                            //查找对比
                            out.seek(0);
                            QString radeline = out.readLine();
                            bool sveFlag = true;
                            while ( !radeline.isNull() ){
                                //qDebug()<<radeline;
                                if ( radeline == strfind){
                                    sveFlag = false;
                                    break;
                                }
                                radeline = out.readLine(); //读下行
                            }
                            if ( sveFlag ){//存储strfind += temp ;
                                //out<<strfind+ QString::number(index,10) + "行         "+"\n";
                                out<<strfind+"\n";
                            }
                        }
                        strStrartIndex = 0;//结束
                        delAscii = false;
                        strfind.clear();
                    }
                }
                if ( strStrartIndex == 1 ){
                    if ( temp != '\\' ){//换行符不添加
                        strfind += temp;
                        strAnnotation_2 =  0;

                        if ( temp > 0xFF ){

                           delAscii = true;
                        }

                    }
                    else {
                        //为"\"
                       strAnnotation_2 = 1 ;
                    }
                }
            }
        }
    }
    return 1;
}

//path 路径
//out 存储地径
static void WriteData(QString path )
{

 QFile file(path);
 if ( file.open(QIODevice::ReadOnly) ) {
     int index = 1;
     QString line;//读一行的数据
     QTextStream in(&file);//行数据流
     line = in.readLine();

     while ( !line.isNull() ){ 
         //qDebug()<<line;
          //判断
          if ( isDataEnable(line,index) )
          {
            //查找，没有重复的才写入数据 
          }
          line = in.readLine(); //读下行
          ++index;
     }
     file.close();
 }
 else {
    //qDebug()<<path;
    //qDebug()<<"失败";
 }
}

static QStringList findFile(QString path)
{
    // 获取所有文件夹名
    QDir dir(path);
    QStringList fileters;
    fileters<<"*.cpp"<<"*.h"<<"*.c";
    path = dir.fromNativeSeparators(path);//  "\\"转为"/"
    QStringList allFile = QStringList("");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(fileters);
    allFile = dir.entryList();
    return  allFile;
}

//当查当前路径下的所有子目录
QStringList findFolder(QString folder )
{
    // 获取所有文件夹名
    QDir dir(folder);
    folder = dir.fromNativeSeparators(folder);//  "\\"转为"/"
    QStringList allFolder = QStringList("");
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    allFolder = dir.entryList();
    return  allFolder;
}

//递归查找当前路径下的所有文件
void findAllFiles(QString folder)
{
    //添加当前路径下所有的文件
    QStringList Files =findFile(folder);
    for(int i=0; i<Files.size(); i++)
    {
        FolderPath.append(QPair<QString,QString>(folder,Files.at(i)));
    }
    // 获取所有文件夹名
    QStringList Folder = findFolder(folder);        //根目录下的子文件夹
    //遍历子文件夹 获取里面的文件和文件夹
    for (int i = 0; i < Folder.size(); ++i)
    {
        findAllFiles(folder+"/"+Folder[i]);//路径合并
    }
}

static QString GetPathName( QString folder )
{
    QString partialPath = folder;
    //qDebug()<<partialPath;
    int i = 0;
    for (  i = partialPath.size() - 1 ; i > 0; i-- )
    {
        if ( partialPath.at(i) == '/' ){
            partialPath.chop(partialPath.size()-i);
            //qDebug()<<partialPath;
            return partialPath;
        }
    }
    qDebug()<<"失败1";
    return 0;
}

static void findAllFileWith_EWP( QString folder )
{
    QString partialPath = GetPathName(folder);
    if ( partialPath != 0 )
    {
        QFile file(folder);
        if ( file.open(QIODevice::ReadOnly) ) {
            QString line;//读一行的数据
            QTextStream in(&file);//行数据流
            line = in.readLine();

            while ( !line.isNull() ){
                line.remove(QRegExp("^ +\s*"));
                line.remove(QRegExp("\s* +$"));
                if ( line.startsWith(("<name>$PROJ_DIR$\\")) )
                {
                    //qDebug()<<line;
                    line.chop(7);     //"</name>"
                    line.remove(0,17);//"<name>$PROJ_DIR$\\"
                    //
                    if ( line.endsWith(".h") || line.endsWith(".c") ){
                    QDir dir(line);
                    line = dir.fromNativeSeparators(line);//  "\\"转为"/"
                    //qDebug()<<partialPath;

                    while( line.startsWith(("../")) )
                    {
                        line.remove(0,3);
                        partialPath = GetPathName(partialPath);
                        //qDebug()<<partialPath;
                        //qDebug()<<line;
                        //qDebug()<<++k;
                        //
                    }
                    FolderPath.append(QPair<QString,QString>(partialPath+"/"+line,"0"));
                    qDebug()<<partialPath+"/"+line;
                    }
                }
                line = in.readLine(); //读下行
                partialPath = GetPathName(folder);
            }
            //qDebug()<<"结束";
            file.close();
        }else qDebug()<<"失败";
    }
}
/*************************************************/
/***************函数*******************************/
/*************************************************/
//运行函数
void FindString::run()
{  
    int i=0;
    QString strPath;
    bool flag = true;

    if ( filePath.endsWith(".ewp")){
        QString tempStr = GetPathName(filePath);
        static QFile savefile_1( tempStr +"/"+"Aout.txt" );
        pFile = &savefile_1;
        flag = false;
    }
     else  {
        static QFile savefile(filePath+"/"+"Aout.txt");
        pFile = &savefile;
    }

    if ( pFile->open(QIODevice::ReadWrite) )
    {   //打开成功
        FolderPath.clear();//清除缓存

        if( !filePath.isEmpty() ){

            if ( filePath.endsWith(".ewp")){
                //qDebug()<<"成功";
                findAllFileWith_EWP(filePath);
            }else {
                findAllFiles(filePath);//文件查找结束
            }
        }

        for(; i < FolderPath.size(); i++)
        {

            //ui->textEdit->append(QString("文件名:%1\n文件路径:%2\n").arg(FolderPath.at(i).second).arg(FolderPath.at(i).first));
            //qDebug()<< QString("文件名:%1\n文件路径:%2\n").arg(FolderPath.at(i).second).arg(FolderPath.at(i).first)
            //qDebug()<< FolderPath.at(i).first +"/"+FolderPath.at(i).second;
            if ( flag )
            {
                strPath = FolderPath.at(i).first +"/"+FolderPath.at(i).second;
            }
            else {
                strPath = FolderPath.at(i).first;
            }
             //触发信号发送
            emit FindString::sendFindstate("进度："+QString::number(i*100/FolderPath.size(),10)+"%"+"     "+strPath);//触发信号
            //数据处理
            WriteData( strPath );

        }
        emit FindString::sendFindstate("进度："+QString::number(i*100/FolderPath.size(),10)+"%"+"     "+strPath);  //触发信号
        pFile->close();
    }//else qDebug()<<"失败";
}
/*************************************************/
/********************槽函数************************/
/*************************************************/
//获取地址
void FindString::GetStringMsg(QString msg)
{
    filePath = msg;
}

void FindString::GetExcelStringMsg( QString msg, int type )
{


}
