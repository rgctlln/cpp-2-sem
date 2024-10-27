#include "MenuWindow.h"
#include "mainwindow.h"

#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QSettings>

MenuWindow::MenuWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Main Menu");
    setFixedSize(400, 300);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(173, 216, 230));
    this->setPalette(palette);

    QLabel *rowsLabel = new QLabel("Rows:", this);
    rowsLabel->setStyleSheet("font-weight: bold;");

    rows_spin_box = new QSpinBox(this);
    rows_spin_box->setRange(5, 30);
    rows_spin_box->setValue(10);

    QLabel *columnsLabel = new QLabel("Columns:", this);
    columnsLabel->setStyleSheet("font-weight: bold;");

    columns_spin_box = new QSpinBox(this);
    columns_spin_box->setRange(5, 30);
    columns_spin_box->setValue(10);

    QLabel *minesLabel = new QLabel("Mines:", this);
    minesLabel->setStyleSheet("font-weight: bold;");

    mines_spin_box = new QSpinBox(this);
    mines_spin_box->setRange(1, 100);
    mines_spin_box->setValue(10);

    debug_check_box = new QCheckBox("Enable Debug", this);
    debug_check_box->setStyleSheet("font-weight: bold;");

    start_button = new QPushButton("Start", this);
    start_button->setStyleSheet("background-color: white; color: black; font-weight: bold;");

    connect(start_button, &QPushButton::clicked, this, &MenuWindow::start_game);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *rowsLayout = new QHBoxLayout;
    rowsLayout->addWidget(rowsLabel);
    rowsLayout->addWidget(rows_spin_box);

    QHBoxLayout *columnsLayout = new QHBoxLayout;
    columnsLayout->addWidget(columnsLabel);
    columnsLayout->addWidget(columns_spin_box);

    QHBoxLayout *minesLayout = new QHBoxLayout;
    minesLayout->addWidget(minesLabel);
    minesLayout->addWidget(mines_spin_box);

    mainLayout->addLayout(rowsLayout);
    mainLayout->addLayout(columnsLayout);
    mainLayout->addLayout(minesLayout);
    mainLayout->addWidget(debug_check_box);
    mainLayout->addWidget(start_button);
    setLayout(mainLayout);
}

void MenuWindow::start_game()
{
    QFile saveFile("savegame.ini");
    bool saveExists = saveFile.exists();
    int rows = rows_spin_box->value();
    int columns = columns_spin_box->value();
    int mines = mines_spin_box->value();
    bool debugMode = debug_check_box->isChecked();
    int maxMines = rows * columns - 1;
    if (mines > maxMines)
    {
        QMessageBox::warning(this, "Invalid Input", "The number of mines is too high for the given grid size. Please reduce the number of mines.");
        return;
    }
    if (saveExists)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Load Saved Game", "A saved game exists. Do you want to load it?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            MainWindow *mainWindow = new MainWindow(rows, columns, mines, debugMode);
            mainWindow->loadGameState();
            mainWindow->show();
            this->close();
            return;
        }
    }
    MainWindow *mainWindow = new MainWindow(rows, columns, mines, debugMode);
    mainWindow->show();
    this->close();
}

void MenuWindow::enable_debug_mode()
{
    debug_check_box->setChecked(true);
}
