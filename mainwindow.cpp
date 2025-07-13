#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameForm(new Form)
{
    ui->setupUi(this);
    connect(ui->Start, &QPushButton::clicked, this, &MainWindow::startGame);
}

MainWindow::~MainWindow() {
    delete ui;
    delete gameForm;
}

void MainWindow::startGame() {
    QString player1 = ui->comboBox_1->currentText();
    QString player2 = ui->comboBox_2->currentText();

    gameForm->setPlayers(player1, player2);

    gameForm->resetGame();

    gameForm->show();

    this->hide();

    connect(gameForm, &Form::backToMainMenu, this, [this]() {
        this->show();
        gameForm->hide();
    });
}
