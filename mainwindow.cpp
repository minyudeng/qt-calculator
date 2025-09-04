#include "mainwindow.h"
#include <QGridLayout>
#include <QDebug>
#include <QFont>
#include <QRegularExpression>
#include <QStack>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentExpression("0")
{
    initUI();
}

MainWindow::~MainWindow() {}

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
        buttonMap[Qt::Key_0 + i] = digitButtons[i];
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

    // 将按钮添加到按钮映射中
    buttonMap[Qt::Key_Plus] = plusButton;
    buttonMap[Qt::Key_Minus] = minusButton;
    buttonMap[Qt::Key_Asterisk] = multiplyButton;
    buttonMap[Qt::Key_Slash] = divideButton;
    buttonMap[Qt::Key_ParenLeft] = leftParenButton;
    buttonMap[Qt::Key_ParenRight] = rightParenButton;
    buttonMap[Qt::Key_Period] = pointButton;
    buttonMap[Qt::Key_Equal] = equalButton;
    buttonMap[Qt::Key_Return] = equalButton;
    buttonMap[Qt::Key_Percent] = percentButton;
    buttonMap[Qt::Key_Enter] = equalButton;    
    buttonMap[Qt::Key_Escape] = clearButton; 
    buttonMap[Qt::Key_Backspace] = backspaceButton; 

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
        else if (text == "+" || text == "-" || text == "×" || text == "÷" || text == "AC" || text == "⌫" || text == "%")
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
    // 如果表达式以操作符或左括号结尾
    if (currentExpression == "0")
    {
        currentExpression = "0.";
    }
    else if (!currentExpression.endsWith('.'))
    {
        currentExpression += ".";
    }

    previewDisplay->setText(currentExpression);
}

// 括号按钮点击处理
void MainWindow::parenthesisClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString parenthesis = clickedButton->text();

    if (currentExpression == "0")
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
    previewDisplay->setText(currentExpression);
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
    if (currentExpression.isEmpty() || currentExpression == "0" || display->text() == "Error")
    {
        return;
    }

    QChar lastChar = currentExpression.back();
    if (lastChar.isDigit() || lastChar == ')')
    {
        currentExpression += "%";
        previewDisplay->setText(currentExpression);
        equalClicked();
    }
}

// 等号按钮点击处理
void MainWindow::equalClicked()
{
    // 更新预览显示完整表达式
    qDebug() << "Current Expression:" << currentExpression;
    previewDisplay->setText(currentExpression);

    QString expr = currentExpression;
    expr.replace("×", "*").replace("÷", "/");
    if (expr.contains('%'))
    {
        expr.replace(QRegularExpression("(\\d+(?:\\.\\d+)?)%"), "(\\1/100)");
    }

    QString postfix = convert2Postfix(expr);
    double calcResult = calculatePostfix(postfix);
    qDebug() << convert2Postfix(expr);

    display->setText("= " + QString::number(calcResult));
}

int MainWindow::precedence(QChar op)
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

QString MainWindow::convert2Postfix(QString infix)
{
    QString postfix;
    QStack<QChar> stack;

    for (int i = 0; i < infix.length(); ++i)
    {
        QChar c = infix[i];

        if (c.isDigit() || c == '.')
        {
            while (i < infix.length() && (infix[i].isDigit() || infix[i] == '.'))
            {
                postfix += infix[i];
                ++i;
            }
            i--;
            postfix += ' ';
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            postfix += ' ';
            while (!stack.isEmpty() && precedence(stack.top()) >= precedence(c))
            {
                postfix += stack.pop();
                postfix += ' ';
            }
            stack.push(c);
        }
        else if (c == '(')
        {
            stack.push(c);
        }
        else if (c == ')')
        {
            while (!stack.isEmpty() && stack.top() != '(')
            {
                postfix += ' ';
                postfix += stack.pop();
            }
            stack.pop(); // 弹出左括号
        }
    }

    while (!stack.isEmpty())
    {
        postfix += ' ';
        postfix += stack.pop();
    }

    return postfix;
}

double MainWindow::calculatePostfix(QString postfix)
{
    QStack<double> stack;

    QStringList tokens = postfix.split(' ', Qt::SkipEmptyParts);
    for (const QString &token : tokens)
    {
        if (token[0].isDigit()) // 如果是数字
        {
            // 尝试将token转换为数字
            bool conversionOk;
            double number = token.toDouble(&conversionOk);
            if (conversionOk)
            {
                stack.push(number);
            }
            else
            {
                return 0;
            }
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/") // 如果是操作符
        {
            if (stack.size() < 2)
            {
                return 0;
            }
            double right = stack.pop();
            double left = stack.pop();
            switch (token[0].toLatin1())
            {
            case '+':
                stack.push(left + right);
                break;
            case '-':
                stack.push(left - right);
                break;
            case '*':
                stack.push(left * right);
                break;
            case '/':
                if (right == 0)
                {
                    return 0;
                }
                stack.push(left / right);
                break;
            }
        }
    }

    if (stack.size() != 1)
    {
        return 0;
        display->setText("Error");
    }

    return stack.pop();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    QString keyText = event->text();
     int key = event->key();
    qDebug() << "KeyText Released:" << keyText;
    qDebug() << "Key Released:" << key;
    if (buttonMap.contains(key)) {
        buttonMap[key]->animateClick();
    }
}