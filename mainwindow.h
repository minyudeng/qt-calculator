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
    void initUI();

private slots:
    void digitClicked();
    void operatorClicked();
    void equalClicked();
    void clearClicked();
    void pointClicked();
    void parenthesisClicked();
    void backspaceClicked();
    void percentClicked();
    double evaluateExpression(QString exp);

private:
    QLineEdit *display;
    QLineEdit *previewDisplay;
    QVector<QPushButton*> digitButtons;
    QPushButton *createButton(const QString &text, void (MainWindow::*member)());

    QString currentExpression;
};
#endif // MAINWINDOW_H
