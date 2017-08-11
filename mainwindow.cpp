#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMdiSubWindow>
#include<QFileDialog>
#include<QSignalMapper>
#include<QCloseEvent>

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
    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateMenus()));
    //更新窗口菜单,创建信号映射器
    windowMapper=new QSignalMapper(this);
    //映射器重新发射信号，根据信号设置活动窗口
    connect (windowMapper,SIGNAL(mapped(QWidget*)),ui->mdiArea,SLOT(setActiveSubWindow(QWidget*)));
    //更新窗口菜单，并且设置当窗口菜单将要显示的时候更新窗口菜单
    updateWindowMenu ();
    connect (ui->menu_W,SIGNAL(aboutToShow ()),this,SLOT(updateWindowMenu()));


}

//更新菜单
void MainWindow::updateMenus ()
{
    //根据是否有活动窗口来设置各个动作是否可用
    bool hasMdiChild=(activeMdiChild ()!=0);

    ui->actionSave->setEnabled (hasMdiChild);   //有，保存按钮启用
    ui->actionSaveAs->setEnabled (hasMdiChild); //有，另存为按钮启用
    ui->actionPaste->setEnabled (hasMdiChild);  //有，粘贴按钮启用
    ui->actionClose->setEnabled (hasMdiChild);  //有，关闭按钮启用
    ui->actionCloseAll->setEnabled (hasMdiChild);   //有，关闭所有按钮启用
    ui->actionTitle->setEnabled (hasMdiChild);      //有，平铺按钮启用
    ui->actionCascade->setEnabled (hasMdiChild);    //有，层叠按钮启用
    ui->actionNext->setEnabled (hasMdiChild);       //有，下一个窗口按钮启用
    ui->actionPrevious->setEnabled (hasMdiChild);   //有，前一个窗口按钮启用

    //设置间隔器是否显示
    actionSeparator->setVisible (hasMdiChild);

    //有活动的窗口，且文字被选中，复制和剪切按钮才能启用
    bool hasSelection=(hasMdiChild && activeMdiChild ()->textCursor ().hasSelection ());

    ui->actionCopy->setEnabled (hasSelection);
    ui->actionCut->setEnabled (hasSelection);

    //有活动窗口，且窗口有撤销操作的时候，恢复操作按钮才可用
    ui->actionUndo->setEnabled (activeMdiChild () && activeMdiChild ()->document ()->isUndoAvailable ());
    ui->actionRedo->setEnabled (activeMdiChild() && activeMdiChild()->document ()->isRedoAvailable ());


}

MainWindow::~MainWindow()
{
    delete ui;
}

//获取活动的窗口
MdiChild* MainWindow::activeMdiChild ()
{
    if(QMdiSubWindow *subWindow=ui->mdiArea->activeSubWindow ())
        return qobject_cast<MdiChild*>(subWindow->widget ());
    return 0;
}

//创建子窗口
MdiChild* MainWindow::createMdiChild ()
{
    MdiChild *child= new MdiChild();

    ui->mdiArea->addSubWindow (child);

    connect (child,SIGNAL(copyAvailable (bool)),ui->actionCopy,SLOT(setEnabled (bool)));
    connect (child,SIGNAL(copyAvailable (bool)),ui->actionCut,SLOT(setEnabled (bool)));
    connect (child->document (),SIGNAL(undoAvailable (bool)),ui->actionUndo,SLOT(setEnabled (bool)));
    connect (child->document (),SIGNAL(redoAvailable (bool)),ui->actionRedo,SLOT(setEnabled (bool)));

    return child;
}

//查找子窗口
QMdiSubWindow* MainWindow::findMdiChild (const QString &fileName)
{
    //去除路径中的特殊符号
    QString canonicalFilePath=QFileInfo(fileName).canonicalFilePath ();

    //遍历mdiAera下面的所有的子窗口，如果其文件路径和窗口的路径相同，则返回该窗口
    foreach(QMdiSubWindow *window,ui->mdiArea->subWindowList ())
    {
        MdiChild *child=qobject_cast<MdiChild*>(window->widget ());
        if(child->currentFile ()==canonicalFilePath)
            return  window;
    }

    return 0;
}

//创建新的文档
void MainWindow::on_actionNew_triggered()
{
    //创建子文档
    MdiChild *child=createMdiChild ();

    //创建新的文件
    child->newFile ();
    child->show ();
}

//打开文档
void MainWindow::on_actionOpen_triggered()
{
    //打开文件
    QString fileName=QFileDialog::getOpenFileName (this);

    if(!fileName.isEmpty ())
    {
        QMdiSubWindow *execting=findMdiChild (fileName);
        if(execting)
        {
            ui->mdiArea->setActiveSubWindow (execting);
            return;
        }
        else
        {
            MdiChild *child=createMdiChild ();
            if(child->loadFile (fileName))
            {
                ui->statusBar->showMessage (tr("文件加载成功！"),2000);
                child->show ();
            }
            else
            {
                child->close ();
            }
        }
    }
}

/*****************************
 * SLOT：更新窗口菜单
 *****************************/
