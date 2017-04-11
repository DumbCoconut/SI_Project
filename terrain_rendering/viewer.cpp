#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include "resolution.h"
#include <QTime>

using namespace std;

/*----------------------------------------------------------------------------------------------------------*/
/*                                              CONSTRUCTORS                                                */
/*----------------------------------------------------------------------------------------------------------*/

Viewer::Viewer(char *filename,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _mode(false),
    _resTexHeightMap(1024, 1024),
    _resTexNormalMap(1024, 1024),
    _resTexRendered(this->width(), this->height()),
    _resTexDepth(1024, 1024) {

  setlocale(LC_ALL,"C");

  // load a mesh into the CPU memory
  _mesh = new Mesh(filename);

  // create a camera (automatically modify model/view matrices according to user interactions)
  _cam  = new Camera(_mesh->radius,glm::vec3(_mesh->center[0],_mesh->center[1],_mesh->center[2]));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}


Viewer::~Viewer() {
  delete _timer;
  delete _mesh;
  delete _cam;

  /*
  for(unsigned int i=0;i<_shaders.size();++i) {
    delete _shaders[i];
  }
  */
  deleteVAO();
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                  VAOS                                                    */
/*----------------------------------------------------------------------------------------------------------*/

void Viewer::createVAO() {
  const GLfloat quadData[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f };

  glGenBuffers(2, _terrain);
  glGenBuffers(1, &_quad);
  glGenVertexArrays(1, &_vaoTerrain);
  glGenVertexArrays(1, &_vaoQuad);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER, _terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER, _grid->nbVertices() * 3 * sizeof(float), _grid->vertices(), GL_STATIC_DRAW);
  glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _grid->nbFaces() * 3 * sizeof(int), _grid->faces(), GL_STATIC_DRAW);

  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER, _quad); // vertices
  glBufferData(GL_ARRAY_BUFFER,  sizeof(quadData), quadData, GL_STATIC_DRAW);
  glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);
}

void Viewer::drawVAO() {

}

void Viewer::generateHeightField() {

}

void Viewer::generateNormalMapAssociatedToHeightFeild() {

}

void Viewer::renderHeightFeild() {

}

void Viewer::postProcess() {

}

void Viewer::shadows() {

}

