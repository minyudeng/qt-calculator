#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void digitClicked();
    void operatorClicked();
    void equalClicked();
    void clearClicked();
    void clearAllClicked();
    void pointClicked();

private:
    QLineEdit *display;
    QVector<QPushButton*> digitButtons;
    QPushButton *createButton(const QString &text, const char *member);
    void calculate(double operand);

    double result;
    QString pendingOperator;
    bool waitingForOperand;
};
#endif // MAINWINDOW_H
