#include "MyButton.h"

#include <QIcon>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>

MyButton::MyButton(bool clickSwapped, QWidget *parent) :
    QPushButton(parent), pressed(false), flagged(false), questioned(false), peeked(false), click_swapped(clickSwapped)
{
    setStyleSheet("background-color: #ADD8E6;");
    connect(this, &QPushButton::clicked, this, &MyButton::change_color);
}

void MyButton::swap_click_setter(bool clickSwapped)
{
    this->click_swapped = clickSwapped;
}

void MyButton::change_color()
{
    if (!pressed && !flagged && !questioned)
    {
        QString color;
        if (text == "*")
        {
            color = "black";
            setStyleSheet(QString("background-color: #FF0000; color: %1;").arg(color));
            setIcon(QIcon(":/_mine.png"));
            setIconSize(QSize(50, 50));
            pressed = true;
            setEnabled(false);
            return;
        }
        else
        {
            int num = text.toInt();
            switch (num)
            {
            case 1:
                color = "blue";
                break;
            case 2:
                color = "green";
                break;
            case 3:
                color = "red";
                break;
            case 4:
                color = "darkblue";
                break;
            case 5:
                color = "darkred";
                break;
            case 6:
                color = "cyan";
                break;
            case 7:
                color = "magenta";
                break;
            case 8:
                color = "black";
                break;
            default:
                color = "black";
                break;
            }
            setStyleSheet(QString("background-color: #D3D3D3; color: %1;").arg(color));
            setText(text);
            pressed = true;
        }
    }
}

void MyButton::set_text(const QString message)
{
    this->text = message;
}

void MyButton::toggle_flag()
{
    if (!pressed)
    {
        if (flagged)
        {
            flagged = false;
            questioned = true;
            setIcon(QIcon(":/question.png"));
            emit flag_changed(false);
        }
        else if (questioned)
        {
            questioned = false;
            setIcon(QIcon());
        }
        else
        {
            flagged = true;
            setIcon(QIcon(":/flag.png"));
            emit flag_changed(true);
        }
        setIconSize(QSize(50, 50));
    }
}

void MyButton::put_flag()
{
    toggle_flag();
}

void MyButton::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::RightButton && !click_swapped) || (event->button() == Qt::LeftButton && click_swapped))
    {
        put_flag();
    }
    else if (event->button() == Qt::MiddleButton)
    {
        if (is_revealed())
        {
            emit middle_clicked();
        }
    }
    else if ((event->button() == Qt::LeftButton && !click_swapped) || (event->button() == Qt::RightButton && click_swapped))
    {
        if (!is_flagged() && !is_questioned())
        {
            change_color();
            emit clicked();
        }
    }
}

bool MyButton::is_flagged()
{
    return this->flagged;
}

bool MyButton::is_questioned()
{
    return this->questioned;
}

bool MyButton::is_revealed()
{
    return this->pressed;
}

void MyButton::highlight()
{
    setStyleSheet("background-color: yellow;");
    QTimer::singleShot(
        500,
        this,
        [this]()
        {
            if (!this->is_revealed())
            {
                setStyleSheet("background-color: #ADD8E6;");
            }
            else
            {
                change_color();
            }
        });
}

void MyButton::reset_color()
{
    if (!peeked)
    {
        setStyleSheet("background-color: #ADD8E6;");
        setText("");
        setIcon(QIcon());
        if (pressed && text == "*")
            setEnabled(true);
        pressed = false;
    }
}

void MyButton::set_peeked(bool value)
{
    this->peeked = value;
}

bool MyButton::is_peeked() const
{
    return this->peeked;
}

void MyButton::set_revealed(bool revealed)
{
    pressed = revealed;
}
