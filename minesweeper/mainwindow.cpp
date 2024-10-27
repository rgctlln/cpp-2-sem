#include "MenuWindow.h"
#include "Mine.h"
#include "MyButton.h"
#include "mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QQueue>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QToolBar>
#include <QVBoxLayout>
#include <QVector>

void MainWindow::saveGameState()
{
    QSettings settings("save.ini", QSettings::IniFormat);
    settings.beginGroup("GameState");
    settings.setValue("rows", row_cnt);
    settings.setValue("columns", column_cnt);
    settings.setValue("mines", mine_cnt);
    settings.setValue("opened_cells", opened_cells);

    settings.beginWriteArray("field");
    const QVector< QVector< int > > &field = mine_field->get_field();
    for (int i = 0; i < field.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.beginWriteArray("row");
        for (int j = 0; j < field[i].size(); ++j)
        {
            settings.setArrayIndex(j);
            settings.setValue("value", field[i][j]);
            settings.setValue("flagged", buttons_vector[i * field[i].size() + j]->is_flagged());
        }
        settings.endArray();
    }
    settings.endArray();

    settings.beginWriteArray("revealed_cells");
    for (int i = 0; i < buttons_vector.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("revealed", buttons_vector[i]->is_revealed());
    }
    settings.endArray();
    settings.endGroup();
}

void MainWindow::loadGameState()
{
    QSettings settings("save.ini", QSettings::IniFormat);
    settings.beginGroup("GameState");
    if (!settings.contains("rows") || !settings.contains("columns") || !settings.contains("mines"))
    {
        QMessageBox::information(this, "No Saved Game", "There is no saved game to load.");
        settings.endGroup();
        return;
    }

    row_cnt = settings.value("rows", 10).toInt();
    column_cnt = settings.value("columns", 10).toInt();
    mine_cnt = settings.value("mines", 10).toInt();
    opened_cells = settings.value("opened_cells", 0).toInt();

    QVector< QVector< int > > field(row_cnt, QVector< int >(column_cnt));
    int size = settings.beginReadArray("field");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        int rowSize = settings.beginReadArray("row");
        for (int j = 0; j < rowSize; ++j)
        {
            settings.setArrayIndex(j);
            field[i][j] = settings.value("value").toInt();
        }
        settings.endArray();
    }
    settings.endArray();
    mine_field->set_field(field);
    fill_layout(row_cnt, column_cnt);
    int revealedSize = settings.beginReadArray("revealed_cells");
    if (revealedSize != buttons_vector.size())
    {
        return;
    }
    for (int i = 0; i < revealedSize; ++i)
    {
        settings.setArrayIndex(i);
        if (settings.value("revealed").toBool())
        {
            buttons_vector[i]->change_color();
        }
    }
    settings.endArray();
    int flagSize = settings.beginReadArray("field");
    for (int i = 0; i < flagSize; ++i)
    {
        settings.setArrayIndex(i);
        int rowSize = settings.beginReadArray("row");
        for (int j = 0; j < rowSize; ++j)
        {
            settings.setArrayIndex(j);
            if (settings.value("flagged").toBool())
            {
                buttons_vector[i * column_cnt + j]->put_flag();
            }
        }
        settings.endArray();
    }
    settings.endArray();
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveGameState();
    QMainWindow::closeEvent(event);
}

void MainWindow::fill_layout(int row_cnt, int column_cnt)
{
    this->grid_layout->setSpacing(0);
    if (!mine_field)
    {
        mine_field = new Mine(row_cnt, column_cnt, mines_placed);
    }
    const QVector< QVector< int > > &field = mine_field->get_field();
    buttons_vector.clear();

    int buttonSize = 50;

    for (int i = 0; i < row_cnt; i++)
    {
        for (int j = 0; j < column_cnt; j++)
        {
            MyButton *button = new MyButton;
            button->setFixedSize(buttonSize, buttonSize);
            if (field[i][j] == -1)
            {
                button->set_text("*");
            }
            else if (field[i][j] == 0)
            {
                button->set_text("");
            }
            else
            {
                button->set_text(QString::number(field[i][j]));
            }
            connect(
                button,
                &MyButton::clicked,
                this,
                [this, button, field, i, j]()
                {
                    if (is_first_move)
                    {
                        handle_first_click(i, j);
                    }
                    else if (field[i][j] == -1 && !is_first_move)
                    {
                        button->setStyleSheet("background-color: #8B0000; color: black;");
                        reveal_all_mines();
                        reveal_all_cells();
                        QMessageBox::critical(this, "Game Over", "You Lose");
                    }
                    else
                    {
                        reveal_empty_cells(i, j);
                    }
                });
            connect(button, &MyButton::middle_clicked, this, [this, i, j]() { handle_middle_click(i, j); });
            connect(button, &MyButton::flag_changed, this, &MainWindow::flag_change);
            this->buttons_vector.append(button);
            this->grid_layout->addWidget(button, i, j);
        }
    }
}

