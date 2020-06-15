

#include <math.h>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/vec2.hpp>                 // glm::vec2
#include <glm/vec3.hpp>                 // glm::vec3
#include <glm/vec4.hpp>                 // glm::vec4
#include <glm/mat4x4.hpp>               // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "texture.hpp"

#include "PlainGenerator.hpp"
#include "Mesh.hpp"

#include "vsShaderLib.h"
#include "shaderDemo.h"

#include "Noise2d.hpp"

VSShaderLib color_light_shader, texture_light_shader;

// Camera Position
glm::vec3 cam = vec3(0.f);
glm::vec3 direction = vec3(0.f);

// Control
mat3 rotationmatrixControl;
glm::vec3 rotationaxes = vec3(0.f, 1.0, 0.f);
float angle = 3.14f / 2;
glm::vec3 left, right;
bool forward, backward, goleft, goright = false;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = -312.0f, beta = 32.0f;
float r = 5.25f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];

glm::mat4 pvm, pv, pers, view;

GLuint cube_vao;
GLuint plain_vao;

int plain_facecount;

glm::vec3 light_center = vec3(10.0, 8.0, 11.0);
float light_radius = 5.0;

// mesh vector for many meshes
std::vector<Mesh *> meshes;

TransformationData cubeTransformOne, cubeTransformTwo, cubeTransformThree, cubeTransformFour, sunTransform;
TransformationData treeTransforms[10];

std::vector<TransformationData> TanneTD;
std::vector<TransformationData> BaumTD;

GLuint textureBaum, textureTanne, textureSphere;

float mover = 0.0f;

glm::mat4 model_two = glm::mat4(1.0);

void changeSize(int w, int h)
{

    float ratio;
    // Prevent a divide by zero, when window is too short
    if (h == 0)
        h = 1;
    // set the viewport to be the entire window
    glViewport(0, 0, w, h);
    // set the projection matrix
    ratio = (1.0f * w) / h;
    pers = glm::perspective(glm::radians(53.13f), ratio, 0.1f, 1000.f);
}

void processMouseButtons(int button, int state, int xx, int yy)
{
    // start tracking the mouse
    if (state == GLUT_DOWN)
    {
        startX = xx;
        startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            tracking = 2;
    }

    //stop tracking the mouse
    else if (state == GLUT_UP)
    {
        if (tracking == 1)
        {
            alpha -= (xx - startX);
            beta += (yy - startY);
        }
        else if (tracking == 2)
        {
            r += (yy - startY) * 0.01f;
            if (r < 0.1f)
                r = 0.1f;
        }
        tracking = 0;
    }
}

void processKeysUp(unsigned char key, int xx, int yy)
{
    switch (key)
    {

    case 27:
        glutLeaveMainLoop();
        break;

    case 'c':
        printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
        break;

    case 'w':
        forward = false;
        break;
    case 'a':
        goleft = false;
        break;
    case 's':
        backward = false;
        break;
    case 'd':
        goright = false;
        break;
    }
    // uncomment this if not using an idle func
    // glutPostRedisplay();
}

void processKeysDown(unsigned char key, int xx, int yy)
{
    switch (key)
    {

    case 27:
        glutLeaveMainLoop();
        break;

    case 'c':
        printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
        break;

    case 'w':
        forward = true;
        break;
    case 'a':
        goleft = true;
        break;
    case 's':
        backward = true;
        break;
    case 'd':
        goright = true;
        break;
    }
    // uncomment this if not using an idle func
    // glutPostRedisplay();
}

