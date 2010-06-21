#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QProcess>
#include <QTime>

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
    QString catPath(QString path);
    void addPath(QString path);
    QTreeWidgetItem *addItem(QTreeWidgetItem *parent, QString path, QString name, bool mark);
    void loadInfo();

private:
    Ui::MainWindow *ui;
    QTreeWidgetItem *root;
    QProcess *process;
    QString search;
    QTime timer;
    int reader_count;

private slots:
    void on_treeWidget_customContextMenuRequested(QPoint pos);
    void on_treeWidget_itemActivated(QTreeWidgetItem* item, int column);
    void on_pushButton_clicked();
    void ReadStdoutOutput();
    void finished();
    void itemOpen();
    void itemTerm();
    void itemCopyPath();
};

#endif // MAINWINDOW_H
