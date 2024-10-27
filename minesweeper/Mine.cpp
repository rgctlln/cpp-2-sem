#include "Mine.h"

#include <QRandomGenerator>

const QVector< QVector< int > >& Mine::get_field()
{
    return this->field;
}

void Mine::distance_to_mine()
{
    int directions[8][2] = { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
    for (int r = 0; r < this->row_cnt; r++)
    {
        for (int c = 0; c < this->column_cnt; c++)
        {
            if (this->field[r][c] == -1)
            {
                continue;
            }
            int mines_around = 0;
            for (auto& elem : directions)
            {
                int xr = r + elem[0];
                int yc = c + elem[1];
                if (xr >= 0 && xr < this->row_cnt && yc >= 0 && yc < this->column_cnt && field[xr][yc] == -1)
                {
                    mines_around++;
                }
            }
            field[r][c] = mines_around;
        }
    }
}

void Mine::place_mines()
{
    int placed_mines = 0;
    for (; placed_mines < this->mine_cnt;)
    {
        int row = rand() % this->row_cnt;
        int column = rand() % this->column_cnt;
        if (this->field[row][column] == -1)
        {
            goto skip;
        }
        this->field[row][column] = -1;
        placed_mines++;
    skip:;
    }
}

void Mine::create_field()
{
    place_mines();
    distance_to_mine();
}

Mine::Mine(int row_cnt, int column_cnt, int mine_cnt) :
    row_cnt(row_cnt), column_cnt(column_cnt), mine_cnt(mine_cnt), field(row_cnt, QVector< int >(column_cnt, 0))
{
    srand(static_cast< unsigned int >(time(0)));
    create_field();
}

void Mine::set_field(const QVector< QVector< int > >& field)
{
    this->field = field;
}

void Mine::generate(int rows, int columns, int mines, int first_row, int first_col)
{
    field.clear();
    field.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        field[i].resize(columns);
    }
    int placed_mines = 0;
    while (placed_mines < mines)
    {
        int row = QRandomGenerator::global()->bounded(rows);
        int col = QRandomGenerator::global()->bounded(columns);
        if (field[row][col] == -1 || (row == first_row && col == first_col))
        {
            continue;
        }
        field[row][col] = -1;
        placed_mines++;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                int newRow = row + i;
                int newCol = col + j;

                if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < columns && field[newRow][newCol] != -1)
                {
                    field[newRow][newCol]++;
                }
            }
        }
    }
}