MainWindow::MainWindow(int rows, int columns, int mines, bool debug, QWidget *parent) :
    QMainWindow(parent), grid_layout(new QGridLayout), mine_field(nullptr), total_cells(rows * columns),
    opened_cells(0), mines_placed(mines), row_cnt(rows), column_cnt(columns), mine_cnt(mines), is_first_move(true),
    debug_mode(debug), revealed(false), swap_made(false), flag_cnt(0), game_end(false)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    field_widget = new QWidget(this);
    fill_layout(rows, columns);
    field_widget->setLayout(grid_layout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(field_widget);
    mainLayout->setAlignment(field_widget, Qt::AlignCenter);
    centralWidget->setLayout(mainLayout);

    tool_bar = addToolBar("Main Toolbar");
    tool_bar->setMovable(false);

    mines_cnt_label = new QLabel(this);
    mines_cnt_label->setStyleSheet("QLabel { font-weight: bold; font-size: 16px; }");
    update_mines_counter();
    tool_bar->addWidget(mines_cnt_label);

    toggle_click = new QPushButton("LMB <-> RMB Mode: Off", this);
    connect(toggle_click, &QPushButton::clicked, this, &MainWindow::click_mode_toggle);
    tool_bar->addWidget(toggle_click);

    QAction *newGameAction = new QAction("Restart Game", this);
    connect(newGameAction, &QAction::triggered, this, &MainWindow::restart_game);
    tool_bar->addAction(newGameAction);

    QAction *restartAction = new QAction("Start New Game", this);
    connect(restartAction, &QAction::triggered, this, &MainWindow::start_new_game);
    tool_bar->addAction(restartAction);

    if (debug_mode)
    {
        QAction *toggleRevealAction = new QAction("Toggle Reveal", this);
        connect(toggleRevealAction, &QAction::triggered, this, &MainWindow::toggle_all_cells);
        tool_bar->addAction(toggleRevealAction);
    }
    this->setMinimumSize(rows * 50 + 20, columns * 50 + tool_bar->height() + 20);
    centralWidget->setStyleSheet("background-color: #FFA07A;");
}

void MainWindow::update_mines_counter()
{
    mines_cnt_label->setText("Mines count: " + QString::number(mines_placed - flag_cnt));
}

MainWindow::~MainWindow()
{
    delete mine_field;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    int windowWidth = this->width();
    int windowHeight = this->height() - tool_bar->height();
    int buttonWidth = windowWidth / column_cnt;
    int buttonHeight = windowHeight / row_cnt;
    int buttonSize = qMin(buttonWidth, buttonHeight);
    for (auto button : buttons_vector)
    {
        button->setFixedSize(buttonSize, buttonSize);
        button->setIconSize(QSize(buttonSize - 5, buttonSize - 5));
    }
}

void MainWindow::handle_first_click(int row, int col)
{
    if (mine_field->get_field()[row][col] == -1)
    {
        mine_field->generate(row_cnt, column_cnt, mines_placed, row, col);
        fill_layout(row_cnt, column_cnt);
    }
    is_first_move = false;
    reveal_empty_cells(row, col);
}

void MainWindow::reveal_all_mines()
{
    const QVector< QVector< int > > &field = mine_field->get_field();
    for (int i = 0; i < field.size(); i++)
    {
        for (int j = 0; j < field.at(0).size(); j++)
        {
            if (field[i][j] == -1)
            {
                buttons_vector[i * field[0].size() + j]->change_color();
            }
        }
    }
    disable_all_buttons();
}

void MainWindow::disable_all_buttons()
{
    for (MyButton *button : buttons_vector)
    {
        button->setEnabled(false);
    }
}