void processMouseMotion(int xx, int yy)
{

    int deltaX, deltaY;
    float alphaAux, betaAux;
    float rAux;

    deltaX = -xx + startX;
    deltaY = yy - startY;

    // left mouse button: move camera
    if (tracking == 1)
    {

        alphaAux = alpha + deltaX;
        betaAux = beta + deltaY;

        if (betaAux > 85.0f)
            betaAux = 85.0f;
        else if (betaAux < -85.0f)
            betaAux = -85.0f;
        rAux = r;
    }
    // right mouse button: zoom
    else if (tracking == 2)
    {

        alphaAux = alpha + deltaX;
        betaAux = beta + deltaY;
        rAux = r + (deltaY * 0.01f);
        if (rAux < 0.1f)
            rAux = 0.1f;
    }

    direction.x = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
    direction.z = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
    direction.y = rAux * sin(betaAux * 3.14f / 180.0f);
    view = glm::lookAt(cam, glm::vec3(cam.x + direction.x, cam.y - direction.y, cam.z + direction.z), glm::vec3(0.0f, 1.0f, 0.0f));

    //  uncomment this if not using an idle func
    //	glutPostRedisplay();
}

GLuint setupShaders()
{

    texture_light_shader.init();
    texture_light_shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/StandardTextureShading.vert");
    texture_light_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/StandardTextureShading.frag");

    // set semantics for the shader variables
    texture_light_shader.setProgramOutput(0, "outputF");
    texture_light_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "vertexPosition_modelspace");
    texture_light_shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "vertexNormal_modelspace");
    texture_light_shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "vertexUV");

    texture_light_shader.prepareProgram();

    color_light_shader.init();
    color_light_shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/StandardColorShading.vert");
    color_light_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/StandardColorShading.frag");

    // set semantics for the shader variables
    color_light_shader.setProgramOutput(0, "outputF");
    color_light_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "vertexPosition_modelspace");
    color_light_shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "vertexNormal_modelspace");
    color_light_shader.setVertexAttribName(VSShaderLib::VERTEX_ATTRIB1, "vertexColor");

    color_light_shader.prepareProgram();

    return (color_light_shader.isProgramValid());
}

