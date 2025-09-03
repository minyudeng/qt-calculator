#include "mainwindow.h"
#include <QGridLayout>
#include <QDebug>
#include <QFont>
#include <QRegularExpression>
#include <QStack>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentExpression("0")
{
   initUI();
}

MainWindow::~MainWindow(){}

void MainWindow::initUI()
{
     // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建预览区域
    previewDisplay = new QLineEdit();
    previewDisplay->setReadOnly(true);
    previewDisplay->setAlignment(Qt::AlignRight);
    previewDisplay->setMaxLength(50);
    previewDisplay->setStyleSheet("QLineEdit { font-size: 16px; padding: 5px; background-color: #f8f8f8; color: #666; border: none; }");
    previewDisplay->setText("0");

    // 创建主显示区域
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);
    display->setStyleSheet("QLineEdit { font-size: 30px; padding: 10px; background-color: #f0f0f0; border: 2px solid #ccc; }");

    // 创建数字按钮 0-9
    for (int i = 0; i < 10; ++i)
    {
        digitButtons.append(createButton(QString::number(i), &MainWindow::digitClicked));
    }

    // 创建操作符按钮
    QPushButton *plusButton = createButton("+", &MainWindow::operatorClicked);
    QPushButton *minusButton = createButton("-", &MainWindow::operatorClicked);
    QPushButton *multiplyButton = createButton("×", &MainWindow::operatorClicked);
    QPushButton *divideButton = createButton("÷", &MainWindow::operatorClicked);
    QPushButton *equalButton = createButton("=", &MainWindow::equalClicked);
    QPushButton *clearButton = createButton("AC", &MainWindow::clearClicked);
    QPushButton *pointButton = createButton(".", &MainWindow::pointClicked);
    QPushButton *leftParenButton = createButton("(", &MainWindow::parenthesisClicked);
    QPushButton *rightParenButton = createButton(")", &MainWindow::parenthesisClicked);
    QPushButton *backspaceButton = createButton("⌫", &MainWindow::backspaceClicked);
    QPushButton *percentButton = createButton("%", &MainWindow::percentClicked);

    // 创建布局
    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(previewDisplay, 0, 0, 1, 5);
    mainLayout->addWidget(display, 1, 0, 1, 5);
    mainLayout->addWidget(clearButton, 2, 0);
    mainLayout->addWidget(backspaceButton, 2, 1);
    mainLayout->addWidget(percentButton, 2, 2);
    mainLayout->addWidget(divideButton, 2, 3);
    mainLayout->addWidget(multiplyButton, 2, 4);
    mainLayout->addWidget(plusButton, 3, 4);
    mainLayout->addWidget(minusButton, 3, 3);
    mainLayout->addWidget(equalButton, 5, 3, 1, 2);

    // 添加数字按钮
    for (int i = 1; i < 10; ++i)
    {
        int row = ((9 - i) / 3) + 3;
        int column = (i - 1) % 3;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 6, 1);
    mainLayout->addWidget(pointButton, 6, 0);
    mainLayout->addWidget(leftParenButton, 4, 3);
    mainLayout->addWidget(rightParenButton, 4, 4);

    // 设置按钮样式
    QString buttonStyle = "QPushButton { font-size: 18px; padding: 12px; background-color: #e0e0e0; border: 1px solid #ccc; border-radius: 5px; }"
                          "QPushButton:hover { background-color: #d0d0d0; }"
                          "QPushButton:pressed { background-color: #c0c0c0; }";

    QString operatorStyle = "QPushButton { font-size: 18px; background-color: #f0a050; }"
                            "QPushButton:hover { background-color: #e09040; }"
                            "QPushButton:pressed { background-color: #d08030; }";

    QString equalStyle = "QPushButton { font-size: 18px; background-color: #70a0e0; color: white; }"
                         "QPushButton:hover { background-color: #6090d0; }"
                         "QPushButton:pressed { background-color: #5080c0; }";

    for (QPushButton *button : centralWidget->findChildren<QPushButton *>())
    {
        QString text = button->text();
        if (text == "=")
        {
            button->setStyleSheet(equalStyle);
        }
        else if (text == "+" || text == "-" || text == "×" || text == "÷" 
            || text == "AC" || text == "⌫" || text == "%")
        {
            button->setStyleSheet(operatorStyle);
        }
        else
        {
            button->setStyleSheet(buttonStyle);
        }
        button->setMinimumSize(60, 60);
    }

    setWindowTitle(tr("DMY Calculator - Advanced"));
    resize(400, 500);
}