void MainWindow::updateWindowMenu ()
{
    //先清空菜单，然后添加各个菜单动作
    ui->menu_W->clear ();
    ui->menu_W->addAction (ui->actionClose);
    ui->menu_W->addAction (ui->actionCloseAll);
    ui->menu_W->addSeparator ();
    ui->menu_W->addAction (ui->actionTitle);
    ui->menu_W->addAction (ui->actionCascade);
    ui->menu_W->addSeparator ();
    ui->menu_W->addAction (ui->actionNext);
    ui->menu_W->addAction (ui->actionPrevious);
    ui->menu_W->addAction (actionSeparator);

    //如果有活动窗口则显示间隔器
    QList<QMdiSubWindow*> windows=ui->mdiArea->subWindowList ();
    actionSeparator->setVisible (!windows.isEmpty ());

    //遍历各个子窗口
    for(int i=0;i<windows.size ();i++)
    {
        MdiChild *child=qobject_cast<MdiChild*>(windows.at (i)->widget ());
        QString text;
        //如果窗口数小于9个，则设置编号为快捷键
        if(i<9)
        {
            text=tr("&%1 %2").arg (i+1).arg (child->userFriendlyCurrentFile ());
        }
        else
        {
            text=tr("%1 %2").arg (i+1).arg (child->userFriendlyCurrentFile ());
        }

        //添加动作到菜单，设置动作可以选择
        QAction *action=ui->menu_W->addAction (text);
        action->setCheckable (true);

        //设置当前活动窗口动作为选中状态
        action->setChecked (child==activeMdiChild ());
        //关联动作的触发信号到信号映射器的map()槽，这个槽会发射mapped（）型号
        connect (action,SIGNAL(triggered()),windowMapper,SLOT(map()));
        //将动作与相应的窗口部件进行映射
        //在发射mapped（）信号时就会以这个窗口部件为参数
        windowMapper->setMapping (action,windows.at (i));
    }
}

/*****************************
 * SLOT：保存操作
 *****************************/
void MainWindow::on_actionSave_triggered()
{
    if(activeMdiChild ()&&activeMdiChild ()->save ())
        ui->statusBar->showMessage (tr("文件保存成功"),2000);
}

/*****************************
 * SLOT：另存为操作
 *****************************/
void MainWindow::on_actionSaveAs_triggered()
{
    if(activeMdiChild ()&&activeMdiChild ()->saveAs ())
        ui->statusBar->showMessage (tr("文件另存为成功"),2000);
}

/*****************************
 * SLOT：退出操作
 *****************************/
void MainWindow::on_actionExit_triggered()
{

    qApp->closeAllWindows ();
}

/*****************************
 * SLOT：撤销操作
 *****************************/
void MainWindow::on_actionUndo_triggered()
{

    if(activeMdiChild ())
        activeMdiChild ()->undo ();

}

/*****************************
 * SLOT：恢复操作
 *****************************/
void MainWindow::on_actionRedo_triggered()
{
    if(activeMdiChild ())
        activeMdiChild ()->redo ();
}

/*****************************
 * SLOT：剪切操作
 *****************************/
void MainWindow::on_actionCut_triggered()
{
    if(activeMdiChild ())
        activeMdiChild ()->cut ();
}

/*****************************
 * SLOT：复制操作
 *****************************/
void MainWindow::on_actionCopy_triggered()
{
//TODO:SLOT：复制操作
}

/*****************************
 * SLOT：粘贴操作
 *****************************/
void MainWindow::on_actionPaste_triggered()
{
//TODO:SLOT：粘贴操作
}

/*****************************
 * SLOT：关闭操作
 *****************************/
void MainWindow::on_actionClose_triggered()
{
//TODO:SLOT：关闭操作
}

/*****************************
 * SLOT：关闭所有窗口操作
 *****************************/
void MainWindow::on_actionCloseAll_triggered()
{
//TODO:SLOT：关闭所有窗口操作
}

/*****************************
 * SLOT：窗口平铺操作
 *****************************/
void MainWindow::on_actionTitle_triggered()
{
//TODO:SLOT：窗口平铺操作
}

/*****************************
 * SLOT：窗口层叠操作
 *****************************/
void MainWindow::on_actionCascade_triggered()
{
//TODO:SLOT：窗口层叠操作
}

/*****************************
 * SLOT：下一个窗口操作
 *****************************/
void MainWindow::on_actionNext_triggered()
{
//TODO:SLOT：下一个窗口操作
}

/*****************************
 * SLOT：上一个窗口操作
 *****************************/
void MainWindow::on_actionPrevious_triggered()
{
//TODO:SLOT：上一个窗口操作
}

/*****************************
 * SLOT：关于操作操作
 *****************************/
void MainWindow::on_actionAbout_triggered()
{
//TODO:SLOT：关于操作操作
}

/*****************************
 * SLOT：关于QT操作
 *****************************/
void MainWindow::on_actionAboutQt_triggered()
{
//TODO:SLOT：关于QT操作
}

void MainWindow::readSettings ()
{

}

void MainWindow::closeEvent (QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows ();
    if(ui->mdiArea->currentSubWindow ())
    {
        event->ignore();
    }
    else
    {
        writeSettings();
        event->accept ();
    }

}
