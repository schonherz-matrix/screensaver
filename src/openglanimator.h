#ifndef OPENGLANIMATOR_H
#define OPENGLANIMATOR_H

#include <QFile>
#include <QImage>
#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <memory>
#include "QNetworkReply"

class OpenGLAnimator : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

 public:
  explicit OpenGLAnimator(QObject *parent = nullptr);
  virtual ~OpenGLAnimator() = default;

  virtual void saveToDisk(QUrl url) = 0;
  virtual QStringList localShaders() = 0;
  double speed() const;

 public slots:
  virtual QImage nextFrame() = 0;
  void setShader(const QUrl url);
  void setSpeed(double value);
  void cacheLocalShaders();

 signals:
  void frameChanged(QImage frame);
  void speedChanged(double speed);

 protected:
  QOffscreenSurface m_surface;
  QOpenGLContext m_context{this};
  QNetworkAccessManager m_manager{this};
  std::shared_ptr<QOpenGLShaderProgram> m_currentProgram;
  QMap<QString, std::shared_ptr<QOpenGLShaderProgram>> m_programs;
  std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
  double m_speed = 20;
  int m_time = 0;
  QOpenGLVertexArrayObject m_vao;
  QOpenGLBuffer m_vbo{QOpenGLBuffer::VertexBuffer};
  QOpenGLBuffer m_ebo{QOpenGLBuffer::IndexBuffer};

  virtual bool readFromOnline(QUrl url) = 0;
  virtual bool readFromLocal(QFile file) = 0;
  virtual void replyFinished(QNetworkReply *reply) = 0;
};

#endif  // OPENGLANIMATOR_H
