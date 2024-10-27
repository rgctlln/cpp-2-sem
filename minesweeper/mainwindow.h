#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Mine.h"
#include "MyButton.h"

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QToolBar>
#include <QVector>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int rows, int columns, int mines, bool debugMode = false, QWidget *parent = nullptr);

    ~MainWindow();

    void loadGameState();

protected:
    void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

private slots:
    void start_new_game();

    void handle_middle_click(int row, int col);

    void click_mode_toggle();

    void flag_change(bool flagged);

private:
    void fill_layout(int row_cnt, int column_cnt);

    void reveal_all_mines();

    void disable_all_buttons();

    void reveal_empty_cells(int row, int col);

    void check_win();

    void first_click_action(int row, int col);

    void reveal_all_cells();

    void restart_game();

    void toggle_all_cells();

    void hide_all_cells();

    void saveGameState();

    void process_single_cell(int i, int j);

    void update_mines_counter();

    void handle_first_click(int row, int col);

    QGridLayout *grid_layout;

    QVector< MyButton * > buttons_vector;

    Mine *mine_field;

    int total_cells;

    int opened_cells;

    int mines_placed;

    int row_cnt;

    int column_cnt;

    int mine_cnt;

    bool is_first_move;

    bool debug_mode;

    bool revealed;

    bool swap_made;

    QToolBar *tool_bar;

    QWidget *field_widget;

    QPushButton *toggle_click;

    QLabel *mines_cnt_label;

    int flag_cnt;

    bool game_end;
};

#endif
