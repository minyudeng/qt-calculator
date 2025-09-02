#include "mainwindow.h"
#include <QGridLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , result(0.0)
    , waitingForOperand(true){
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建显示框
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);
    display->setStyleSheet("QLineEdit { font-size: 30px; padding: 10px; background-color: #f0f0f0; border: 2px solid #ccc; }");

    // 创建数字按钮 0-9
    for (int i = 0; i < 10; ++i) {
        digitButtons.append(createButton(QString::number(i), SLOT(digitClicked())));
    }

    // 创建操作符按钮
    QPushButton *plusButton = createButton("+", SLOT(operatorClicked()));
    QPushButton *minusButton = createButton("-", SLOT(operatorClicked()));
    QPushButton *multiplyButton = createButton("×", SLOT(operatorClicked())); // 使用×而不是x
    QPushButton *divideButton = createButton("÷", SLOT(operatorClicked()));
    QPushButton *equalButton = createButton("=", SLOT(equalClicked()));
    QPushButton *clearButton = createButton("C", SLOT(clearClicked()));
    QPushButton *clearAllButton = createButton("AC", SLOT(clearAllClicked()));
    QPushButton *pointButton = createButton(".", SLOT(pointClicked()));

    // 创建布局
    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(display, 0, 0, 1, 4);

    mainLayout->addWidget(clearAllButton, 1, 0);
    mainLayout->addWidget(clearButton, 1, 1);

    // 添加数字按钮
    for (int i = 1; i < 10; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = (i - 1) % 3;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 5, 1);
    mainLayout->addWidget(pointButton, 5, 0);

    // 添加操作符按钮
    mainLayout->addWidget(plusButton, 2, 3);
    mainLayout->addWidget(minusButton, 3, 3);
    mainLayout->addWidget(multiplyButton, 4, 3);
    mainLayout->addWidget(divideButton, 5, 3);
    mainLayout->addWidget(equalButton, 5, 2);

    // 设置按钮样式
    QString buttonStyle = "QPushButton { font-size: 20px; padding: 15px; background-color: #e0e0e0; border: 1px solid #ccc; }"
                          "QPushButton:hover { background-color: #d0d0d0; }"
                          "QPushButton:pressed { background-color: #c0c0c0; }";

    for (QPushButton* button : centralWidget->findChildren<QPushButton*>()) {
        button->setStyleSheet(buttonStyle);
        button->setMinimumSize(60, 60);
    }

    // 设置操作符按钮的特殊样式
    QString operatorStyle = "QPushButton { background-color: #f0a050; }"
                            "QPushButton:hover { background-color: #e09040; }"
                            "QPushButton:pressed { background-color: #d08030; }";

    plusButton->setStyleSheet(operatorStyle);
    minusButton->setStyleSheet(operatorStyle);
    multiplyButton->setStyleSheet(operatorStyle);
    divideButton->setStyleSheet(operatorStyle);
    equalButton->setStyleSheet(operatorStyle);

    setWindowTitle(tr("DMY Calculator"));
    resize(400, 500);
    }

MainWindow::~MainWindow() {

}

QPushButton *MainWindow::createButton(const QString &text, const char *member) {
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    // connect(button, &QPushButton::clicked, this, member);
    return button;
}

void MainWindow::digitClicked() {
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    int digitValue = clickedButton->text().toInt();
    
    if (display->text() == "0" && digitValue == 0.0) {
        return;
    }
    
    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    
    display->setText(display->text() + QString::number(digitValue));
}

void MainWindow::operatorClicked(){
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    
    if (!pendingOperator.isEmpty()) {
        calculate(operand);
        display->setText(QString::number(result));
    } else {
        result = operand;
    }
    
    pendingOperator = clickedOperator;
    waitingForOperand = true;
}

void MainWindow::equalClicked(){
    double operand = display->text().toDouble();
    
    if (!pendingOperator.isEmpty()) {
        calculate(operand);
        pendingOperator.clear();
    } else {
        result = operand;
    }
    
    display->setText(QString::number(result));
    result = 0.0;
    waitingForOperand = true;
}

void MainWindow::clearClicked(){
    if (display->text().length() > 0) {
        display->setText(display->text().left(display->text().length() - 1));
        if (display->text().isEmpty()) {
            display->setText("0");
            waitingForOperand = true;
        }
    }
}

void MainWindow::clearAllClicked(){
    display->setText("0");
    result = 0.0;
    pendingOperator.clear();
    waitingForOperand = true;
}

void MainWindow::pointClicked(){
    if (waitingForOperand) {
        display->setText("0");
    }
    
    if (!display->text().contains('.')) {
        display->setText(display->text() + ".");
    }
    
    waitingForOperand = false;
}

void MainWindow::calculate(double operand){
    if (pendingOperator == "+") {
        result += operand;
    } else if (pendingOperator == "-") {
        result -= operand;
    } else if (pendingOperator == "×") {
        result *= operand;
    } else if (pendingOperator == "÷") {
        if (operand == 0.0) {
            display->setText("Error");
            result = 0.0;
            pendingOperator.clear();
            waitingForOperand = true;
            return;
        }
        result /= operand;
    }
}