QPushButton *MainWindow::createButton(const QString &text, void (MainWindow::*member)())
{
    QPushButton *button = new QPushButton(text);
    connect(button, &QPushButton::clicked, this, member);
    return button;
}

// 数字按钮点击处理
void MainWindow::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString digitValue = clickedButton->text();

    if (currentExpression == "0")
    {
        // 如果当前表达式是初始状态，替换为新数字
        currentExpression = digitValue;
    }
    else if (currentExpression.endsWith('(') && digitValue == "0")
    {
        // 如果在括号后输入0，保持不变
    }
    else if (currentExpression.endsWith('(') && digitValue != "0")
    {
        // 如果在括号后输入非0数字，直接添加
        currentExpression += digitValue;
    }
    else if (display->text() == "Error")
    {
        // 如果之前计算出错，不管
    }
    else
    {
        currentExpression += digitValue;
    }
    equalClicked();
}

// 操作符按钮点击处理
void MainWindow::operatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();

    if (currentExpression.isEmpty())
    {
        return;
    }
    else if (currentExpression.size() > 1 && QRegularExpression("[-+×÷]$").match(currentExpression).hasMatch())
    {
        // 如果表达式以操作符结尾且有多个字符，替换为新的操作符
        currentExpression.replace(QRegularExpression("[-+×÷]$"), clickedOperator);
        previewDisplay->setText(currentExpression);
        return;
    }
    else if (currentExpression == "0" && clickedOperator == "-")
    {
        // 如果表达式只有一个0
        currentExpression = clickedOperator;
        previewDisplay->setText(currentExpression);
        return;
    }
    else
    {
        // 否则直接添加操作符
        currentExpression += clickedOperator;
    }

    previewDisplay->setText(currentExpression);
}

// 清除按钮点击处理
void MainWindow::clearClicked()
{
    currentExpression = "0";
    equalClicked();
}

// 小数点按钮点击处理
void MainWindow::pointClicked()
{

    display->setText("0");

    if (!display->text().contains('.'))
    {
        display->setText(display->text() + ".");
    }
}

// 括号按钮点击处理
void MainWindow::parenthesisClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString parenthesis = clickedButton->text();

    if(currentExpression == "0" && parenthesis == "(")
    {
        currentExpression = "(";
    }
    else if (parenthesis == "(")
    {
        currentExpression += parenthesis;
    }
    else if (parenthesis == ")")
    {
        currentExpression += parenthesis;
    }
    equalClicked();
}

// 退格按钮点击处理
void MainWindow::backspaceClicked()
{
    if (currentExpression != "0" && !currentExpression.isEmpty())
    {
        currentExpression.chop(1); // 删除最后一个字符
        if (currentExpression.isEmpty())
        {
            currentExpression = "0";
        }
    }
    equalClicked();
}

// 百分号按钮点击处理
void MainWindow::percentClicked()
{
    
}

// 等号按钮点击处理
void MainWindow::equalClicked()
{
    // 更新预览显示完整表达式
    qDebug() << "Current Expression:" << currentExpression;
    previewDisplay->setText(currentExpression);

    // 计算表达式
    QString expr = currentExpression;
    expr.replace("×", "*").replace("÷", "/");
    double calcResult = evaluateExpression(expr);

    display->setText("= " + QString::number(calcResult));
}

// 计算表达式
double MainWindow::evaluateExpression(QString expr)
{
    // 简单的从左到右计算
    QVector<double> numbers;
    QVector<QChar> operators;

    QString currentNumber;
    for (int i = 0; i < expr.length(); ++i)
    {
        QChar c = expr[i];

        if (c.isDigit() || c == '.')
        {
            currentNumber += c;
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            if (!currentNumber.isEmpty())
            {
                numbers.append(currentNumber.toDouble());
                currentNumber.clear();
            }
            operators.append(c);
        }
    }

    // 添加最后一个数字
    if (!currentNumber.isEmpty())
    {
        numbers.append(currentNumber.toDouble());
    }

    // 如果没有操作符，直接返回数字
    if (operators.isEmpty())
    {
        return numbers.value(0, 0);
    }

    // 从左到右计算
    double result = numbers[0];
    for (int i = 0; i < operators.size(); ++i)
    {
        double nextNumber = numbers.value(i + 1, 0);

        switch (operators[i].toLatin1())
        {
        case '+':
            result += nextNumber;
            break;
        case '-':
            result -= nextNumber;
            break;
        case '*':
            result *= nextNumber;
            break;
        case '/':
            if (nextNumber == 0.0)
            {
                display->setText("Error");
                return 0.0;
            }
            result /= nextNumber;
            break;
        }
    }

    return result;
}