#include "owidget.h"

#include <QDebug>
#include <QPainter>

static const GLfloat g_vertex_buffer_data[] = {
    -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0};

OWidget::OWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      vao(this),
      vbo(QOpenGLBuffer::VertexBuffer),
      surfaceBuffer(QOpenGLBuffer::VertexBuffer),
      timer(this) {
  timer.setInterval(1000 / 60);
  timer.connect(&timer, &QTimer::timeout, this, [=]() {
    update();
    transmitter.sendFrame(grabFramebuffer());
  });

  program = new QOpenGLShaderProgram(this);
  programs["Default(Sin/Cos)"] = program;
}

bool OWidget::compileFromSandbox(const QString data, const QString name) {
  bool ret;
  timer.stop();
  time = 0;
  auto prog = new QOpenGLShaderProgram(this);

  prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.glsl");
  prog->addShaderFromSourceCode(QOpenGLShader::Fragment, data);
  ret = prog->link();

  if (!ret) {
    delete prog;
    return ret;
  }

  locationInit();
  programs[name] = prog;
  program = prog;
  timer.start();

  return ret;
}

bool OWidget::compileFromFile(QString path, const QString name) {
  bool ret;
  timer.stop();
  time = 0;
  auto prog = new QOpenGLShaderProgram(this);
  prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.glsl");
  prog->addShaderFromSourceFile(QOpenGLShader::Fragment, path);
  ret = prog->link();

  if (!ret) {
    delete prog;
    return ret;
  }

  locationInit();
  programs[name] = prog;
  timer.start();

  return ret;
}

void OWidget::setSpeed(int value) { speed = value; }

void OWidget::setShaderProgram(const QString name) {
  timer.stop();
  program = programs[name];
  time = 0;
  timer.start();
}

void OWidget::initializeGL() {
  initializeOpenGLFunctions();
  vao.create();
  vao.bind();
  vbo.create();
  vbo.bind();
  vbo.allocate(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
  program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.glsl");
  program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment.glsl");
  program->link();

  // get locations
  locationInit();
  timer.start();
  emit ready();
}

void OWidget::paintGL() {
  glViewport(0, 0, 32, 26);
  program->bind();

  program->setUniformValue("time", time++ / speed);
  program->setUniformValue("resolution", width(), height());
  program->setUniformValue("mouse", 0, 0);
  program->setUniformValue("backbuffer", 0);
  program->setUniformValue("surfaceSize", 1, 1);

  vbo.bind();
  glVertexAttribPointer(locations["position"], 2, GL_FLOAT, GL_FALSE, 0,
                        nullptr);

  // actual drawing
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  program->release();
}

void OWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void OWidget::locationInit() {
  locations["position"] = program->attributeLocation("position");
  locations["time"] = program->uniformLocation("time");
  locations["mouse"] = program->uniformLocation("mouse");
  locations["resolution"] = program->uniformLocation("resolution");
  locations["texture"] = program->uniformLocation("texture");
  locations["backbuffer"] = program->uniformLocation("backbuffer");
  locations["surfaceSize"] = program->uniformLocation("surfaceSize");

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