void initModels(const char *path)
{
    meshes.push_back(
        new Mesh(path,
                 glm::vec4(1.f, 0.f, 0.f, 0.f),
                 glm::vec4(0.f),
                 glm::vec4(0.f),
                 glm::vec4(1.f)));
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void initOpenGL()
{
    // set the camera position based on its spherical coordinates
    cam.x = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
    cam.z = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
    cam.y = r * sin(beta * 3.14f / 180.0f);
    view = glm::lookAt(cam, glm::vec3(cam.x + cam.x, 0.0f, cam.z + cam.z), glm::vec3(0.0f, 1.0f, 0.0f));

    // some GL settings
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    plain_facecount = generateHills(&plain_vao);

    textureTanne = loadBMP_custom("Tanne.bmp");
    textureBaum = loadBMP_custom("Baum.bmp");
    textureSphere = loadBMP_custom("sun.bmp");

    for (int i = 1; i < 30; i++)
    {
        for (int j = 1; j < 30; j++)
        {
            float x = ((float)i) / 30.0;
            float z = ((float)j) / 30.0;
            float y = instance.noise(x, z);

            if (y < 0.3)
            {
                TransformationData td = TransformationData();
                td.scale *= 0.05;
                td.angle = (float)i * j;
                td.position = vec3(x * 20.0, y, z * 20.0);
                BaumTD.push_back(td);
            }
            else if (y < 0.6)
            {
                TransformationData td = TransformationData();
                td.scale *= 0.05;
                td.angle = (float)i * j;
                td.position = vec3(x * 20.0, y, z * 20.0);
                TanneTD.push_back(td);
            }
        }
    }
}

void renderScene(void)
{
    if (forward)
    {
      cam.z += direction.z * 0.005f;
      cam.x += direction.x * 0.005f;
      cam.y -= direction.y * 0.005f;
      view = glm::lookAt(cam, glm::vec3(cam.x + direction.x, cam.y - direction.y, cam.z + direction.z), glm::vec3(0.0f, 1.0f, 0.0));
    }
    if (backward)
    {
      cam.z -= direction.z * 0.005f;
      cam.x -= direction.x * 0.005f;
      cam.y += direction.y * 0.005f;
      view = glm::lookAt(cam, glm::vec3(cam.x + direction.x, cam.y - direction.y, cam.z + direction.z), glm::vec3(0.0f, 1.0f, 0.0));
    }
    if (goleft)
    {
      rotationmatrixControl = glm::rotate(angle, rotationaxes);
      left = rotationmatrixControl * direction;
      cam.z += left.z * 0.005f;
      cam.x += left.x * 0.005f;
      view = glm::lookAt(cam, glm::vec3(cam.x + direction.x, cam.y - direction.y, cam.z + direction.z), glm::vec3(0.0f, 1.0f, 0.0));
    }
    if (goright)
    {
      rotationmatrixControl = glm::rotate(angle * 3, rotationaxes);
      right = rotationmatrixControl * direction;
      cam.z += right.z * 0.005f;
      cam.x += right.x * 0.005f;
      view = glm::lookAt(cam, glm::vec3(cam.x + direction.x, cam.y - direction.y, cam.z + direction.z), glm::vec3(0.0f, 1.0f, 0.0));
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    // load identity matrices
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureSphere);

    vec3 light_pos = light_center;
    float x = sin(mover) * light_radius;
    float z = cos(mover) * light_radius;
    light_pos.x += x;
    light_pos.z += z;

    sunTransform.position = light_pos;
    sunTransform.position.y -= 1.0;
    sunTransform.position.x -= 1.0;
    sunTransform.position.z -= 1.0;
    mat4 model = sunTransform.calculateTransformationMatrix();
    pv = pers * view;
    pvm = pv * model;

    texture_light_shader.setUniform("PVM", &pvm);
    texture_light_shader.setUniform("M", &model);
    texture_light_shader.setUniform("V", &view);
    texture_light_shader.setUniform("LightPosition_worldspace", &light_pos);
    meshes[1]->drawStuff();

    glUseProgram(color_light_shader.getProgramIndex());

    mat4 m = mat4(1.0);

    pvm = pv;
    color_light_shader.setUniform("PVM", &pvm);
    color_light_shader.setUniform("M", &m);
    color_light_shader.setUniform("V", &view);
    color_light_shader.setUniform("LightPosition_worldspace", &light_pos);

    glBindVertexArray(plain_vao);
    glDrawArrays(GL_TRIANGLES, 0, plain_facecount * 3);
    // use our shader
    glUseProgram(texture_light_shader.getProgramIndex());

    // draws the mesh
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureBaum);
    texture_light_shader.setUniform("myTextureSampler", 0);

    pv = pers * view;

    for (auto &td : BaumTD)
    {

        mat4 model = td.calculateTransformationMatrix();
        pvm = (pv * model);

        texture_light_shader.setUniform("PVM", &pvm);
        texture_light_shader.setUniform("M", &model);
        texture_light_shader.setUniform("V", &view);
        texture_light_shader.setUniform("LightPosition_worldspace", &light_pos);
        meshes[2]->drawStuff();
    }

    // draws the mesh
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureTanne);
    texture_light_shader.setUniform("myTextureSampler", 0);
    for (auto &td : TanneTD)
    {

        mat4 model = td.calculateTransformationMatrix();
        pvm = (pv * model);

        texture_light_shader.setUniform("PVM", &pvm);
        texture_light_shader.setUniform("M", &model);
        texture_light_shader.setUniform("V", &view);
        texture_light_shader.setUniform("LightPosition_worldspace", &light_pos);
        meshes[0]->drawStuff();
    }

    glutPostRedisplay();
    glutSwapBuffers();

    mover += 0.005f;
}

// ------------------------------------------------------------
//
// Main function
//

int main(int argc, char **argv)
{
    //  GLUT initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_DEBUG);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Lighthouse3D - Simple Shader Demo");

    //  Callback Registration
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

    //	Mouse and Keyboard Callbacks
    glutKeyboardFunc(processKeysDown);
    glutKeyboardUpFunc(processKeysUp);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    //	return from main loop
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    //	Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    if (!setupShaders())
        return (1);

    initModels("Tanne.obj");
    initModels("sphere.obj");
    initModels("Baum.obj");

    initOpenGL();
    //initVSL();

    //  GLUT main loop
    glutMainLoop();

    return (0);
}
