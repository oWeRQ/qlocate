#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    QString catPath(QString path, QString search);
    void addPath(QString path);

private:
    Ui::MainWindow *ui;
    QTreeWidgetItem *root;

private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
