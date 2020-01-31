#ifndef GLSLSANDBOXANIMATOR_H
#define GLSLSANDBOXANIMATOR_H

#include "openglanimator.h"

class GLSLSandboxAnimator : public OpenGLAnimator {
  Q_OBJECT

 public:
  explicit GLSLSandboxAnimator(QObject *parent = nullptr);

  // OpenGLAnimator interface
 public:
  void saveToDisk(QUrl url) override;
  QStringList localShaders() override;

 public slots:
  QImage nextFrame() override;

 protected:
  bool readFromOnline(QUrl url) override;
  bool readFromLocal(QFile file) override;
  void replyFinished(QNetworkReply *reply) override;

  bool compile(const QString data, const QString id, bool isFile = false);

 private:
  QUrl m_currentUrl;
};

#endif  // GLSLSANDBOXANIMATOR_H
