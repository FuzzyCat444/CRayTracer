#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RayTracingEngine.h"
#include "Mat4.h"
#include "Vec3.h"
#include "MathFunctions.h"

void closeCallback(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoveCallback(GLFWwindow* window, double dx, double dy);

void fatalError(char *str);

GLuint loadShaders(const char *vertexShader, const char *fragmentShader);

int main(int argc, char **argv)
{
    srand(time(NULL));

    if (!glfwInit())
    {
        fatalError("Failed to initialize GLFW.");
    }

    const int width = 1152;
    const int height = 864;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Ray Tracing Engine", NULL, NULL);
    if (!window)
    {
        fatalError("Failed to create GLFW window.");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    // Callbacks
    glfwSetWindowCloseCallback(window, closeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // OpenGL init crap
    if (!gladLoadGL())
    {
        fatalError("Failed to load OpenGL (glad).");
    }
    glViewport(0, 0, width, height);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    const GLfloat screenVerts[] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof screenVerts, screenVerts, GL_STATIC_DRAW);
    GLuint shader = loadShaders("#version 330 core\n"
                                "in vec3 vertexPosition_modelspace;\n"
                                "in vec2 vertexUV;\n"
                                "out vec2 UV;\n"
                                "void main() {\n"
                                "  gl_Position.xyz = vertexPosition_modelspace;\n"
                                "  gl_Position.w = 1.0;\n"
                                "  UV = vertexUV;\n"
                                "}",
                                "#version 330 core\n"
                                "in vec2 UV;\n"
                                "out vec3 color;\n"
                                "uniform sampler2D tex;\n"
                                "void main(){\n"
                                "  color = texture(tex, UV).rgb;\n"
                                "}");
    GLint texUniform = glGetUniformLocation(shader, "tex");

    RayTracingEngine *engine = RayTracingEngine_create(width, height, 5, 70.0f);
    if (!engine)
    {
        fatalError("Failed to create ray tracing engine.");
    }
    glfwSetWindowUserPointer(window, engine);

    Scene *scene = RayTracingEngine_getScene(engine);
    Scene_addPointLight(scene, (Vec3) {0.0f, 5.0f, 0.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, 20.0f);
    Scene_addPointLight(scene, (Vec3) {0.0f, 0.0f, 0.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, 5.0f);
    Scene_addPlane(scene, (Vec3) {0.0f, -1.0f, 0.0f}, 10.0f, 10.0f, 0.0f, 0.0f, Material_create((Vec3) {1.0f, 1.0f, 1.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, 0.0f));
    Scene_addPlane(scene, (Vec3) {-5.0f, 4.0f, 0.0f}, 10.0f, 10.0f, M_PI * 0.5f, M_PI * 0.5f, Material_create((Vec3) {1.0f, 1.0f, 1.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, 0.5f));

    float sd = 1.1f;
    float spec = 0.2f;
    Scene_addSphere(scene, (Vec3) {-sd, 0.0f, sd}, 1.0f, Material_create((Vec3) {1.0f, 0.0f, 0.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, spec));
    Scene_addSphere(scene, (Vec3) {sd, 0.0f, sd}, 1.0f, Material_create((Vec3) {0.0f, 1.0f, 0.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, spec));
    Scene_addSphere(scene, (Vec3) {-sd, 0.0f, -sd}, 1.0f, Material_create((Vec3) {0.0f, 0.0f, 1.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, spec));
    Scene_addSphere(scene, (Vec3) {sd, 0.0f, -sd}, 1.0f, Material_create((Vec3) {1.0f, 1.0f, 0.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, spec));

    Scene_addSphere(scene, (Vec3) {-3.1f, 0.0f, 0.0f}, 0.7f, Material_create((Vec3) {1.0f, 1.0f, 1.0f}, (Vec3) {1.0f, 1.0f, 1.0f}, 0.9f));

    while (!glfwWindowShouldClose(window))
    {
        // Events
        glfwPollEvents();

        // Updates/Ray Trace Rendering
        RayTracingEngine_simulate(engine);

        // Render buffer tuning
        Framebuffer *renderBuffer = RayTracingEngine_getRenderBuffer(engine);

        // OpenGL rendering
        glUseProgram(shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texUniform, 0);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Framebuffer_getWidth(renderBuffer), Framebuffer_getHeight(renderBuffer), GL_RGB, GL_UNSIGNED_BYTE, Framebuffer_getPixels(renderBuffer));
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
        glDrawArrays(GL_QUADS, 0, 4);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader);

    RayTracingEngine_destroy(engine);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void closeCallback(GLFWwindow *window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    RayTracingEngine *eng = glfwGetWindowUserPointer(window);
    float moveSpeed = 0.25f;
    if (key == GLFW_KEY_W)
    {
        RayTracingEngine_moveCameraForward(eng, moveSpeed);
    }
    else if (key == GLFW_KEY_S)
    {
        RayTracingEngine_moveCameraForward(eng, -moveSpeed);
    }
    else if (key == GLFW_KEY_D)
    {
        RayTracingEngine_moveCameraRight(eng, moveSpeed);
    }
    else if (key == GLFW_KEY_A)
    {
        RayTracingEngine_moveCameraRight(eng, -moveSpeed);
    }
    else if (key == GLFW_KEY_E)
    {
        RayTracingEngine_moveCameraUp(eng, moveSpeed);
    }
    else if (key == GLFW_KEY_Q)
    {
        RayTracingEngine_moveCameraUp(eng, -moveSpeed);
    }
}

void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
    static double oldX = 9999.0f;
    static double oldY = 0.0f;
    float dx = (float) (x - oldX);
    float dy = (float) (y - oldY);
    if (oldX == 9999.0f)
    {
        dx = 0.0f;
        dy = 0.0f;
    }
    RayTracingEngine *eng = glfwGetWindowUserPointer(window);
    //glfwSetCursorPos(window, RayTracingEngine_getWidth(eng) / 2, RayTracingEngine_getHeight(eng) / 2);
    float rotSpeed = 0.01f;
    RayTracingEngine_moveCamera(eng, (Vec3){0.0f}, dx * rotSpeed, dy * rotSpeed);

    oldX = x;
    oldY = y;
}

void fatalError(char *str)
{
    printf("Fatal Error: %s", str);
    glfwTerminate();
    exit(1);
}

// Stolen and modified from: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
GLuint loadShaders(const char *vertexShader, const char *fragmentShader)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = vertexShader;
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Compile Fragment Shader
	char const * FragmentSourcePointer = fragmentShader;
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