void MainWindow::reveal_empty_cells(int row, int col)
{
    QQueue< QPair< int, int > > queue;
    queue.enqueue(qMakePair(row, col));
    QVector< QVector< bool > > visited(row_cnt, QVector< bool >(column_cnt, false));
    while (!queue.isEmpty())
    {
        QPair< int, int > cell = queue.dequeue();
        int r = cell.first;
        int c = cell.second;

        if (r < 0 || r >= row_cnt || c < 0 || c >= column_cnt || visited[r][c])
            continue;

        visited[r][c] = true;

        MyButton *button = buttons_vector[r * column_cnt + c];
        if (!button->is_revealed() && !button->is_flagged())
        {
            button->change_color();
        }

        if (mine_field->get_field()[r][c] == 0)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    int newRow = r + dx;
                    int newCol = c + dy;
                    if (newRow >= 0 && newRow < row_cnt && newCol >= 0 && newCol < column_cnt && !visited[newRow][newCol])
                    {
                        queue.enqueue(qMakePair(newRow, newCol));
                    }
                }
            }
        }
        check_win();
    }
}

void MainWindow::click_mode_toggle()
{
    swap_made = !swap_made;
    if (swap_made)
    {
        toggle_click->setText("LMB <-> RMB Mode: On");
    }
    else
    {
        toggle_click->setText("LMB <-> RMB Mode: Off");
    }
    for (MyButton *button : buttons_vector)
    {
        button->swap_click_setter(swap_made);
    }
}

void MainWindow::check_win()
{
    int openedCount = 0;
    for (auto button : buttons_vector)
    {
        if (button->is_revealed())
        {
            openedCount++;
        }
    }
    if (openedCount == total_cells - mines_placed)
    {
        QMessageBox::information(this, "Congratulations", "You Win!");
        disable_all_buttons();
        game_end = true;
    }
}

void MainWindow::start_new_game()
{
    this->close();
    QFile::remove("save.ini");
    MenuWindow *menuWindow = new MenuWindow();
    menuWindow->show();
}

void MainWindow::handle_middle_click(int row, int col)
{
    MyButton *originButton = buttons_vector[row * column_cnt + col];
    if (!originButton->is_revealed())
    {
        return;
    }
    const QVector< QVector< int > > &field = mine_field->get_field();
    int surroundingMines = field[row][col];
    int markedFlags = 0;
    QVector< QPair< int, int > > cellsToReveal;
    bool incorrectFlag = false;
    int directions[8][2] = { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
    for (auto &dir : directions)
    {
        int newRow = row + dir[0];
        int newCol = col + dir[1];
        if (newRow >= 0 && newRow < field.size() && newCol >= 0 && newCol < field[0].size())
        {
            MyButton *button = buttons_vector[newRow * field[0].size() + newCol];
            if (button->is_flagged())
            {
                markedFlags++;
                if (field[newRow][newCol] != -1)
                {
                    incorrectFlag = true;
                }
            }
            else if (!button->is_revealed())
            {
                cellsToReveal.append(qMakePair(newRow, newCol));
            }
        }
    }
    if (incorrectFlag)
    {
        reveal_all_mines();
        QMessageBox::critical(this, "Game Over", "You Lose");
        return;
    }

    if (markedFlags == surroundingMines)
    {
        for (const auto &cell : cellsToReveal)
        {
            reveal_empty_cells(cell.first, cell.second);
        }
    }
    else
    {
        for (const auto &cell : cellsToReveal)
        {
            buttons_vector[cell.first * field[0].size() + cell.second]->highlight();
        }
    }
}

void MainWindow::toggle_all_cells()
{
    if (game_end)
    {
        return;
    }
    if (revealed)
    {
        hide_all_cells();
    }
    else
    {
        reveal_all_cells();
    }
    revealed = !revealed;
}

void MainWindow::reveal_all_cells()
{
    for (MyButton *button : buttons_vector)
    {
        if (!button->is_revealed() && !button->is_flagged())
        {
            button->set_peeked(true);
            button->change_color();
        }
    }
}

void MainWindow::hide_all_cells()
{
    for (MyButton *button : buttons_vector)
    {
        if (button->is_peeked())
        {
            button->set_peeked(false);
            button->reset_color();
            if (button->is_flagged())
            {
                button->put_flag();
            }
            else
            {
                button->setEnabled(true);
            }
        }
    }
}

void MainWindow::restart_game()
{
    opened_cells = 0;
    flag_cnt = 0;
    update_mines_counter();
    game_end = false;
    for (auto button : buttons_vector)
    {
        grid_layout->removeWidget(button);
        delete button;
    }
    buttons_vector.clear();
    if (mine_field != nullptr)
    {
        delete mine_field;
        mine_field = nullptr;
    }
    mine_field = new Mine(row_cnt, column_cnt, mines_placed);
    is_first_move = true;
    fill_layout(row_cnt, column_cnt);
    this->update();
}

void MainWindow::flag_change(bool flagged)
{
    if (flagged)
    {
        flag_cnt++;
    }
    else
    {
        flag_cnt--;
    }
    update_mines_counter();
}
