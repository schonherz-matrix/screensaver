#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glslsandboxanimator.h"
#include "muebtransmitter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  // QObject interface
 protected:
  void timerEvent(QTimerEvent *event) override;

 private slots:
  void on_shaderLoadButton_clicked();

  void on_urlInput_activated(const QString &arg1);

  void on_saveToFileBtn_clicked();

  void on_shaderSelection_activated(const QString &arg1);

  void on_speedDoubleSpinBox_valueChanged(double arg1);

  void on_speedSlider_valueChanged(int value);

 private:
  GLSLSandboxAnimator m_sandbox{this};
  MuebTransmitter m_transmitter{this};
  Ui::MainWindow *ui;
  OpenGLAnimator *m_currentAnimator{&m_sandbox};
};

#endif  // MAINWINDOW_H
