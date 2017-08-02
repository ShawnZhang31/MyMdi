#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mdichild.h"

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

private:
    QAction *actionSeparator;       //间隔器
    MdiChild *activeMdiChild();     //活动窗口
};

#endif // MAINWINDOW_H
