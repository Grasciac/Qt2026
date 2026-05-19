#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QString>
#include "../common/game_room.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    game::GameState state = {};
    int my_id = -1;
protected:
    void paintEvent(QPaintEvent *event) override;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_connect_clicked();
    void on_poll_timer();
    void on_imitation_timer();

    void move_up(); void move_down();
    void move_left(); void move_right();
    void toggle_imitation();
    void update_color();

private:
    void send_action(int type);
    QString get_current_hex_color();

    Ui::MainWindow *ui;
    game::GameRoom room;
    QTimer *poll_timer_;
    QTimer *imitation_timer_;
    CanvasWidget *canvas;

    int my_id = -1;
    int cur_x = 0;
    int cur_y = 0;
    bool is_imitating = false;
};
#endif
