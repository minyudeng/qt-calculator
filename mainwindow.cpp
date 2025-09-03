#include "mainwindow.h"
#include <QGridLayout>
#include <QDebug>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , result(0.0)
    , waitingForOperand(true)
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
    for (int i = 0; i < 10; ++i) {
        digitButtons.append(createButton(QString::number(i), &MainWindow::digitClicked));
    }

    // 创建操作符按钮
    QPushButton *plusButton = createButton("+", &MainWindow::operatorClicked);
    QPushButton *minusButton = createButton("-", &MainWindow::operatorClicked);
    QPushButton *multiplyButton = createButton("×", &MainWindow::operatorClicked);
    QPushButton *divideButton = createButton("÷", &MainWindow::operatorClicked);
    QPushButton *equalButton = createButton("=", &MainWindow::equalClicked);
    QPushButton *clearButton = createButton("C", &MainWindow::clearClicked);
    QPushButton *clearAllButton = createButton("AC", &MainWindow::clearAllClicked);
    QPushButton *pointButton = createButton(".", &MainWindow::pointClicked);
    QPushButton *leftParenButton = createButton("(", &MainWindow::parenthesisClicked);
    QPushButton *rightParenButton = createButton(")", &MainWindow::parenthesisClicked);
    QPushButton *backspaceButton = createButton("⌫", &MainWindow::backspaceClicked);

    // 创建布局
    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(previewDisplay, 0, 0, 1, 5);
    mainLayout->addWidget(display, 1, 0, 1, 5);
    
    mainLayout->addWidget(clearAllButton, 2, 0);
    mainLayout->addWidget(clearButton, 2, 1);
    mainLayout->addWidget(backspaceButton, 2, 2);
    mainLayout->addWidget(divideButton, 2, 3);
    mainLayout->addWidget(multiplyButton, 2, 4);
    mainLayout->addWidget(plusButton, 3, 4);
    mainLayout->addWidget(minusButton, 3, 3);
    mainLayout->addWidget(equalButton, 5, 3, 1, 2);

    // 添加数字按钮
    for (int i = 1; i < 10; ++i) {
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
                          
    QString operatorStyle = "QPushButton { background-color: #f0a050; }"
                            "QPushButton:hover { background-color: #e09040; }"
                            "QPushButton:pressed { background-color: #d08030; }";
                            
    QString equalStyle = "QPushButton { background-color: #70a0e0; color: white; }"
                         "QPushButton:hover { background-color: #6090d0; }"
                         "QPushButton:pressed { background-color: #5080c0; }";
    
    for (QPushButton* button : centralWidget->findChildren<QPushButton*>()) {
        QString text = button->text();
        if (text == "=") {
            button->setStyleSheet(equalStyle);
        } else if (text == "+" || text == "-" || text == "×" || text == "÷") {
            button->setStyleSheet(operatorStyle);
        } else {
            button->setStyleSheet(buttonStyle);
        }
        button->setMinimumSize(60, 60);
    }

    setWindowTitle(tr("DMY Calculator - Advanced"));
    resize(400, 500);
    
    // 初始化表达式
    currentExpression = "0";
}

MainWindow::~MainWindow() {

}

QPushButton *MainWindow::createButton(const QString &text, void (MainWindow::*member)()) {
    QPushButton *button = new QPushButton(text);
    connect(button, &QPushButton::clicked, this, member);
    return button;
}

void MainWindow::digitClicked() {
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString digitValue = clickedButton->text();
    
    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
        
        // 如果当前表达式以操作符结尾，直接添加数字
        if (currentExpression.endsWith("+") || currentExpression.endsWith("-") || 
            currentExpression.endsWith("×") || currentExpression.endsWith("÷")) {
            // 保持当前表达式不变
        } else {
            // 否则开始新的表达式
            currentExpression = "";
        }
    }
    
    // 如果当前显示为"0"且输入的不是0，则替换0
    if (display->text() == "0" && digitValue != "0") {
        display->setText(digitValue);
    } else {
        display->setText(display->text() + digitValue);
    }
    
    // 更新预览
    updatePreview(digitValue);
}