void Viewer::deleteVAO() {
  glDeleteBuffers(2, _terrain);
  glDeleteBuffers(1, &_quad);
  glDeleteVertexArrays(1, &_vaoTerrain);
  glDeleteVertexArrays(1, &_vaoQuad);
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                  FBOS                                                    */
/*----------------------------------------------------------------------------------------------------------*/


void Viewer::createFBO() {
    createTextures();

    glGenFramebuffers(1, &_fboGenerateHFAndNM);
    glGenFramebuffers(1, &_fboRenderHF);
    glGenFramebuffers(1, &_fboCreateShadowMap);
}

void Viewer::initFBO() {
    initTextures();

    /* One for the 1st and 2nd pass
     *      the heightmap and normalmap could be stored in color attachements 0 et 1 of the FBO
     *      texture sizes/viewport = resolution of the grid                                                   */
    glBindFramebuffer(GL_FRAMEBUFFER, _fboGenerateHFAndNM);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texHeightMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _texNormalMap, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout << "[ERROR] \"_fboGenerateHFAndNM checks GL_FRAMEBUFFER_COMPLETE\" failed" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* One for the 3rd pass
     *      the rendered texture could be attached to color attachment 0 of the FBO
     *      you can attach other maps to extract geometric properties (deph/positions/etc) in other attachments
     *      do not forget to add a depth texture for the depth test (depth attachment)
     *      texture sizes/viewport = resolution of the window                                                 */
    glBindFramebuffer(GL_FRAMEBUFFER, _fboRenderHF);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, &_texRendered, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout << "[ERROR] \"_fboRenderHF checks GL_FRAMEBUFFER_COMPLETE\" failed" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* One for the 5th pass to create the shadow map
     *      only a depth texture needed here (depth attachment)
     *      do not need to draw into any color buffer (glDrawBuffer(GL_NONE);)
     *      texture sizes/viewport = resolution of the shadow map                                             */
    glBindFramebuffer(GL_FRAMEBUFFER, _fboCreateShadowMap);
    glDrawBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texDepth, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout << "[ERROR] \"_fboCreateShadowMap checks GL_FRAMEBUFFER_COMPLETE\" failed" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewer::deleteFBO() {
    deleteTextures();

    glDeleteFramebuffers(1, &_fboGenerateHFAndNM);
    glDeleteFramebuffers(1, &_fboRenderHF);
    glDeleteFramebuffers(1, &_fboCreateShadowMap);
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                 SHADERS                                                  */
/*----------------------------------------------------------------------------------------------------------*/

void Viewer::createShaders() {
    // load normal shader
    _shaderNormal = new Shader();
    _shaderNormal->load("shaders/normal.vert", "shaders/normal.frag");

    // load noise shader
    _shaderNoise = new Shader();
    _shaderNormal->load("shaders/noise.vert", "shaders/noise.frag");
}

void Viewer::enableShaders() {
    // Enable normal shader
    glUseProgram(_shaderNormal->id());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texHeightMap);
    glUniform1i(glGetUniformLocation(_shaderNormal->id(), "heightMap"), 0);

    // Enable noise shader
    // TODO
    /*
     * .vert
     * layout (location = 0) in vec 3 pos;
     * uniform sample 2d normedDepthMap;
     * out vec4 normedDepth;
     * vec3 p = pos;
     * vec4 d = texture ( ... );
     * p = p + d.w * ( ...)
     * normedDepth = d
     */
}


void Viewer::disableShaders() {
    delete _shaderNormal;
    delete _shaderNoise;
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                TEXTURES                                                  */
/*----------------------------------------------------------------------------------------------------------*/

void Viewer::createTextures() {
    glGenTextures(1, &_texHeightMap);
    glGenTextures(1, &_texNormalMap);
    glGenTextures(1, &_texRendered);
    glGenTextures(1, &_texDepth);
}

void Viewer::initTextures() {
    // create the texture for rendering height map
    glBindTexture(GL_TEXTURE_2D, _texHeightMap);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA32F, _resTexHeightMap.width(), _resTexHeightMap.height(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create the texture for rendering normal map
    glBindTexture(GL_TEXTURE_2D, _texNormalMap);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA32F, _resTexNormalMap.width(), _resTexNormalMap.height(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create the texture for rendering rendered
    glBindTexture(GL_TEXTURE_2D, _texRendered);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->width(), this->height(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create the texture for rendering depth
    glBindTexture(GL_TEXTURE_2D, _texDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _resTexDepth.width(), _resTexDepth.height(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Viewer::deleteTexture() {
    glDeleteTextures(1, &_texHeightMap);
    glDeleteTextures(1, &_texNormalMap);
    glDeleteTextures(1, &_texRendered);
    glDeleteTextures(1, &_texDepth);
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                CONTROLS                                                  */
/*----------------------------------------------------------------------------------------------------------*/

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  
  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    for(unsigned int i=0;i<_vertexFilenames.size();++i) {
      _shaders[i]->reload(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
    }
  }

  // space: next shader
  if(ke->key()==Qt::Key_Space) {
    _currentshader = (_currentshader+1)%_shaders.size();
  }

  updateGL();
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                GL MANAGING                                               */
/*----------------------------------------------------------------------------------------------------------*/

void Viewer::paintGL() {
  // clear the color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // tell the GPU to use this specified shader and send custom variables (matrices and others)
  enableShader(_currentshader);

  // actually draw the scene
  drawVAO();

  // tell the GPU to stop using this shader
  disableShader();
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }


  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init and load all shader files
  for(unsigned int i=0;i<_vertexFilenames.size();++i) {
    _shaders.push_back(new Shader());
    _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  }

  // VAO creation 
  createVAO();

  // starts the timer 
  _timer->start();
}
