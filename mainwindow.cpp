#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QFileIconProvider>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setFocus();
    ui->treeWidget->setColumnWidth(0, 400);
    process = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (process != NULL && process->state() == QProcess::Running)
    {
        process->terminate();
        return;
    }

    search = ui->lineEdit->text();

    ui->treeWidget->clear();
    ui->lineEdit->setDisabled(true);
    ui->pushButton->setText("Stop");
    ui->statusBar->showMessage("Searching...");

    timer.start();

    root = addItem(NULL, QString("/"), QString("/"), false);

    QStringList arguments;

    if (ui->checkBox_basename->isChecked())
        arguments << "--basename";
    if (ui->checkBox_ignorecase->isChecked())
        arguments << "--ignore-case";
    if (ui->checkBox_regex->isChecked())
        arguments << "--regex";
    if (ui->checkBox_limit->isChecked())
    {
        arguments << "--limit";
        arguments << ui->lineEdit_limit->text();
    }

    arguments << "--quiet";
    arguments << search;

    process = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(ReadStdoutOutput()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished()));
    process->start("/usr/bin/locate", arguments, QIODevice::ReadOnly);
}

void MainWindow::ReadStdoutOutput()
{
    while (process->canReadLine())
    {
        QByteArray path = process->readLine(1024).trimmed();
        addPath(path);
        QCoreApplication::processEvents();
    }

    if (process->state() == QProcess::NotRunning)
    {
        ui->statusBar->showMessage(QString("Finish build tree: %1 ms").arg(timer.elapsed()));
    }
}

void MainWindow::finished()
{
    ui->lineEdit->setDisabled(false);
    ui->pushButton->setText("Search");
    ui->statusBar->showMessage(QString("Finish search: %1 ms").arg(timer.elapsed()));
}

void MainWindow::addPath(QString path)
{
    QTreeWidgetItem *newparent, *parent = root;

    QString currentpath;

    foreach (QString dir, path.split('/'))
    {
        if (dir.length() == 0)
            continue;

        currentpath += "/" + dir;

        newparent = NULL;
        for (int i=0; i < parent->childCount(); i++)
        {
            if (QString::compare(parent->child(i)->text(0), dir, Qt::CaseSensitive) == 0)
            {
                newparent = parent->child(i);
                break;
            }
        }

        if (newparent != NULL)
            parent = newparent;
        else
            parent = addItem(parent, currentpath, dir,
                             (dir.indexOf(search) != -1)
                             );
    }
}

QTreeWidgetItem *MainWindow::addItem(QTreeWidgetItem *parent, QString path, QString name, bool mark)
{
    QFileInfo info(path);

    QIcon icon = QFileIconProvider().icon(info);
    QString type = QFileIconProvider().type(info);

    QTreeWidgetItem *item = new QTreeWidgetItem(parent);

    item->setText(0, name);
    item->setIcon(0, icon);
    item->setData(0, Qt::UserRole, path);
    if (mark)
    {
        QFont font;
        font.setBold(true);
        item->setFont(0, font);
        item->setTextColor(0, QColor(0, 128, 0));
    }

    item->setText(1, type);

    item->setText(2, QString("%1 KB").arg( int((info.size() + 1023) / 1024) ));

    item->setText(3, info.lastModified().toString("ddd MMM d, hh:mm"));

    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->expandItem(item);

    return item;
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    QString path = item->data(0, Qt::UserRole).toString();
    QDesktopServices::openUrl(QUrl("file://" + path));
}
