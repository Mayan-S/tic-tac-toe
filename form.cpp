#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
    , xTurn(true)
    , gameOver(false)
    , computerTimer(new QTimer(this))
{
    ui->setupUi(this);

    buttons = {
        ui->pushButton, ui->pushButton_2, ui->pushButton_3,
        ui->pushButton_4, ui->pushButton_5, ui->pushButton_6,
        ui->pushButton_7, ui->pushButton_8, ui->pushButton_9
    };

    for (QPushButton *btn : buttons) {
        connect(btn, &QPushButton::clicked, this, &Form::handleButtonClick);
    }

    connect(ui->restartButton, &QPushButton::clicked, this, &Form::resetGame);

    connect(ui->quitButton, &QPushButton::clicked, this, &Form::backToMainMenu);

    connect(computerTimer, &QTimer::timeout, this, &Form::handleComputerMove);
}

Form::~Form() {
    delete ui;
}

void Form::setPlayers(const QString &p1, const QString &p2) {
    player1Type = p1;
    player2Type = p2;
}

void Form::resetGame() {
    for (QPushButton *btn : buttons) {
        btn->setText("");
        btn->setEnabled(true);
    }

    xTurn = true;
    gameOver = false;
    updateStatus();

    if ((player1Type == "Computer" && xTurn) || (player2Type == "Computer" && !xTurn)) {
        disableAllButtons();
        computerTimer->start(1000);
    }
    else {
        enableEmptyButtons();
    }
}

void Form::updateStatus() {
    if (gameOver){
        return;
    }

    if (xTurn){
        ui->Status->setText("X's Turn!");
    }
    else{
        ui->Status->setText("O's Turn!");
    }
}

void Form::handleButtonClick() {
    if (gameOver){
        return;
    }

    QPushButton *clicked = qobject_cast<QPushButton*>(sender());
    if (!clicked || !clicked->text().isEmpty()){
        return;
    }

    QString symbol = xTurn ? "X" : "O";
    clicked->setText(symbol);
    clicked->setEnabled(false);

    checkGameState();

    if (!gameOver) {
        xTurn = !xTurn;
        updateStatus();

        if ((xTurn && player1Type == "Computer") || (!xTurn && player2Type == "Computer")) {
            disableAllButtons();
            computerTimer->start(1000);
        }
    }
}

void Form::handleComputerMove() {
    computerTimer->stop();

    if (gameOver) return;

    computerPlay();
}

void Form::computerPlay() {
    QString symbol = xTurn ? "X" : "O";
    int move = bestMove(symbol);
    if (move != -1) {
        makeMove(move, symbol);
        checkGameState();

        if (!gameOver) {
            xTurn = !xTurn;
            updateStatus();

            if ((xTurn && player1Type == "Computer") || (!xTurn && player2Type == "Computer")) {
                disableAllButtons();
                computerTimer->start(1000);
            }
            else {
                enableEmptyButtons();
            }
        }
    }
}

void Form::makeMove(int index, const QString &symbol) {
    buttons[index]->setText(symbol);
    buttons[index]->setEnabled(false);
}

void Form::checkGameState() {
    QString winner = checkWinner();

    if (!winner.isEmpty()) {
        ui->Status->setText(winner + " Wins!");
        gameOver = true;
        disableAllButtons();
    }
    else {
        bool draw = true;
        for (QPushButton *btn : buttons) {
            if (btn->text().isEmpty()) {
                draw = false;
                break;
            }
        }
        if (draw) {
            ui->Status->setText("Draw!");
            gameOver = true;
            disableAllButtons();
        }
    }
}

QString Form::checkWinner() {
    const int winPatterns[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };

    for (auto &pattern : winPatterns) {
        QString a = buttons[pattern[0]]->text();
        QString b = buttons[pattern[1]]->text();
        QString c = buttons[pattern[2]]->text();
        if (!a.isEmpty() && a == b && b == c){
            return a;
        }
    }

    return "";
}

void Form::disableAllButtons() {
    for (QPushButton *btn : buttons) {
        btn->setEnabled(false);
    }
}

void Form::enableEmptyButtons() {
    for (QPushButton *btn : buttons) {
        if (btn->text().isEmpty()) {
            btn->setEnabled(true);
        }
        else {
            btn->setEnabled(false);
        }
    }
}

int Form::bestMove(const QString &symbol) {
    QVector<QString> board;

    for (QPushButton *btn : buttons){
        board.append(btn->text());
    }

    int bestScore = -1000;
    int move = -1;

    for (int i = 0; i < 9; ++i) {
        if (board[i].isEmpty()) {
            board[i] = symbol;
            int score = minimax(board, false, symbol, symbol == "X" ? "O" : "X");
            board[i] = "";
            if (score > bestScore) {
                bestScore = score;
                move = i;
            }
        }
    }

    return move;
}

int Form::minimax(QVector<QString> board, bool isMaximizing, const QString &aiSymbol, const QString &humanSymbol) {
    auto winner = [board]() -> QString {
        const int winPatterns[8][3] = {
            {0,1,2}, {3,4,5}, {6,7,8},
            {0,3,6}, {1,4,7}, {2,5,8},
            {0,4,8}, {2,4,6}
        };
        for (auto &pattern : winPatterns) {
            QString a = board[pattern[0]];
            QString b = board[pattern[1]];
            QString c = board[pattern[2]];
            if (!a.isEmpty() && a == b && b == c){
                return a;
            }
        }
        return "";
    };

    QString result = winner();
    if (result == aiSymbol){
        return 10;
    }
    if (result == humanSymbol){
        return -10;
    }

    bool draw = true;
    for (const QString &cell : board) {
        if (cell.isEmpty()) {
            draw = false;
            break;
        }
    }
    if (draw){
        return 0;
    }

    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < 9; ++i) {
            if (board[i].isEmpty()) {
                board[i] = aiSymbol;
                int score = minimax(board, false, aiSymbol, humanSymbol);
                board[i] = "";
                bestScore = std::max(score, bestScore);
            }
        }
        return bestScore;
    }
    else {
        int bestScore = 1000;
        for (int i = 0; i < 9; ++i) {
            if (board[i].isEmpty()) {
                board[i] = humanSymbol;
                int score = minimax(board, true, aiSymbol, humanSymbol);
                board[i] = "";
                bestScore = std::min(score, bestScore);
            }
        }
        return bestScore;
    }
}
