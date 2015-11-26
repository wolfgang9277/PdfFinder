#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->addButton->installEventFilter(this);
    ui->settingsButton->installEventFilter(this);
    ui->helpButton->installEventFilter(this);
    ui->exitButton->installEventFilter(this);

    QObject::connect(this,SIGNAL(mainButtonReleased(const QPushButton*)),this,SLOT(on_mainButtonReleased(const QPushButton*)));
    fillPaths();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QPushButton * actualButton = qobject_cast<QPushButton *>(obj);

    if (event->type() == QEvent::HoverEnter) {
        actualButton->setIconSize(QSize(52, 52));
        return true;
    }
    else if (event->type() == QEvent::HoverLeave) {
        actualButton->setIconSize(QSize(48, 48));
        return true;
    }

    else if (event->type() == QEvent::MouseButtonRelease) {
        actualButton->setIconSize(QSize(52, 52));
        emit mainButtonReleased(actualButton);
        return true;
    }

    else if (event->type() == QEvent::MouseButtonPress) {
        actualButton->setIconSize(QSize(30, 30));
        return true;
    }

    return false;
}

QString MainWindow::getPathFromFile(readType type)
{
    QString readPath{};
    QDir dir;
    QString projectPath = dir.absolutePath();
    QFile pathFile(projectPath+"/PATH.txt");

    if (!pathFile.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "Informacja", "Ścieżka nie odczytana poprawnie\n.ERROR: can't open PATH.txt");
            return QString(QDir::homePath());
        }

    QTextStream out(&pathFile);
    out >> readPath;

    if(!readPath.isEmpty() && type != readType::INIT) {
        QStringList pathList = readPath.split(";");
        if(type == readType::INPUT) readPath = pathList.at(0);
        else if(type == readType::OUTPUT) readPath = pathList.at(1);
    }

    pathFile.close();
    return readPath == "" ? QString(QDir::homePath()) : readPath;
}

void MainWindow::savePathToFile()
{
    QDir dir;
    QString projectPath = dir.absolutePath();
    QFile pathFile(projectPath+"/PATH.txt");

    if (!pathFile.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, "Informacja", "Ścieżka nie zapisana poprawnie\n. ERROR: can't open PATH.txt");
            return;
        }

    QTextStream in(&pathFile);
    in << ui->inputLineEdit->text() + ";" + ui->outputLineEdit->text();
    pathFile.close();
}

void MainWindow::fillPaths()
{
    QString path = getPathFromFile(INIT);

    if( path == QString(QDir::homePath())){
        ui->inputLineEdit->setText(path);
        ui->outputLineEdit->setText(path);
    }
    else {
        QStringList pathList = path.split(";");

        if( pathList.at(0).isEmpty()) ui->inputLineEdit->setText(QString(QDir::homePath()));
        else ui->inputLineEdit->setText(pathList.at(0));

        if( pathList.at(1).isEmpty()) ui->outputLineEdit->setText(QString(QDir::homePath()));
        else ui->outputLineEdit->setText(pathList.at(1));
    }
}

QString MainWindow::getInputPath()
{
    QString initPath = getPathFromFile(INPUT);
    QString path = QFileDialog::getExistingDirectory(this , tr("Wybierz folder"), initPath );
    return path;
}

QString MainWindow::getOutputPath()
{
    QString initPath = getPathFromFile(OUTPUT);
    QString path = QFileDialog::getExistingDirectory(this , tr("Wybierz folder"), initPath );
    return path;
}

QString MainWindow::getSchedulePath()
{
    QString schedulePath = QFileDialog::getOpenFileName(this, tr("Wybierz folder"), tr("//K1/Handlowy/DH/Realizacje"), tr("Pliki XLSX (*.xlsx)"));
    return schedulePath;
}

void MainWindow::on_inputButton_released()
{
    QString inputPath = getInputPath();
    if ( !inputPath.isEmpty() ) {
        ui->inputLineEdit->setText(inputPath);
        savePathToFile();
    }
}

void MainWindow::on_outputButton_released()
{
    QString outputPath = getOutputPath();
    if ( !outputPath.isEmpty() ) {
        ui->outputLineEdit->setText(outputPath);
        savePathToFile();
    }
}

void MainWindow::on_mainButtonReleased(const QPushButton *mainButton)
{
    if( mainButton == ui->addButton )
    {
        QString schedulePath = getSchedulePath();
        if ( !schedulePath.isEmpty() ) {
            QStringList pathList  = schedulePath.split("/");
            ui->scheduleLineEdit->setText(pathList.at(pathList.size()-1));
            ui->scheduleLineEdit->setCursorPosition(0);
        }
    }

    else if ( mainButton == ui->exitButton )
        QApplication::quit();

    else if ( mainButton == ui->helpButton )
        {
            Finder f(QString("C:\\Users\\BPokrzywa\\Desktop\\SR-357.00.00.00"));
            f.loadFileList(QString("C:\\Users\\BPokrzywa\\Desktop\\RZ-398_09_15 (Stoły rehabilitacyjne).xlsx"));
            f.findFiles();
        }

}


void MainWindow::on_searchButton_clicked()
{
    QStringList missingPaths;

    if( ui->inputLineEdit->text().isEmpty() || !ui->inputLineEdit->text().contains("/"))
        missingPaths << "ścieżka wyszukiwania";
    if( ui->outputLineEdit->text().isEmpty() || !ui->outputLineEdit->text().contains("/"))
        missingPaths << "ścieżka zapisu";
    if( ui->scheduleLineEdit->text().isEmpty() || !ui->scheduleLineEdit->text().contains("xlsx"))
        missingPaths << "ścieżka harmonogramu";

    if (!missingPaths.isEmpty())
        QMessageBox::information(this, tr("Informacja"), QString("Brakujące ścieżki: "+missingPaths.join(",")+"" + "."));
    else {
        /* Searching ... */
    }
}
