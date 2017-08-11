#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mdichild.h"
class QMdiSubWindow;
class QSignalMapper;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void updateMenus(); //更新菜单
    void on_actionNew_triggered();  //新建文档
    void on_actionOpen_triggered(); //打开文档
    void updateWindowMenu();        //更新窗口菜单


    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionExit_triggered();

    void on_actionRedo_triggered();

    void on_actionUndo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionClose_triggered();

    void on_actionCloseAll_triggered();

    void on_actionTitle_triggered();

    void on_actionCascade_triggered();

    void on_actionNext_triggered();

    void on_actionPrevious_triggered();

    void on_actionAbout_triggered();

    void on_actionAboutQt_triggered();

protected:
    void closeEvent(QCloseEvent *event);    //关闭窗口是读取窗口设置信息

private:
    QAction *actionSeparator;       //间隔器
    MdiChild *activeMdiChild();     //活动窗口
    MdiChild *createMdiChild();     //创建子窗口
    QMdiSubWindow *findMdiChild(const QString &fileName);   //查找子窗口
    QSignalMapper *windowMapper;        //信号映射器
    void readSettings();            //读取窗口设置
    void writeSettings();           //写入窗口设置
};

#endif // MAINWINDOW_H
