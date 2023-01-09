#include "window.hpp"

#include "gui/style.hpp"

#include "spdlog/spdlog.h"

using namespace inferno;

Window::Window() 
{

}

Window::~Window() 
{
    shutdownImGui();
    shutdownGLFW();    
}

void Window::init(std::string title, int width, int height)
{
    this->width = width;
    this->height = height;
    setupGLFW(title);

    glfwSetKeyCallback(getGLFWWindow(), glfwKeyCallback);

    setupImGui();
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

glm::vec2 Window::getSize() 
{
    return {width, height};
}

void Window::getPos(int& x, int& y) 
{
    glfwGetWindowPos(window, &x, &y);
}

void Window::setFPSMode()
{
    mWinMode = WIN_MODE_FPS;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Window::setKeyCallback(KeyCallback callback)
{
    mKeyCallback = callback;
}

KeyCallback Window::getKeyCallback()
{
    return mKeyCallback;
}

bool Window::newFrame() 
{
    glfwPollEvents();
    if (mWinMode == WIN_MODE_FPS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(window, (double)width / 2, (double)height / 2);
    }
    if (glfwWindowShouldClose(window)) { return false; }

    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("main", nullptr, WINDOW_FLAGS);
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
    // GL 4.5 + GLSL 450
    glslVersion = "#version 450";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
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

    SetupImGuiStyle2();
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

void Window::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (Window::GetInstance().getKeyCallback() != nullptr)
    {
        Window::GetInstance().getKeyCallback()(key, scancode, action, mods);
    }
}

void Window::glfwErrorCallback(int error, const char* description) {
    spdlog::error("[GLFW {0}] {1}", error, description);
}
