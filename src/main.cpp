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

    uint32_t gltfPositionIndex = model.meshes[0].primitives[0].attributes["POSITION"];
    uint32_t gltfBufferIndex = model.bufferViews[gltfPositionIndex].buffer;
    unsigned char* gltfBufferData = model.buffers[gltfBufferIndex].data.data();

    float* vertex = (float*)gltfBufferData;
    for(int i = 0; i < 9; i++){
        std::cout << vertex[i] << std::endl;
    }

    unsigned int buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), vertex, GL_STATIC_DRAW);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.0F, 1.0F, 0.0F, 1.0F);
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