#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "meshLoader.h"
#include "shader.h"
#include "resolution.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(char *filename,
	 const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                GL MANAGING                                               */
  /*----------------------------------------------------------------------------------------------------------*/

  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                CONTROLS                                                  */
  /*----------------------------------------------------------------------------------------------------------*/

  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                  VAOS                                                    */
  /*----------------------------------------------------------------------------------------------------------*/

  void createVAO();
  void drawVAO();
  void deleteVAO();

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                DRAWING                                                   */
  /*----------------------------------------------------------------------------------------------------------*/

  void drawObject(const glm::vec3 &pos,const glm::vec3 &col);
  void drawQuad();

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                SHADERS                                                   */
  /*----------------------------------------------------------------------------------------------------------*/


  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                  FBOS                                                    */
  /*----------------------------------------------------------------------------------------------------------*/

  void createFBO();
  void deleteFBO();
  void initFBO();

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                TEXTURES                                                  */
  /*----------------------------------------------------------------------------------------------------------*/

  void createTextures();
  void initTextures();
  void deleteTextures();

  /*----------------------------------------------------------------------------------------------------------*/
  /*                                                ATTRIBUTES                                                */
  /*----------------------------------------------------------------------------------------------------------*/

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  Mesh   *_mesh;   // the mesh
  Camera *_cam;    // the camera

  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion

  Shader *_shaderFirstPass; // shader used to draw geometry in the FBO
  Shader *_shaderSecondPass; // shader used to compute lighting

  /* grid */
  Grid *_grid;

  /* vaos */
  //the .h file should contain the following VAO/buffer ids
  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;

  /* fbos */
  // 1st pass: generate a heightfield (the terrain) using a procedural perlin noise.
  // 2nd pass: generate the normal map associated with the heightfield
  GLuint _fboGenerateHFAndNM;
  // 3rd pass: render the heightfield
  GLuint _fboRenderHF;
  // 5th pass: shadows
  GLuint _fboCreateShadowMap;

  /* textures */
  GLuint _texHeightMap;
  GLuint _texNormalMap;
  GLuint _texRendered;
  GLuint _texDepth;

  /* texture resolutions */
  Resolution _resTexHeightMap;
  Resolution _resTexNormalMap;
  Resolution _resTexRendered;
  Resolution _resTexDepth;
};

#endif // VIEWER_H
