#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class MenuWindow : public QWidget
{
    Q_OBJECT

public:
    MenuWindow(QWidget *parent = nullptr);

    void enable_debug_mode();

private slots:
    void start_game();

private:
    QPushButton *start_button;

    QSpinBox *rows_spin_box;

    QSpinBox *columns_spin_box;

    QSpinBox *mines_spin_box;

    QCheckBox *debug_check_box;
};

#endif
