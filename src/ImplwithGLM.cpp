

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

#include "PlainGenerator.hpp"
#include "Mesh.hpp"

#include "vsShaderLib.h"
#include "shaderDemo.h"


VSShaderLib shader;
VSShaderLib another_shader;
VSShaderLib jet_another_shader;

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

glm::mat4 *pvm = new glm::mat4();
glm::mat4 *another_pvm = new glm::mat4();

GLuint vao;
GLuint another_vao;

int another_facecount;
glm::mat4 pers, view, model;

glm::vec3 light_pos = vec3(10.0, 5.0, 10.0);

// mesh vector for many meshes
std::vector<Mesh*>meshes;

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

    // Shader for models
    shader.init();
    shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/color.vert");
    shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/color.frag");

    // set semantics for the shader variables
    shader.setProgramOutput(0, "outputF");
    shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

    shader.prepareProgram();

    // this is only useful for the uniform version of the shader
    float c[4] = {1.0f, 0.8f, 0.2f, 1.0f};
    shader.setUniform("color", c);

    printf("InfoLog for Hello World Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

    another_shader.init();
    another_shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/another_color.vert");
    another_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/another_color.frag");

    // set semantics for the shader variables
    another_shader.setProgramOutput(0, "outputF");
    another_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
    another_shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "normal");
    another_shader.setVertexAttribName(VSShaderLib::VERTEX_ATTRIB1, "color");

    another_shader.prepareProgram();

    another_shader.setUniform("another_pvm", pvm);

    jet_another_shader.init();
    jet_another_shader.loadShader(VSShaderLib::VERTEX_SHADER, "./src/shaders/StandardShading.vert");
    jet_another_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "./src/shaders/StandardShading.frag");

    // set semantics for the shader variables
    jet_another_shader.setProgramOutput(0, "outputF");
    jet_another_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "vertexPosition_modelspace");
    jet_another_shader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "vertexNormal_modelspace");
    jet_another_shader.setVertexAttribName(VSShaderLib::VERTEX_ATTRIB1, "vertexColor");

    jet_another_shader.prepareProgram();

    if (jet_another_shader.isProgramValid())
    {
        std::cout << "#####################" << jet_another_shader.getProgramIndex() << std::endl;
    }

    return (shader.isProgramValid());
}

initModels(const char * path)
{
  meshes.push_back(
  new Mesh(path,
    glm::vec4(1.f, 0.f, 0.f, 0.f),
    glm::vec4(0.f),
    glm::vec4(0.f),
    glm::vec4(1.f)
  )
);
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
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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

    another_facecount = generateHills(&another_vao);

    std::cout << another_facecount << "--------------------" << another_vao << std::endl;
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // load identity matrices

    // use our shader
    glUseProgram(shader.getProgramIndex());

    *pvm = (pers * view);

    shader.setUniform("pvm", pvm);
    // send matrices to uniform buffer
    // render VAO
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);

    glUseProgram(jet_another_shader.getProgramIndex());

    // draws the mesh
    for(auto const& value: meshes)
    {
      value->drawStuff();
    }

    jet_another_shader.setUniform("PVM", pvm);
    jet_another_shader.setUniform("M", &model);
    jet_another_shader.setUniform("V", &view);
    jet_another_shader.setUniform("LightPosition_worldspace", &light_pos);
    glBindVertexArray(another_vao);
    glDrawArrays(GL_TRIANGLES, 0, another_facecount * 3);
    //swap buffers
    glutSwapBuffers();
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
    initOpenGL();

    //initVSL();

    //  GLUT main loop
    glutMainLoop();

    return (0);
}
