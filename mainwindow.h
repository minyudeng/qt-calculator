#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include <QString>

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
    void parenthesisClicked();
    void backspaceClicked();
    void updatePreview(const QString &newInput);
    double evaluateExpression(const QString &expression);

private:
    QLineEdit *display;
    QLineEdit *previewDisplay;
    QVector<QPushButton*> digitButtons;
    QPushButton *createButton(const QString &text, void (MainWindow::*member)());
    // void calculate(double operand);

    double result;
    QString pendingOperator;
    bool waitingForOperand;
    QString currentExpression;
};
#endif // MAINWINDOW_H
