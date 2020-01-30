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
  void readShaders();
  void readSandbox(QNetworkReply *reply);
  void on_shaderLoadButton_clicked();
  void on_urlInput_activated(const QString &arg1);
  void on_saveToFileBtn_clicked();

  private:
  Ui::MainWindow *ui;
  QNetworkAccessManager manager;
};

#endif  // MAINWINDOW_H
