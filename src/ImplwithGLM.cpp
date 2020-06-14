

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

VSShaderLib texture_shader;
VSShaderLib light_shader;

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = -143.0f, beta = 28.0f;
float r = 5.25f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];

glm::mat4 pvm, pv, pers, view, model;
glm::mat4 *another_pvm = new glm::mat4();

GLuint cube_vao;
GLuint plain_vao;

int plain_facecount;

glm::vec3 light_pos = vec3(10.0, 8.0, 11.0);

// mesh vector for many meshes
std::vector<Mesh *> meshes;

TransformationData cubeTransformOne, cubeTransformTwo, cubeTransformThree, cubeTransformFour, sunTransform;

GLuint textureCube, textureSphere;

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

void processKeys(unsigned char key, int xx, int yy)
{
    switch (key)
    {

    case 27:
        glutLeaveMainLoop();
        break;

    case 'c':
        printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
        break;
    }

    //  uncomment this if not using an idle func
    //	glutPostRedisplay();
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

        alphaAux = alpha;
        betaAux = beta;
        rAux = r + (deltaY * 0.01f);
        if (rAux < 0.1f)
            rAux = 0.1f;
    }

    camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
    camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
    camY = rAux * sin(betaAux * 3.14f / 180.0f);
    view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    //  uncomment this if not using an idle func
    //	glutPostRedisplay();
}

GLuint setupShaders()
{

    texture_shader.init();
    texture_shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/another_color.vert");
    texture_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/another_color.frag");

    // set semantics for the shader variables
    texture_shader.setProgramOutput(0, "outputF");
    texture_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
    texture_shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "normal");
    texture_shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "uvs");

    texture_shader.prepareProgram();

    texture_shader.setUniform("another_pvm", &pvm);

    light_shader.init();
    light_shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/StandardShading.vert");
    light_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/StandardShading.frag");

    // set semantics for the shader variables
    light_shader.setProgramOutput(0, "outputF");
    light_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "vertexPosition_modelspace");
    light_shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "vertexNormal_modelspace");
    light_shader.setVertexAttribName(VSShaderLib::VERTEX_ATTRIB1, "vertexColor");

    light_shader.prepareProgram();

    if (light_shader.isProgramValid())
    {
        std::cout << "#####################" << light_shader.getProgramIndex() << std::endl;
    }

    return (light_shader.isProgramValid());
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
    camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
    camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
    camY = r * sin(beta * 3.14f / 180.0f);
    view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // some GL settings
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // create the VAO
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);

    // create buffers for our vertex data
    GLuint buffers[4];
    glGenBuffers(4, buffers);

    //vertex coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
    glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

    //texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::TEXTURE_COORD_ATTRIB);
    glVertexAttribPointer(VSShaderLib::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

    //normals buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
    glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

    //index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndex), faceIndex, GL_STATIC_DRAW);

    // unbind the VAO
    glBindVertexArray(0);

    plain_facecount = generateHills(&plain_vao);

    textureCube = loadBMP_custom("test1.bmp");
    textureSphere = loadBMP_custom("sun.bmp");
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    // load identity matrices

    // use our shader
    glUseProgram(texture_shader.getProgramIndex());

    // draws the mesh
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureCube);
    texture_shader.setUniform("myTextureSampler", 0);

    pv = pers * view;

    pvm = (pv * cubeTransformOne.calculateTransformationMatrix());

    texture_shader.setUniform("pvm", &pvm);
    meshes[0]->drawStuff();

    pvm = (pv * cubeTransformTwo.calculateTransformationMatrix());

    texture_shader.setUniform("pvm", &pvm);
    meshes[0]->drawStuff();

    pvm = (pv * cubeTransformThree.calculateTransformationMatrix());

    texture_shader.setUniform("pvm", &pvm);
    meshes[0]->drawStuff();

    pvm = (pv * cubeTransformFour.calculateTransformationMatrix());

    texture_shader.setUniform("pvm", &pvm);
    meshes[0]->drawStuff();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureSphere);
    model_two[3][2] = sin(mover) * 20;
    model_two[3][0] = cos(mover) * 20;

    mat4 trans = sunTransform.calculateTransformationMatrix();

    pvm = ((pv * trans) * model_two);

    texture_shader.setUniform("pvm", &pvm);
    meshes[1]->drawStuff();

    model_two[3][2] = sin(mover) * 10;
    model_two[3][0] = cos(mover) * 10;

    glUseProgram(light_shader.getProgramIndex());

    pvm = (pv);
    light_shader.setUniform("PVM", &pvm);
    light_shader.setUniform("M", &model_two);
    light_shader.setUniform("V", &view);
    light_shader.setUniform("LightPosition_worldspace", &light_pos);

    glBindVertexArray(plain_vao);
    glDrawArrays(GL_TRIANGLES, 0, plain_facecount * 3);

    glutPostRedisplay();
    glutSwapBuffers();

    mover += 0.01f;
}

void mouseWheel(int wheel, int direction, int x, int y)
{

    r += direction * 0.1f;
    if (r < 0.1f)
        r = 0.1f;

    camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
    camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
    camY = r * sin(beta * 3.14f / 180.0f);

    view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    //  uncomment this if not using an idle func
    //	glutPostRedisplay();
}

// ------------------------------------------------------------
//
// Main function
//

int main(int argc, char **argv)
{
    model = mat4(1.0);
    cubeTransformOne.angle = (0.0f);
    cubeTransformOne.position = vec3(8.f, 0.f, 1.f);

    cubeTransformTwo.angle = (0.f);
    cubeTransformTwo.position = vec3(1.f, 0.f, 8.f);

    cubeTransformThree.angle = (0.f);
    cubeTransformThree.position = vec3(8.f, 0.f, 8.f);

    cubeTransformFour.angle = (1.568f);
    cubeTransformFour.position = vec3(1.f, 0.f, 1.f);

    sunTransform.angle = (3.f);
    sunTransform.position = vec3(10.f, 8.f, 11.f);

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
    glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    glutMouseWheelFunc(mouseWheel);
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

    initModels("cube.obj");
    initModels("sphere.obj");
    initOpenGL();

    //initVSL();

    //  GLUT main loop
    glutMainLoop();

    return (0);
}
