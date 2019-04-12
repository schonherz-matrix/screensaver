#include "owidget.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QVariantMap>

static const GLfloat g_vertex_buffer_data[] = {
    -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0};

OWidget::OWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      vbo(QOpenGLBuffer::VertexBuffer),
      surfaceBuffer(QOpenGLBuffer::VertexBuffer),
      program(this) {
  startTimer(1000 / 60);
}

void OWidget::compileShader(const QByteArray data) {
  if (program.shaders().size() > 1) program.removeShader(program.shaders()[1]);
  QJsonDocument doc = QJsonDocument::fromJson(data);
  qDebug() << doc.object().value("code");
  program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                  doc.object().value("code").toString());
  program.link();
  locations["position"] = program.attributeLocation("position");
  locations["time"] = program.uniformLocation("time");
  locations["mouse"] = program.uniformLocation("mouse");
  locations["resolution"] = program.uniformLocation("resolution");
  locations["texture"] = program.uniformLocation("texture");
  locations["backbuffer"] = program.uniformLocation("backbuffer");
  locations["surfaceSize"] = program.uniformLocation("surfaceSize");
  // vertex
  glEnableVertexAttribArray(locations["position"]);
}

void OWidget::initializeGL() {
  initializeOpenGLFunctions();
  vao.create();
  vao.bind();
  vbo.create();
  vbo.bind();
  vbo.allocate(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
  program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                  ":/shaders/vertex.glsl");
  program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                  ":/shaders/fragment.glsl");
  program.link();
  program.bind();

  // get locations
  locations["position"] = program.attributeLocation("position");
  locations["time"] = program.uniformLocation("time");
  locations["mouse"] = program.uniformLocation("mouse");
  locations["resolution"] = program.uniformLocation("resolution");
  locations["texture"] = program.uniformLocation("texture");
  locations["backbuffer"] = program.uniformLocation("backbuffer");
  locations["surfaceSize"] = program.uniformLocation("surfaceSize");

  // vertex
  glEnableVertexAttribArray(locations["position"]);

  // fragment
  /*
  surfaceBuffer.create();
  surfaceBuffer.bind();
  //surface.height * parameters.screenWidth / parameters.screenHeight;
  surfaceBuffer.allocate(); //MOUSE related
  */
}

void OWidget::paintGL() {
  glUniform1f(locations["time"], time++ / 50);
  glUniform2f(locations["mouse"], 0, 0);
  glUniform2f(locations["resolution"], width(), height());
  glUniform1i(locations["backbuffer"], 0);
  glUniform2f(locations["surfaceSize"], 1, 1);

  vbo.bind();
  glVertexAttribPointer(locations["position"], 2, GL_FLOAT, GL_FALSE, 0,
                        nullptr);

  // actual drawing
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void OWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void OWidget::timerEvent(QTimerEvent *event) {
  update();
  transmitter.sendFrame(grabFramebuffer());
};
