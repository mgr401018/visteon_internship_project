//#include "./thirdparty/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "tiny_gltf.h"

struct WindowGLContext{
    GLuint vertexArrayObject;
    GLuint program;
    GLuint indexBuffer;
    GLuint indecesCount;
    std::unordered_map<std::string, float> materialUniformFloats;
};

struct WindowContext{
    WindowGLContext gl;
};

void loadMesh(WindowContext &windowContext, tinygltf::Model& model, unsigned int meshId){
    GLuint vertexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint texCoordBuffer = 0;
    GLuint indexBuffer = 0;

    uint32_t gltfAccessorPositionIndex = model.meshes[meshId].primitives[0].attributes["POSITION"];
    uint32_t gltfAccessorNormalIndex = model.meshes[meshId].primitives[0].attributes["NORMAL"];
    uint32_t gltfAccessorTexCoordIndex = model.meshes[meshId].primitives[0].attributes["TEXCOORD_0"];
    uint32_t gltfAccessorIndicesIndex = model.meshes[meshId].primitives[0].indices;

    uint32_t gltfBufferViewPositionIndex = model.accessors[gltfAccessorPositionIndex].bufferView;
    uint32_t gltfBufferViewNormalIndex = model.accessors[gltfAccessorNormalIndex].bufferView;
    uint32_t gltfBufferViewTexCoordIndex = model.accessors[gltfAccessorTexCoordIndex].bufferView;
    uint32_t gltfBufferViewIndicesIndex = model.accessors[gltfAccessorIndicesIndex].bufferView;

    uint32_t gltfBufferIndexPosition = model.bufferViews[gltfBufferViewPositionIndex].buffer;
    uint32_t gltfBufferIndexNormal = model.bufferViews[gltfBufferViewNormalIndex].buffer;
    uint32_t gltfBufferIndexTexCoord = model.bufferViews[gltfBufferViewTexCoordIndex].buffer;
    uint32_t gltfBufferIndexIndices = model.bufferViews[gltfBufferViewIndicesIndex].buffer;

    unsigned char* gltfBufferDataPosition = model.buffers[gltfBufferIndexPosition].data.data();
    unsigned char* gltfBufferDataNormal = model.buffers[gltfBufferIndexNormal].data.data();
    unsigned char* gltfBufferDataTexCoord = model.buffers[gltfBufferIndexTexCoord].data.data();
    unsigned char* gltfBufferDataIndices = model.buffers[gltfBufferIndexIndices].data.data();

    uint32_t gltfPositionByteOffset = model.bufferViews[gltfBufferViewPositionIndex].byteOffset;
    uint32_t gltfNormalByteOffset = model.bufferViews[gltfBufferViewNormalIndex].byteOffset;
    uint32_t gltfTexCoordByteOffset = model.bufferViews[gltfBufferViewTexCoordIndex].byteOffset;
    uint32_t gltfIndicesByteOffset = model.bufferViews[gltfBufferViewIndicesIndex].byteOffset;

    uint32_t gltfPositionByteLength = model.bufferViews[gltfBufferViewPositionIndex].byteLength;
    uint32_t gltfNormalByteLength = model.bufferViews[gltfBufferViewNormalIndex].byteLength;
    uint32_t gltfTexCoordByteLength = model.bufferViews[gltfBufferViewTexCoordIndex].byteLength;
    uint32_t gltfIndicesByteLength = model.bufferViews[gltfBufferViewIndicesIndex].byteLength;    
    
    // GLfloat vertecies[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     -0.5f, 0.5f, 0.0f,
    //     0.5f, 0.5f, 0.0f
    // };

    // GLushort indices[] = {
    //     0, 1, 2,
    //     1, 2, 3
    // };

    // glGenBuffers(1, &vertexBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);

    windowContext.gl.indecesCount = gltfPositionByteLength / sizeof(GLushort);
    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, gltfPositionByteLength, gltfBufferDataPosition + gltfPositionByteOffset, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gltfIndicesByteLength, gltfBufferDataIndices + gltfIndicesByteOffset, GL_STATIC_DRAW);
    windowContext.gl.indexBuffer = indexBuffer;

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, gltfNormalByteLength, gltfBufferDataNormal + gltfNormalByteOffset, GL_STATIC_DRAW);    

    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, gltfTexCoordByteLength, gltfBufferDataTexCoord + gltfTexCoordByteOffset, GL_STATIC_DRAW);

    glGenVertexArrays(1, &windowContext.gl.vertexArrayObject);
    glBindVertexArray(windowContext.gl.vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//static draw materialSetP
static void materialSetProperty(WindowGLContext& glContext, std::string uniformName, float value)
{
    if (glContext.materialUniformFloats.find(uniformName) != glContext.materialUniformFloats.end())
    {
        glContext.materialUniformFloats[uniformName] = value;
    }
}

static void materialUpdateProperties(WindowGLContext& glContext)
{
    for (auto& uniform : glContext.materialUniformFloats)
    {
        GLint location = glGetUniformLocation(glContext.program, uniform.first.c_str());
        if (location != -1)
        {
            glUniform1f(location, uniform.second);
        }
        std::cout << "Uniform: " << uniform.first << " = " << uniform.second << std::endl;
    }
}

void loadShaders(WindowContext windowContext, tinygltf::Model& model, std::filesystem::path gltfDirectory, unsigned int materialId){
    // const char* vertexShaderSource = R"(
    //     #version 300 es

    //     layout(location = 0) in vec3 position;
    //     void main(){
    //         gl_Position = vec4(position, 1.0);        
    //     }
    // )";

    // const char* vertexShaderSource = R"(
    //     #version 300 es

    //     layout(location = 0) in vec2 position;
    //     layout(location = 2) in vec2 color;
    //     out vec2 vColor;
    //     void main(){
    //         vColor = color;
    //         gl_Position = vec4(position, 0.0, 1.0);        
    //     }
    // )";
    

    // // const char* fragmentShaderSource = R"(
    // //     #version 300 es

    // //     precision mediump float;
    // //     out vec4 fragColor;
    // //     void main(){
    // //         fragColor = vec4(0.76, 0.0, 0.0, 1.0);
    // //     }
    // // )";

    // const char* fragmentShaderSource = R"(
    //     #version 300 es

    //     precision mediump float;
    //     in vec2 vColor;
    //     out vec4 fragColor;
    //     void main(){
    //         fragColor = vec4(vColor, 0.33, 1.0);
    //     }
    // )";

    std::filesystem::path vertexShaderPath;
    std::filesystem::path fragmentShaderPath;
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    const char* defaultVertexShaderSource = R"(
        attribute vec2 position;
        void main(){
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )";

    const char* defaultFragmentShaderSource = R"(
        void main(){
            gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )";

    if(materialId < model.materials.size()){
        auto gltfMaterialExtras = model.materials[materialId].extras;
        if (gltfMaterialExtras.Has("shader")){
            auto gltfMaterialShader = gltfMaterialExtras.Get("shader");
            if(gltfMaterialShader.Has("vertex")){
                std::string gltfMaterialShaderVertex = gltfMaterialShader.Get("vertex").Get<std::string>();
                vertexShaderPath = gltfDirectory / gltfMaterialShaderVertex;
            }
            if(gltfMaterialShader.Has("fragment")){
                std::string gltfMaterialShaderFragment = gltfMaterialShader.Get("fragment").Get<std::string>();
                fragmentShaderPath = gltfDirectory / gltfMaterialShaderFragment;
            }
            if (gltfMaterialShader.Has("uniforms")){
                auto gltfUniforms = gltfMaterialShader.Get("uniforms");
                for (int uniformIdx = 0; uniformIdx < gltfUniforms.ArrayLen(); uniformIdx++){
                    auto uniform = gltfUniforms.Get(uniformIdx);
                    std::string uniformName;
                    if (uniform.Has("name")){
                        uniformName = uniform.Get("name").Get<std::string>();
                    }
                    if (uniform.Has("type")){
                        std::string type = uniform.Get("type").Get<std::string>();
                        auto uniformValue = uniform.Get("value");
                        if (type == "Float"){
                            double uniformValueFloat = uniformValue.Get(0).Get<double>();
                            windowContext.gl.materialUniformFloats[uniformName] = uniformValueFloat;
                            std::cout << "Uniforms: " << uniformName << " = " << uniformValueFloat << std::endl;
                        }

                    }
                }
            }
        }

        std::ifstream vertexShaderFile(vertexShaderPath);
        if (vertexShaderFile.is_open())
        {
            std::stringstream buffer;
            buffer << vertexShaderFile.rdbuf();
            vertexShaderSource = buffer.str();
        }

        std::ifstream fragmentShaderFile(fragmentShaderPath);
        if (fragmentShaderFile.is_open())
        {
            std::stringstream buffer;
            buffer << fragmentShaderFile.rdbuf();
            fragmentShaderSource = buffer.str();
        }
    } else {
        vertexShaderSource = defaultVertexShaderSource;
        fragmentShaderSource = defaultFragmentShaderSource;
    }

    
    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);

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

    // std::string gltfFilename = "../examples/gltf/01_triangle/export/triangle.gltf";
    // std::string gltfFilename = "../examples/gltf/03_shaders/export/shaders.gltf";
    //std::string gltfFilename = "../examples/gltf/04_suzanne/export/suzanne.gltf";
    std::string gltfFilename = "../examples/gltf/05_suzanne_uniforms/export/suzanne.gltf";
    //std::string gltfFilename = "../examples/gltf/06_shadertoy/export/shadertoy.gltf";

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

    // GLfloat vertex[] = {
    //     -0.5f, -0.5f, 0.0f, // positions[0]​
    //     0.5f, -0.5f, 0.0f, // positions[1]​
    //     0.0f, 0.5f, 0.0f, // positions[2]​
    //     1.0f, 0.0f, 0.0f, // normals[0]​
    //     0.0f, 1.0f, 0.0f, // normals[1]​
    //     0.0f, 0.0f, 1.0f, // normals[2]​
    //     0.25f, 0.25f, // texture coordinates[0]​
    //     0.75f, 0.25f, // texture coordinates[1]​
    //     0.5f, 0.75f // texture coordinates[2]​
    // };

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

    // unsigned int buffer = 0;
    // glGenBuffers(1, &buffer);
    // glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);
    
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));

    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));

    // GLuint vertexArrayObject0 = 0;
    // glGenVertexArrays(1, &vertexArrayObject0);
    // glBindVertexArray(vertexArrayObject0);

    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, 0);
    
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, (void*)(9 * sizeof(GL_FLOAT)));

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, /*2 * sizeof(GL_FLOAT)*/ 0, (void*)(18 * sizeof(GL_FLOAT)));

    // GLuint vertexArrayObject1 = 0;
    // glGenVertexArrays(1, &vertexArrayObject1);
    // glBindVertexArray(vertexArrayObject1);

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, 0);
    
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, /*3 * sizeof(GL_FLOAT)*/ 0, (void*)(9 * sizeof(GL_FLOAT)));

    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, /*2 * sizeof(GL_FLOAT)*/ 0, (void*)(18 * sizeof(GL_FLOAT)));

    
    loadMesh(windowContext, model, 0);

    std::filesystem::path gltfPath = gltfFilename;
    std::filesystem::path gltfDirectory = gltfPath.parent_path();
    loadShaders(windowContext, model, gltfDirectory, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //glClearColor(0.0F, 1.0F, 0.0F, 1.0F);
        glClearColor(0.251F, 0.510F, 0.427F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(windowContext.gl.program);

        // glBegin(GL_TRIANGLES);
        // glVertex2d(0.5f, 0.0f);
        // glVertex2d(0.0f, 0.5f);
        // glVertex2d(0.0f, -0.5f);
        // glEnd();
        
        // glBindVertexArray(vertexArrayObject0);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // glBindVertexArray(vertexArrayObject1);
        glBindVertexArray(windowContext.gl.vertexArrayObject);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, windowContext.gl.indexBuffer);
        glDrawElements(GL_TRIANGLES, windowContext.gl.indecesCount, GL_UNSIGNED_SHORT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}