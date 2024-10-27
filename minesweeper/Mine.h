#ifndef MINE_H
#define MINE_H

#include <QApplication>
#include <QVector>

class Mine
{
public:
    Mine(int row_cnt, int column_cnt, int mine_cnt);

    void create_field();

    void place_mines();

    const QVector< QVector< int > >& get_field();

    void set_field(const QVector< QVector< int > >& field);

    void generate(int rows, int columns, int mines, int first_row, int first_col);

private:
    int row_cnt;

    int column_cnt;

    int mine_cnt;

    QVector< QVector< int > > field;

    void distance_to_mine();
};

#endif
