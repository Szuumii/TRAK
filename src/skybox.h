#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "inc.h"
#include "shader.cpp"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <string>

using namespace std;

class Skybox {
public:
    Skybox(std::vector<std::string> faces) {

        initBox();

        cubemapTexture = loadCubemap(faces);
    }


    void Render(GLuint program) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        Shader::setInt(program, "skybox", 3);
        glBindVertexArray(skyboxVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, (void *) (8 * sizeof(GLint)));
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    GLuint getSkybox() {
        return cubemapTexture;
    }

    void deallocate() {
        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, skyboxVBO);
    }

private:

    void initBox() {
        GLfloat vertices[][3] = {
                {-1, -1, -1},
                {1,  -1, -1},
                {1,  1,  -1},
                {-1, 1,  -1},
                {-1, -1, 1},
                {1,  -1, 1},
                {1,  1,  1},
                {-1, 1,  1}
        };
        GLint index[] = {
                0, 1, 3, 2, 7, 6, 4, 5,
                2, 6, 1, 5, 0, 4, 3, 7
        };

        glGenVertexArrays(1, &skyboxVAO);
        glBindVertexArray(skyboxVAO);
        glGenBuffers(2, skyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (const void *) vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), (const void *) index, GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    GLuint loadCubemap(std::vector<std::string> faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                             data);
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    GLuint skyboxVAO;
    GLuint skyboxVBO[2];
    GLuint cubemapTexture;

};

#endif