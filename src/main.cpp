//#include "./thirdparty/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "tiny_gltf.h"

struct WindowGLContext{
    GLuint vertexArrayObject;
    GLuint program;
};

struct WindowContext{
    WindowGLContext gl;
};

void loadShaders(WindowContext windowContext){
    // const char* vertexShaderSource = R"(
    //     #version 300 es

    //     layout(location = 0) in vec3 position;
    //     void main(){
    //         gl_Position = vec4(position, 1.0);        
    //     }
    // )";

    const char* vertexShaderSource = R"(
        #version 300 es

        layout(location = 0) in vec2 position;
        layout(location = 1) in vec3 color;
        out vec3 vColor;
        void main(){
            vColor = color;
            gl_Position = vec4(position, 0.0, 1.0);        
        }
    )";
    

    // const char* fragmentShaderSource = R"(
    //     #version 300 es

    //     precision mediump float;
    //     out vec4 fragColor;
    //     void main(){
    //         fragColor = vec4(0.76, 0.0, 0.0, 1.0);
    //     }
    // )";

    const char* fragmentShaderSource = R"(
        #version 300 es

        precision mediump float;
        in vec3 vColor;
        out vec4 fragColor;
        void main(){
            fragColor = vec4(vColor, 1.0);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    glCompileShader(vertexShader);
    GLint status;
    char errbuffer[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        GLint length;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
        glGetShaderInfoLog(vertexShader, 512, &length, errbuffer);
        std::cout << "Vertex shader compilation failed " << errbuffer << std::endl;
    }

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        GLint length;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
        glGetShaderInfoLog(fragmentShader, 512, &length, errbuffer);
        std::cout << "Fragment shader compilation failed " << errbuffer << std::endl;
    }

    windowContext.gl.program = glCreateProgram();
    glAttachShader(windowContext.gl.program, vertexShader);
    glAttachShader(windowContext.gl.program, fragmentShader);

    glLinkProgram(windowContext.gl.program);
    glGetProgramiv(windowContext.gl.program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE){
        GLint length;
        glGetProgramiv(windowContext.gl.program, GL_INFO_LOG_LENGTH, &length);
        glGetProgramInfoLog(windowContext.gl.program, 512, &length, errbuffer);
        std::cout << "Program linking failed " << errbuffer << std::endl;
    }

    glUseProgram(windowContext.gl.program);
}

int main(void){
    GLFWwindow* window;
    WindowContext windowContext;

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

    // GLfloat vertex[] = {
    //     -0.5f, -0.5f, 0.0f, // positions[0]​
    //     0.5f, -0.5f, 0.0f, // positions[1]​
    //     0.0f, 0.5f, 0.0f, // positions[2]​
    //     0.0f, 0.0f, 1.0f, // normals[0]​
    //     0.0f, 0.0f, 1.0f, // normals[1]​
    //     0.0f, 0.0f, 1.0f, // normals[2]​
    //     0.25f, 0.25f, // texture coordinates[0]​
    //     0.75f, 0.25f, // texture coordinates[1]​
    //     0.5f, 0.75f // texture coordinates[2]​
    // };

    GLfloat vertex[] = {
        -0.5f, -0.5f, 0.0f, // positions[0]​
        0.5f, -0.5f, 0.0f, // positions[1]​
        0.0f, 0.5f, 0.0f, // positions[2]​
        1.0f, 0.0f, 0.0f, // normals[0]​
        0.0f, 1.0f, 0.0f, // normals[1]​
        0.0f, 0.0f, 1.0f, // normals[2]​
        0.25f, 0.25f, // texture coordinates[0]​
        0.75f, 0.25f, // texture coordinates[1]​
        0.5f, 0.75f // texture coordinates[2]​
    };

    // GLfloat vertecesPositions[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f, 0.5f, 0.0f
    // };

    // GLfloat vertecesColors[] = {
    //     1.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f, 0.0f,
    //     0.0f, 0.0f, 1.0f
    // };

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

    GLuint vertexArrayObject0 = 0;
    glGenVertexArrays(1, &vertexArrayObject0);
    glBindVertexArray(vertexArrayObject0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, (void*)(9 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, /*2 * sizeof(GL_FLOAT)*/ 0, (void*)(18 * sizeof(GL_FLOAT)));

    GLuint vertexArrayObject1 = 0;
    glGenVertexArrays(1, &vertexArrayObject1);
    glBindVertexArray(vertexArrayObject1);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, (void*)(9 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, /*2 * sizeof(GL_FLOAT)*/ 0, (void*)(18 * sizeof(GL_FLOAT)));

    loadShaders(windowContext);

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
        glBindVertexArray(vertexArrayObject0);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glBindVertexArray(vertexArrayObject1);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}