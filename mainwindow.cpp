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
    ui->treeWidget->setColumnWidth(0, 270);
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
    search = ui->lineEdit->text();

    ui->treeWidget->clear();
    ui->statusBar->showMessage("Searching...");

    QIcon icon = QFileIconProvider().icon(QString("/"));
    root = new QTreeWidgetItem(ui->treeWidget);
    root->setText(0, "/");
    root->setIcon(0, icon);
    ui->treeWidget->addTopLevelItem(root);
    ui->treeWidget->expandItem(root);

    process = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(ReadStdoutOutput()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished()));
    process->start("/usr/bin/locate", QStringList() << search, QIODevice::ReadOnly);
}

void MainWindow::ReadStdoutOutput()
{
    qDebug() << "ReadStdoutOutput()";
    while (process->canReadLine())
    {
        QByteArray path = process->readLine(1024).trimmed();
        addPath(catPath(path));
    }
}

void MainWindow::finished()
{
    ui->statusBar->showMessage("Finish");
}

QString MainWindow::catPath(QString path)
{
    int i = path.lastIndexOf(search);
    i = path.indexOf('/', i);
    if (i != -1)
        path.truncate(i);
    return path;
}

void MainWindow::addPath(QString path)
{
    QTreeWidgetItem *item, *newparent, *parent = root;

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
        {
            parent = newparent;
        }
        else
        {
            QFileInfo info(currentpath);
            QIcon icon = QFileIconProvider().icon(info);
            QString type = QFileIconProvider().type(info);

            item = new QTreeWidgetItem(parent);

            item->setText(0, dir);
            item->setIcon(0, icon);
            item->setData(0, Qt::UserRole, currentpath);

            item->setText(1, type);

            item->setText(2, QString("%1 KB").arg( int((info.size() + 1023) / 1024) ));

            item->setText(3, info.lastModified().toString());

            ui->treeWidget->addTopLevelItem(item);
            ui->treeWidget->expandItem(item);
            parent = item;
        }
    }
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    QString path = item->data(0, Qt::UserRole).toString();
    QDesktopServices::openUrl(QUrl("file://" + path));
}
