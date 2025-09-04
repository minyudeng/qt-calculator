#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include <QString>
#include <QHash>
#include <QKeyEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUI();

protected:
    // void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void digitClicked();
    void operatorClicked();
    void equalClicked();
    void clearClicked();
    void pointClicked();
    void parenthesisClicked();
    void backspaceClicked();
    void percentClicked();

private:
    QString convert2Postfix(QString infix);
    int precedence(QChar op);
    double calculatePostfix(QString postfix);
    QString currentExpression;
    QHash<int, QPushButton*> buttonMap;

    QLineEdit *display;
    QLineEdit *previewDisplay;
    QVector<QPushButton*> digitButtons;
    QPushButton *createButton(const QString &text, void (MainWindow::*member)());
};
#endif // MAINWINDOW_H
