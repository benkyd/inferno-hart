#include "window.hpp"

#include "spdlog/spdlog.h"
#include "gl.h"

using namespace core;

Window::Window(std::string title, int width, int height) 
{
    this->width = width;
    this->height = height;
    setupGLFW(title);
    setupImGui();
}

Window::~Window() 
{
    shutdownImGui();
    shutdownGLFW();    
}

void Window::setTitle(std::string title) 
{
    glfwSetWindowTitle(window, title.c_str());
}

void Window::setSize(int w, int h) 
{
    width = w;
    height = h;
    glfwSetWindowSize(window, width, height);
}

void Window::setPos(int x, int y) 
{
    glfwSetWindowPos(window, x, y);
}

void Window::getSize(int& w, int& h) 
{
    w = width;
    h = height;
}

void Window::getPos(int& x, int& y) 
{
    glfwGetWindowPos(window, &x, &y);
}

bool Window::newFrame() 
{
    glfwPollEvents();
    if (glfwWindowShouldClose(window)) { return false; }

    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // removed this for now, draws semitransparent over opengl
    ImGui::Begin("Main", NULL, WINDOW_FLAGS);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(width, height));

    return true;
}

void Window::render() 
{
    ImGui::End();
    ImGui::Render();
    auto io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    ImGui::UpdatePlatformWindows();
}

void Window::setupGLFW(std::string title) 
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glslVersion = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glslVersion = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, title.c_str(), NULL, NULL);
    if (window == NULL) throw std::runtime_error("Could not create window");
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
}

void Window::setupImGui() 
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
    // io.ConfigDockingWithShift = true;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void Window::shutdownImGui() 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Window::shutdownGLFW() 
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::glfwErrorCallback(int error, const char* description) {
    spdlog::error("[GLFW {0}] {1}", error, description);
}
