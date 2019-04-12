#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkReply>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager(this) {
  ui->setupUi(this);
  connect(&manager, &QNetworkAccessManager::finished, this,
          [=](QNetworkReply *reply) {
            ui->openGLWidget->compileShader(reply->readAll());
          });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_urlLineEdit_returnPressed() {
  manager.get(QNetworkRequest(ui->urlLineEdit->text()));
}
