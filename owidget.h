#ifndef OWIDGET_H
#define OWIDGET_H

#include <muebtransmitter.h>
#include <QMap>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class OWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  OWidget(QWidget *parent = nullptr);

 public slots:
  void compileShader(const QByteArray data);

  // QOpenGLWidget interface
 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

 private:
  QOpenGLVertexArrayObject vao;
  QOpenGLBuffer vbo;
  QOpenGLBuffer surfaceBuffer;
  QOpenGLShaderProgram program;
  QOpenGLBuffer buffer;
  QMap<QString, int> locations;
  GLfloat time = 0;
  MUEBTransmitter transmitter;

  // QObject interface
 protected:
  void timerEvent(QTimerEvent *event) override;
};

#endif  // OWIDGET_H
