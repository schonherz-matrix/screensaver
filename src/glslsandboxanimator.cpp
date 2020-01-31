#include "glslsandboxanimator.h"

#include <QDir>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>

GLSLSandboxAnimator::GLSLSandboxAnimator(QObject *parent)
    : OpenGLAnimator(parent) {
  compile(":/fragment.glsl", "Default", true);

  m_currentProgram = m_programs["Default"];
  m_currentProgram->bind();
}

void GLSLSandboxAnimator::saveToDisk(QUrl url) {
  if (!url.isValid() || url.isRelative() ||
      !m_programs.contains(url.toString()))
    return;

  auto id = url.toString();

  for (auto shader : m_programs[id]->shaders()) {
    if (shader->shaderType() == QOpenGLShader::Fragment) {
      QFile file("shaders/glslsandbox.com/" + url.fragment() + ".glsl");

      if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

      file.write(shader->sourceCode());
    }
  }
}

QStringList GLSLSandboxAnimator::localShaders() {
  QDir dir("shaders/glslsandbox.com");
  auto list = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
  list.prepend("Default");

  return list;
}

QImage GLSLSandboxAnimator::nextFrame() {
  m_currentProgram->setUniformValue("time",
                                    static_cast<float>(m_time++ / m_speed));
  m_currentProgram->setUniformValue("resolution", 32.0f, 26.0f);
  m_currentProgram->setUniformValue("mouse", 0.0f, 0.0f);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  return m_fbo->toImage();
}

bool GLSLSandboxAnimator::readFromOnline(QUrl url) {
  if (!url.hasFragment() || url.fileName() != "e") return false;

  m_currentUrl = url;
  m_manager.get(
      QNetworkRequest(QUrl("http://glslsandbox.com/item/" + url.fragment())));

  QEventLoop loop;
  connect(&m_manager, &QNetworkAccessManager::finished, &loop,
          &QEventLoop::quit);
  loop.exec();

  return true;
}

bool GLSLSandboxAnimator::readFromLocal(QFile file) {
  if (file.fileName() == "Default") return true;

  return compile("shaders/glslsandbox.com/" + file.fileName(), file.fileName(),
                 true);
}

void GLSLSandboxAnimator::replyFinished(QNetworkReply *reply) {
  QByteArray data = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);

  auto code = doc.object().value("code").toString();
  compile(code, m_currentUrl.toString());

  reply->deleteLater();
}

bool GLSLSandboxAnimator::compile(const QString file, const QString id,
                                  bool isFile) {
  auto prog = std::make_shared<QOpenGLShaderProgram>(this);

  prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.glsl");

  if (isFile) {
    prog->addShaderFromSourceFile(QOpenGLShader::Fragment, file);
  } else {
    auto shader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    shader->compileSourceCode(file);
    prog->addShader(shader);
  }

  if (!prog->link()) return false;

  prog->enableAttributeArray("position");
  prog->setAttributeBuffer("position", GL_FLOAT, 0, 3);

  m_programs[id] = prog;

  return true;
}
