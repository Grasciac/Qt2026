#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QMessageBox>
#include <QRandomGenerator>

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    for (size_t i = 0; i < state.players_count; ++i) {
        const auto& player = state.players[i];
        if (!player.is_online) continue;

        int px = (player.x + 10) * 15;
        int py = (player.y + 10) * 15;
        p.setBrush(QColor(QString::fromStdString(player.color)));

        if (player.id == my_id) {
            p.drawEllipse(px, py, 15, 15);
        } else {
            p.drawRect(px, py, 15, 15);
        }
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0); // Окно 1 при запуске

    canvas = new CanvasWidget(ui->canvas_container);
    canvas->setFixedSize(300, 300);

    ui->cb_color->addItems({"Синий", "Красный", "Зеленый", "Желтый"});

    poll_timer_ = new QTimer(this);
    connect(poll_timer_, &QTimer::timeout, this, &MainWindow::on_poll_timer);

    imitation_timer_ = new QTimer(this);
    connect(imitation_timer_, &QTimer::timeout, this, &MainWindow::on_imitation_timer);

    connect(ui->btn_connect, &QPushButton::clicked, this, &MainWindow::on_btn_connect_clicked);
    connect(ui->btn_up, &QPushButton::clicked, this, &MainWindow::move_up);
    connect(ui->btn_down, &QPushButton::clicked, this, &MainWindow::move_down);
    connect(ui->btn_left, &QPushButton::clicked, this, &MainWindow::move_left);
    connect(ui->btn_right, &QPushButton::clicked, this, &MainWindow::move_right);
    connect(ui->btn_imitate, &QPushButton::clicked, this, &MainWindow::toggle_imitation);
    connect(ui->cb_color, &QComboBox::currentIndexChanged, this, &MainWindow::update_color);
}

MainWindow::~MainWindow() { delete ui; }

QString MainWindow::get_current_hex_color() {
    QString text = ui->cb_color->currentText();
    if (text == "Красный") return "#FF0000";
    if (text == "Зеленый") return "#00FF00";
    if (text == "Желтый") return "#FFFF00";
    return "#0000FF"; // Синий
}

void MainWindow::on_btn_connect_clicked() {
    my_id = ui->le_id->text().toInt();
    room.set_address(ui->le_ip->text().toStdString(), ui->le_port->text().toInt());

    send_action(1); // AUTH
}

void MainWindow::send_action(int type) {
    game::ClientAction act{type, my_id, cur_x, cur_y};
    std::strcpy(act.color, get_current_hex_color().toStdString().c_str());

    game::GameState state = room.run_once(act);

    if (type == 1) { // Проверка авторизации
        if (state.status_code == 1) {
            QMessageBox::warning(this, "Ошибка", "Этот ID уже подключен!");
            return;
        } else if (state.status_code == -1) {
            QMessageBox::warning(this, "Ошибка", "Сервер недоступен!");
            return;
        }
        ui->stackedWidget->setCurrentIndex(1); // Переход на Окно 2
        canvas->my_id = my_id;
        poll_timer_->start(500);
    }

    if (state.status_code == 0) {
        ui->lb_weather->setText(QString("Погода:\n%1").arg(state.weather));
        canvas->state = state;
        canvas->update();
    }
}

void MainWindow::on_poll_timer() { send_action(0); }
void MainWindow::update_color() { send_action(2); }

void MainWindow::move_up()    { cur_y--; send_action(2); }
void MainWindow::move_down()  { cur_y++; send_action(2); }
void MainWindow::move_left()  { cur_x--; send_action(2); }
void MainWindow::move_right() { cur_x++; send_action(2); }

void MainWindow::toggle_imitation() {
    is_imitating = !is_imitating;
    ui->btn_up->setEnabled(!is_imitating);
    ui->btn_down->setEnabled(!is_imitating);
    ui->btn_left->setEnabled(!is_imitating);
    ui->btn_right->setEnabled(!is_imitating);

    if (is_imitating) imitation_timer_->start(1000);
    else imitation_timer_->stop();
}

void MainWindow::on_imitation_timer() {
    cur_x += QRandomGenerator::global()->bounded(-1, 2);
    cur_y += QRandomGenerator::global()->bounded(-1, 2);
    send_action(2);
}
