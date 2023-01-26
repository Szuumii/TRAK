#include <iostream>

#include "gl/glew.h"
#include "gl/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "file_loader.h"
#include "shader.cpp"
#include "skybox.h"
#include "camera.h"
#include "scene.cpp"
#include "frameBuffer.h"

#define PI 3.1415926
using namespace std;
const int wndWidth = 1280, wndHeight = 720;
const float cameraX = 0.0, cameraY = 2.0, cameraZ = 100.0;
const float angleXZ = -PI / 2, angleY = PI / 2;

Camera *camera = NULL;
Skybox *skybox;
Scene *scene;
FrameBuffer *framebuffer0 = NULL;
FrameBuffer *framebuffer1 = NULL;

GLuint mainProgram;
GLuint backProgram;
GLuint skyboxProgram;

//parameters
float INDEX = 1.333;

const bool SHOW_CONTEXT_MENU = true;
bool shouldLoadObj = true;
bool shouldLoadSkybox = true;
bool shouldChooseRefractionIndex = true;

std::string getProjectRoot();

const std::string PROJECT_ROOT = getProjectRoot();
FileLoader fileLoader(PROJECT_ROOT);

void exit() {
    delete framebuffer0;
    delete framebuffer1;

    delete skybox;
    delete scene;
    glutExit();
}

void init() {
    Shader::glGenProgramFromFile("back", "src/shaders/back_vs.glsl", NULL, NULL, NULL, "src/shaders/back_fs.glsl");
    Shader::glGenProgramFromFile("main", "src/shaders/main_vs.glsl", NULL, NULL, NULL, "src/shaders/main_fs.glsl");
    Shader::glGenProgramFromFile("skybox", "src/shaders/skybox_vs.glsl", NULL, NULL, NULL,
                                 "src/shaders/skybox_fs.glsl");


    if (SHOW_CONTEXT_MENU) {
        std::cout << std::endl;

        if (shouldLoadObj) {
            shouldLoadObj = false;
            fileLoader.chooseObject();
            std::cout << std::endl;
        }

        if (shouldLoadSkybox) {
            shouldLoadSkybox = false;
            fileLoader.chooseSkybox();
            std::cout << std::endl;
        }

        if (shouldChooseRefractionIndex) {
            shouldChooseRefractionIndex = false;
            fileLoader.chooseRefractionIndex();
            std::cout << std::endl;
            INDEX = fileLoader.chosenRefractionIndex;
        }

        std::cout << "Model, skybox and refraction index loaded - rendering scene...\n\n";

        std::cout << "Press:" << std::endl;
        std::cout << "0: to load different model" << std::endl;
        std::cout << "1: to load different skybox" << std::endl;
        std::cout << "2: to load different model and skybox" << std::endl;
        std::cout << "3: to choose different refraction index" << std::endl;
        std::cout << "4 or ESC: to shutdown the program" << std::endl;
    }

    scene = new Scene(fileLoader.chosenObjectPath);

    for (auto &word: fileLoader.chosenSkyboxPaths) {
        std::cout << word << std::endl;
    }

    const char *chosenSkyboxFilename[fileLoader.chosenSkyboxPaths.size()];
    int i = 0;
    for (auto &filename: fileLoader.chosenSkyboxPaths) {
        cout << filename.c_str();
        chosenSkyboxFilename[i] = filename.c_str();
        i++;
    }

    skybox = new Skybox(fileLoader.chosenSkyboxPaths);

    camera = new Camera(wndWidth, wndHeight, glm::vec3(cameraX, cameraY, cameraZ), angleXZ, angleY);

    framebuffer0 = new FrameBuffer(wndWidth, wndHeight, 2);
    framebuffer1 = new FrameBuffer(wndWidth, wndHeight, 1);

    mainProgram = Shader::container["main"];
    backProgram = Shader::container["back"];
    skyboxProgram = Shader::container["skybox"];

    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void renderBack(GLuint program) {
    glUseProgram(program);
    glm::mat4 gWorld = glm::mat4(1.0);
    glm::mat4 gViewProjectMatrix = camera->GetPerspectiveMatrix() * camera->GetViewMatrix();

    Shader::setMat4(program, "gWorld", gWorld);
    Shader::setMat4(program, "gViewProjectMatrix", gViewProjectMatrix);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getSkybox());
    Shader::setInt(program, "skybox", 3);
    scene->RenderScene(program);
}

void render(GLuint program) {
    glUseProgram(program);
    glm::mat4 gWorld = glm::mat4(1.0);
    glm::mat4 gViewProjectMatrix = camera->GetPerspectiveMatrix() * camera->GetViewMatrix();
    glm::vec3 EyePosition = camera->GetCameraPos();

    Shader::setMat4(program, "gWorld", gWorld);
    Shader::setMat4(program, "gViewProjectMatrix", gViewProjectMatrix);
    Shader::setVec3(program, "EyePosition", EyePosition);
    Shader::setFloat(program, "index", INDEX);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer0->GetColorTexture(0));
    Shader::setInt(program, "backWorldPos", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, framebuffer0->GetColorTexture(1));
    Shader::setInt(program, "backWorldNorm", 1);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getSkybox());
    Shader::setInt(program, "skybox", 3);

    scene->RenderScene(program);
}

void display() {
    glDepthFunc(GL_GREATER);
    glClearDepth(0);
    framebuffer0->Begin();
    renderBack(backProgram);
    framebuffer0->End();
    glClearDepth(1);
    glDepthFunc(GL_LESS);

    framebuffer1->Begin();
    render(mainProgram);

    mat4 gViewProjectMatrix = camera->GetPerspectiveMatrix() * mat4(mat3(camera->GetViewMatrix()));

    glUseProgram(skyboxProgram);
    Shader::setMat4(skyboxProgram, "gViewProjectMatrix", gViewProjectMatrix);
    skybox->Render(skyboxProgram);
    framebuffer1->Bilt(0);

    glutSwapBuffers();

}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            camera->Move(1);
            break;
        case 'a':
            camera->MoveXZ_LR(1);
            break;
        case 's':
            camera->Move(-1);
            break;
        case 'd':
            camera->MoveXZ_LR(-1);
            break;
        case 'z':
            INDEX += 0.01;
            break;
        case 'x':
            INDEX -= 0.01;
            break;
        case 'e':
            exit();
            break;
    }
}

void mouseButtonFunc(int button, int state, int mouseX, int mouseY) {
    camera->OnMouseMove(mouseX, mouseY);
    camera->SetMousePos(mouseX, mouseY);
    if ((state == GLUT_DOWN)) {
        camera->SetMousePos(mouseX, mouseY);
    } else if ((state == GLUT_UP)) {
        camera->SetMousePos(-1, -1);
    }
}

void mouseActiveMove(int mouseX, int mouseY) {
    camera->OnMouseMove(mouseX, mouseY);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(wndWidth, wndHeight);
    glutInitContextVersion(4, 4);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("trak-projekt");
    glutCreateMenu(NULL);
    glewExperimental = true;

    if (glewInit()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(0);
    }

    printf("OpenGL version supported by this platform (%s): \n", \
        glGetString(GL_VERSION));

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseButtonFunc);
    glutMotionFunc(mouseActiveMove);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}

std::string getProjectRoot() {
    char *val = getenv("ROOT");
    return val == NULL ? std::string("./src") : std::string(val);
}
