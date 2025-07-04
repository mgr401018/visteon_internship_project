//#include "./thirdparty/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "tiny_gltf.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    std::string gltfFilename = "../examples/gltf/01_triangle/export/triangle.gltf";

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool success = loader.LoadASCIIFromFile(&model, &err, &warn, gltfFilename);
    if(!success){
        std::cerr << "Failed to load gltf file: " << gltfFilename <<std::endl;
        std::cerr << "Error: " << err << std::endl;
        std::cerr << "Warning: " << warn << std::endl;
        return 1;
    }

    // float vertex[6] = {
    //     0.0, 0.5,
    //     -0.5, 0.0,
    //     0.5, 0.0
    // };

    // GLfloat vertex[] = {
    //     -0.5f, -0.5f, 0.0f, // positions[0]​
    //     0.0f, 0.0f, 1.0f, // normals[0]​
    //     0.25f, 0.25f, // texture coordinates[0]​
    //     0.5f, -0.5f, 0.0f, // positions[1]​
    //     0.0f, 0.0f, 1.0f, // normals[1]​
    //     0.75f, 0.25f, // texture coordinates[1]​
    //     0.0f, 0.5f, 0.0f, // positions[2]​
    //     0.0f, 0.0f, 1.0f, // normals[2]​
    //     0.5f, 0.75f // texture coordinates[2]​
    // };

    GLfloat vertex[] = {
        -0.5f, -0.5f, 0.0f, // positions[0]​
        0.5f, -0.5f, 0.0f, // positions[1]​
        0.0f, 0.5f, 0.0f, // positions[2]​
        0.0f, 0.0f, 1.0f, // normals[0]​
        0.0f, 0.0f, 1.0f, // normals[1]​
        0.0f, 0.0f, 1.0f, // normals[2]​
        0.25f, 0.25f, // texture coordinates[0]​
        0.75f, 0.25f, // texture coordinates[1]​
        0.5f, 0.75f // texture coordinates[2]​
    };

    // uint32_t gltfPositionIndex = model.meshes[0].primitives[0].attributes["POSITION"];
    // uint32_t gltfBufferIndex = model.bufferViews[gltfPositionIndex].buffer;
    // unsigned char* gltfBufferData = model.buffers[gltfBufferIndex].data.data();

    // float* vertex = (float*)gltfBufferData;
    // for(int i = 0; i < 9; i++){
    //     std::cout << vertex[i] << std::endl;
    // }

    unsigned int buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);
    
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));

    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)(9 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)(18 * sizeof(GL_FLOAT)));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //glClearColor(0.0F, 1.0F, 0.0F, 1.0F);
        glClearColor(0.251F, 0.510F, 0.427F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        // glBegin(GL_TRIANGLES);
        // glVertex2d(0.5f, 0.0f);
        // glVertex2d(0.0f, 0.5f);
        // glVertex2d(0.0f, -0.5f);
        // glEnd();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}