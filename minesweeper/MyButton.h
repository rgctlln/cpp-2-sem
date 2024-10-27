#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QApplication>
#include <QPushButton>

class MyButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MyButton(bool clickSwapped = false, QWidget *parent = nullptr);

    void set_text(const QString message);

    void set_button_text();

    void change_color();

    void reset_color();

    void put_flag();

    void toggle_flag();

    QString get_text();

    bool is_flagged();

    bool is_questioned();

    bool is_revealed();

    void highlight();

    void set_peeked(bool value);

    bool is_peeked() const;

    void swap_click_setter(bool clickSwapped);

    void set_revealed(bool revealed);

signals:
    void middle_clicked();

    void flag_changed(bool flagged);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool pressed;

    bool flagged;

    bool questioned;

    QString text;

    bool peeked;

    bool click_swapped;
};

#endif
