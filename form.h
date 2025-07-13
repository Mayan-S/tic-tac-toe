#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Form; }
QT_END_NAMESPACE

class Form : public QWidget {
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

    void setPlayers(const QString &p1, const QString &p2);
    void resetGame();

signals:
    void backToMainMenu();

private slots:
    void handleButtonClick();
    void handleComputerMove();

private:
    Ui::Form *ui;
    QVector<QPushButton*> buttons;
    QString player1Type;
    QString player2Type;
    bool xTurn;
    bool gameOver;
    QTimer *computerTimer;

    void updateStatus();
    void makeMove(int index, const QString &symbol);
    void checkGameState();
    QString checkWinner();
    void disableAllButtons();
    void enableEmptyButtons();
    void computerPlay();
    int bestMove(const QString &symbol);
    int minimax(QVector<QString> board, bool isMaximizing, const QString &aiSymbol, const QString &humanSymbol);
};

#endif // FORM_H