void MainWindow::operatorClicked(){
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();
    
    // 获取当前显示的数字
    QString currentValue = display->text();
    
    // 如果当前没有等待操作数，说明用户刚刚输入了一个数字
    if (!waitingForOperand) {
        // 将当前数字添加到表达式
        currentExpression = currentValue;
    }
    
    // 添加操作符到表达式
    currentExpression += clickedOperator;
    
    // 更新预览
    previewDisplay->setText(currentExpression);
    
    // 设置为等待操作数状态
    waitingForOperand = true;
}

void MainWindow::equalClicked(){
    // 更新预览显示完整表达式
    previewDisplay->setText(currentExpression + " =");
    
    // 计算表达式
    double calcResult = evaluateExpression(currentExpression);
    
    // 显示结果
    display->setText(QString::number(calcResult));
    
    // 重置表达式为结果，以便继续计算
    currentExpression = QString::number(calcResult);
    
    // 设置为等待操作数状态
    waitingForOperand = true;
}

void MainWindow::clearClicked(){
    // 清除当前输入
    display->setText("0");
    waitingForOperand = true;
}

void MainWindow::clearAllClicked(){
    // 清除所有
    display->setText("0");
    previewDisplay->setText("0");
    currentExpression = "0";
    result = 0.0;
    waitingForOperand = true;
}

void MainWindow::pointClicked(){
    if (waitingForOperand) {
        display->setText("0");
        waitingForOperand = false;
    }
    
    if (!display->text().contains('.')) {
        display->setText(display->text() + ".");
    }
}

void MainWindow::parenthesisClicked() {
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString parenthesis = clickedButton->text();
    
    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    
    // 更新预览
    updatePreview(parenthesis);
}

void MainWindow::backspaceClicked() {
    if (display->text().length() > 0) {
        QString currentText = display->text();
        currentText.chop(1); // 删除最后一个字符
        
        if (currentText.isEmpty()) {
            display->setText("0");
            waitingForOperand = true;
        } else {
            display->setText(currentText);
        }
    }
}

void MainWindow::updatePreview(const QString &newInput) {
    // 如果当前表达式是"0"且新输入是数字，替换0
    if (currentExpression == "0" && newInput != "0" && 
        newInput != "+" && newInput != "-" && newInput != "×" && newInput != "÷" &&
        newInput != "(" && newInput != ")") {
        currentExpression = newInput;
    } else {
        currentExpression += newInput;
    }
    
    previewDisplay->setText(currentExpression);
}

double MainWindow::evaluateExpression(const QString &expression) {
    QString expr = expression;
    expr.replace("×", "*").replace("÷", "/");
    
    // 简单的从左到右计算
    QVector<double> numbers;
    QVector<QChar> operators;
    
    QString currentNumber;
    for (int i = 0; i < expr.length(); ++i) {
        QChar c = expr[i];
        
        if (c.isDigit() || c == '.') {
            currentNumber += c;
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            if (!currentNumber.isEmpty()) {
                numbers.append(currentNumber.toDouble());
                currentNumber.clear();
            }
            operators.append(c);
        }
    }
    
    // 添加最后一个数字
    if (!currentNumber.isEmpty()) {
        numbers.append(currentNumber.toDouble());
    }
    
    // 如果没有操作符，直接返回数字
    if (operators.isEmpty()) {
        return numbers.value(0, 0);
    }
    
    // 从左到右计算
    double result = numbers[0];
    for (int i = 0; i < operators.size(); ++i) {
        double nextNumber = numbers.value(i + 1, 0);
        
        switch (operators[i].toLatin1()) {
            case '+': result += nextNumber; break;
            case '-': result -= nextNumber; break;
            case '*': result *= nextNumber; break;
            case '/': 
                if (nextNumber == 0.0) {
                    display->setText("Error");
                    return 0.0;
                }
                result /= nextNumber; 
                break;
        }
    }
    
    return result;
}