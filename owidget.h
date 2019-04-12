#ifndef OWIDGET_H
#define OWIDGET_H

#include <muebtransmitter.h>
#include <QList>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>

class OWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

 public:
  OWidget(QWidget *parent = nullptr);

 public slots:
  bool compileFromSandbox(const QString data);
  bool compileFromFile(QString path);
  void setSpeed(int value);
  void setShaderProgram(int id);

 signals:
  void ready();

  // QOpenGLWidget interface
 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

 private:
  void locationInit();

  QOpenGLVertexArrayObject vao;
  QOpenGLBuffer vbo;
  QOpenGLBuffer surfaceBuffer;
  QOpenGLShaderProgram *program;
  QOpenGLBuffer buffer;
  QMap<QString, int> locations;
  GLfloat time = 0;
  MUEBTransmitter transmitter;
  int speed = 50;
  QTimer timer;
  QList<QOpenGLShaderProgram *> programs;
};

#endif  // OWIDGET_H
