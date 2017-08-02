#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建间隔器动作，并在其中设置间隔器
    actionSeparator=new QAction(this);
    actionSeparator->setSeparator (true);

    //更新菜单
    updateMenus ();

    //当有活动窗口的时候更新菜单
    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow *)),this,SLOT(updateMenus()));

}

//更新菜单
void MainWindow::updateMenus ()
{
    //根据是否有活动窗口来设置各个动作是否可用
    bool hasMdiChild=(activeMdiChild ()!=0);

    ui->actionSave->setEnabled (hasMdiChild);
    ui->actionSaveAs->setEnabled (hasMdiChild);
}

MainWindow::~MainWindow()
{
    delete ui;
}
