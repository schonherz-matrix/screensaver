#include "openglanimator.h"

#include <chrono>

static const float vertices[] = {
    1.0f,  1.0f,  0.0f,  // top right
    1.0f,  -1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f,  // bottom left
    -1.0f, 1.0f,  0.0f   // top left
};

static const unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3,  // first triangle
    1, 2, 3   // second triangle
};

OpenGLAnimator::OpenGLAnimator(QObject *parent) : QObject(parent) {
  m_context.create();
  m_surface.setFormat(m_context.format());
  m_surface.create();
  m_context.makeCurrent(&m_surface);

  m_vao.create();
  m_vao.bind();

  m_vbo.create();
  m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo.bind();
  m_vbo.allocate(vertices, sizeof(vertices));

  m_ebo.create();
  m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_ebo.bind();
  m_ebo.allocate(indices, sizeof(indices));

  m_fbo = std::make_unique<QOpenGLFramebufferObject>(32, 26);
  m_fbo->bind();

  initializeOpenGLFunctions();

  connect(&m_manager, &QNetworkAccessManager::finished, this,
          &OpenGLAnimator::replyFinished);
}

void OpenGLAnimator::setShader(const QUrl url) {
  if (!url.isValid()) return;

  QString id = url.toString();
  bool ret = true;

  if (!m_programs.contains(id)) {
    if (url.isRelative())
      ret = readFromLocal(id);
    else
      ret = readFromOnline(url);
  }

  if (ret == false) return;

  if (m_currentProgram != m_programs[id]) {
    m_currentProgram = m_programs[id];
    m_currentProgram->bind();

    m_currentShaderIdx = localShaders().indexOf(id);
    emit shaderChanged(id);
  }
}

void OpenGLAnimator::setSpeed(double value) {
  m_speed = value;
  emit speedChanged(value);
}

void OpenGLAnimator::cacheLocalShaders() {
  for (auto file : localShaders()) {
    readFromLocal(file);
  }
}

void OpenGLAnimator::startAutoPlay() {
  using namespace std::chrono_literals;

  if (localShaders().size() <= 1) return;

  m_timerId = startTimer(30s);
}

void OpenGLAnimator::stopAutoPlay() { killTimer(m_timerId); }

void OpenGLAnimator::timerEvent(QTimerEvent *) {
  auto shaders = localShaders();

  m_currentShaderIdx++;
  if (m_currentShaderIdx >= shaders.size()) m_currentShaderIdx = 0;

  setShader(shaders[m_currentShaderIdx]);
}

double OpenGLAnimator::speed() const { return m_speed; }
