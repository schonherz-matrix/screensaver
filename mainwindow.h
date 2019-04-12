#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_urlLineEdit_returnPressed();
  void readShaders();
  void readSandbox(QNetworkReply *reply);
  void on_shaderLoadButton_clicked();
  void on_pushButton_clicked();

 private:
  Ui::MainWindow *ui;
  QNetworkAccessManager manager;
};

#endif  // MAINWINDOW_H
