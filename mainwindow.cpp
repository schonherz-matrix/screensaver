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
  QByteArray data = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  ret = ui->openGLWidget->compileFromSandbox(
      doc.object().value("code").toString());
  if (!ret) return;
  QVariant v(data);
  ui->urlInput->setItemData(ui->urlInput->currentIndex(), v);
  reply->deleteLater();
}

void MainWindow::on_shaderLoadButton_clicked() { readShaders(); }

void MainWindow::on_urlInput_activated(const QString &arg1) {
  QString url =
      "http://glslsandbox.com/item/" + arg1.mid(arg1.indexOf('#') + 1);
  manager.get(QNetworkRequest(url));
}

void MainWindow::on_saveToFileBtn_clicked() {
  if (!ui->urlInput->count()) return;
  QVariant v = ui->urlInput->currentData();
  auto shader = v.value<QByteArray>();
  QString url = ui->urlInput->currentText();
  url = "shaders/" + url.mid(url.indexOf('#') + 1) + ".glsl";
  QFile file(url);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QJsonDocument doc = QJsonDocument::fromJson(shader);
  file.write(doc.object().value("code").toString().toUtf8());
  file.close();
  readShaders();
}
