#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager(this) {
  ui->setupUi(this);
  connect(&manager, &QNetworkAccessManager::finished, this,
          &MainWindow::readSandbox);

  connect(ui->openGLWidget, &OWidget::ready, this, &MainWindow::readShaders);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_urlLineEdit_returnPressed() {
  QString url = ui->urlLineEdit->text();
  url = "http://glslsandbox.com/item/" + url.mid(url.indexOf('#') + 1);
  manager.get(QNetworkRequest(url));
}

void MainWindow::readShaders() {
  QDir dir("shaders");
  QListIterator<QFileInfo> it(
      dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot));
  while (it.hasNext()) {
    QFileInfo file = it.next();
    if (ui->shaderSelection->findText(file.fileName()) >= 0) continue;
    bool ret = ui->openGLWidget->compileFromFile(file.absoluteFilePath());
    if (!ret) continue;
    ui->shaderSelection->addItem(file.fileName());
  }
}

void MainWindow::readSandbox(QNetworkReply *reply) {
  bool ret;
  QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
  ret = ui->openGLWidget->compileFromSandbox(
      doc.object().value("code").toString());
  if (!ret) return;
  ui->shaderSelection->addItem(reply->url().fileName());
  ui->shaderSelection->setCurrentIndex(ui->shaderSelection->count() - 1);
}

void MainWindow::on_shaderLoadButton_clicked() { readShaders(); }

void MainWindow::on_pushButton_clicked() { on_urlLineEdit_returnPressed(); }
