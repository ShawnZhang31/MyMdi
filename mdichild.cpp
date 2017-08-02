#include "mdichild.h"
#include <QFile>
#include<QTextStream>
#include<QMessageBox>
#include<QFileInfo>
#include<QApplication>
#include<QFileDialog>
#include<QCloseEvent>
#include<QPushButton>

MdiChild::MdiChild(QWidget *parent) : QTextEdit (parent)
{
    //设置在子窗口关闭时销毁这个类的对象
    setAttribute (Qt::WA_DeleteOnClose);

    //初始化isUntitled为true
    isUntitled=true;
}

//新建文件
void MdiChild::newFile ()
{
    //设置窗口标号，因为编号一直被保存，所以需要使用静态变量
    static int sequenceNumber =1;

    //新建文档没有被保存过
    isUntitled=true;

    //将当前文件命名为未命名文档加编号，编号先使用再加1
    curFile=tr("未命名文档%1.txt").arg(sequenceNumber++);

    //设置窗口标题，使用[*]可以在文档被更改后再文件名称后显示“*”号
    setWindowTitle (curFile+"[*]"+tr("- 多文档编辑器"));

    //文档更改的时候发射contentsChanged（）信号，执行documentWasModified（）槽

    connect (document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
}

//文档被修改的槽
void MdiChild::documentWasModified ()
{
    //根据文档的isModified()函数的返回值，判断编辑器内容是否被更改了
    setWindowModified (document()->isModified ());
}

//加载文件操作
bool MdiChild::loadFile (const QString &fileName)
{
    //新建QFile对象
    QFile file(fileName);

    //只读方式打开文件，出错则提示，并返回false
    if(!file.open (QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning (this,tr("多文档编辑器"),tr("无法读取%1:\n%2.").arg(fileName).arg(file.errorString()));

        return false;
    }

    //新建文本流对象
    QTextStream in(&file);

    //设置鼠标状态为等待状态
    QApplication::setOverrideCursor (Qt::WaitCursor);

    //读取文件的全部文本内容，并添加到编辑器中
    setPlainText (in.readAll ());

    //回复鼠标装填
    QApplication::restoreOverrideCursor ();

    setCurrentFile (fileName);

    connect (document(),SIGNAL(connectsChanged()),this,SLOT(documentWasModified()));

    return true;

}

//设置当前文件操作
void MdiChild::setCurrentFile (const QString &fileName)
{
    //canonicalFilePath ()可以除去路径中的符号链接,"."和".."等符号
    curFile=QFileInfo(fileName).canonicalFilePath ();

    //文件已经被保存过了
    isUntitled=false;

    //文档没有被更改过
    document ()->setModified (false);

    //窗口不显示更改标志
    setWindowModified (false);

    //设置窗口标题，userFriendlyCurrentFile()返回文件名
    setWindowTitle (userFriendlyCurrentFile ()+"[*]");
}

//对用户友好的文件名
QString MdiChild::userFriendlyCurrentFile ()
{
    return QFileInfo(curFile).fileName ();
}

//文件保存操作
bool MdiChild::save ()
{
    //如果文件已经保存过，则执行另存为操作，否则直接保存文件
    if(isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile (curFile);
    }
}

//文件另存为操作
bool MdiChild::saveAs ()
{
    QString fileName=QFileDialog::getSaveFileName (this,tr("另存为"),curFile);
    //获取文件路径，如果为空，则返回false，否则保存文件
    if(fileName.isEmpty ())
        return false;
    return saveFile (fileName);
}

//文件写入操作
bool MdiChild::saveFile (const QString &fileName)
{
    QFile file(fileName);
    if(!file.open (QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning (this,tr("多文档编辑器"),tr("无法写入文件 %1:\n %2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor (Qt::WaitCursor);
    //以纯文本的文件写入
    out<<toPlainText ();
    QApplication::restoreOverrideCursor ();
    setCurrentFile (fileName);
    return true;
}

//关闭事件
void MdiChild::closeEvent(QCloseEvent *event)
{
    //如果maybeSave（）函数返回true，则关闭窗口，否则忽略该事件
    if(maybeSave ())
    {
        event->accept ();
    }
    else
    {
        event->ignore ();
    }
}

//检查文件是否已经保存
bool MdiChild::maybeSave ()
{
    //如果文档被更改过
    if(document ()->isModified ())
    {
        QMessageBox box;
        box.setWindowTitle (tr("多文档编辑器"));
        box.setText (tr("是否保存对“%1”的更改").arg(userFriendlyCurrentFile ()));
        box.setIcon (QMessageBox::Warning);

        QPushButton *yesBtn= box.addButton (tr("是"),QMessageBox::YesRole);
        box.addButton (tr("否"),QMessageBox::NoRole);
        QPushButton *cancleBtn=box.addButton (tr("取消"),QMessageBox::RejectRole);

        box.exec ();

        if(box.clickedButton ()==yesBtn)
            return save();
        else if(box.clickedButton ()==cancleBtn)
            return false;

    }

    return true;
}























