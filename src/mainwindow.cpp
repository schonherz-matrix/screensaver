#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(m_currentAnimator, &OpenGLAnimator::speedChanged,
          ui->speedDoubleSpinBox, &QDoubleSpinBox::setValue);
  connect(m_currentAnimator, &OpenGLAnimator::speedChanged, ui->speedSlider,
          &QSlider::setValue);

  ui->speedDoubleSpinBox->setValue(m_currentAnimator->speed());

  m_currentAnimator->cacheLocalShaders();
  ui->shaderSelection->addItems(m_currentAnimator->localShaders());

  startTimer(1000 / 60);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_shaderLoadButton_clicked() {
  for (auto shader : m_currentAnimator->localShaders()) {
    if (ui->shaderSelection->findText(shader) < 0)
      ui->shaderSelection->addItem(shader);
  }
}

void MainWindow::on_urlInput_activated(const QString &arg1) {
  m_currentAnimator->setShader(arg1);
}

void MainWindow::on_saveToFileBtn_clicked() {
  m_currentAnimator->saveToDisk(ui->urlInput->currentText());
}

void MainWindow::timerEvent(QTimerEvent *) {
  m_transmitter.sendFrame(m_currentAnimator->nextFrame());
}

void MainWindow::on_shaderSelection_activated(const QString &arg1) {
  m_currentAnimator->setShader(arg1);
}

void MainWindow::on_speedDoubleSpinBox_valueChanged(double arg1) {
  m_currentAnimator->setSpeed(arg1);
}

void MainWindow::on_speedSlider_valueChanged(int value) {
  m_currentAnimator->setSpeed(value);
}
