
#include "window.hpp"
#include "gui/style.hpp"
#include "yolo/yolo.hpp"
#include <GLFW/glfw3.h>

namespace inferno::graphics {

static WINDOW_MODE WinMode = WINDOW_MODE::WIN_MODE_DEFAULT;
static KeyCallback UserKeyCallback = nullptr;
static int Width, Height;
static const char* GlslVersion;
static GLFWwindow* Window;

void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
    int action, int mods)
{
    if (UserKeyCallback != nullptr) {
        UserKeyCallback(key, scancode, action, mods);
    }
}

void glfwErrorCallback(int error, const char* description)
{
    yolo::error("[GLFW {}] {}", error, description);
}

void setupGLFW(std::string title)
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    uint32_t ext = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ext, nullptr);
    yolo::info("Vulkan {} extensions supported", ext);

    // Create window with graphics context
    Window = glfwCreateWindow(1280, 720, title.c_str(), NULL, NULL);
    if (Window == NULL)
        throw std::runtime_error("Could not create window");
    glfwMakeContextCurrent(Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
    yolo::info("GLFW {} initialized", glfwGetVersionString());
    yolo::info("OpenGL {} initialized", glGetString(GL_VERSION));
    yolo::info("GLSL {} initialized", glGetString(GL_SHADING_LANGUAGE_VERSION));
    yolo::info("INFERNO HART Running on ", glGetString(GL_RENDERER));
}

void setupImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts; // FIXME: THIS CURRENTLY DOESN'T
                                                            // WORK AS EXPECTED. DON'T USE IN
                                                            // USER APP!
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME:
                                                                // io.ConfigDockingWithShift
                                                                // = true;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init(GlslVersion);

    inferno::SetupImGuiStyle2();
}

void shutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void shutdownGLFW()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void window_create(std::string title, int width, int height)
{
    Width = width;
    Height = height;
    setupGLFW(title);
    glfwSetKeyCallback(Window, glfwKeyCallback);
    setupImGui();
}

void window_cleanup()
{
    shutdownImGui();
    shutdownGLFW();
}

void window_set_title(std::string title)
{
    glfwSetWindowTitle(Window, title.c_str());
}

void window_set_size(int w, int h)
{
    Width = w;
    Height = h;
    glfwSetWindowSize(Window, Width, Height);
}

void window_set_pos(int x, int y) { glfwSetWindowPos(Window, x, y); }

glm::vec2 window_get_size() { return { Width, Height }; }

void window_get_pos(int& x, int& y) { glfwGetWindowPos(Window, &x, &y); }

GLFWwindow* window_get_glfw_window() { return Window; }

void window_set_mode(WINDOW_MODE mode)
{
    WinMode = mode;
    if (mode == WINDOW_MODE::WIN_MODE_FPS) {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void window_set_key_callback(KeyCallback callback) { UserKeyCallback = callback; }

KeyCallback window_get_key_callback() { return UserKeyCallback; }

bool window_new_frame()
{
    glfwPollEvents();
    if (WinMode == WIN_MODE_FPS) {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(Window, (double)Width / 2, (double)Height / 2);
    }
    if (glfwWindowShouldClose(Window)) {
        return false;
    }

    glfwGetWindowSize(Window, &Width, &Height);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("main", nullptr, WINDOW_FLAGS);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(Width, Height));

    return true;
}

void window_render()
{
    ImGui::End();
    ImGui::Render();
    auto io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(Window);
    ImGui::UpdatePlatformWindows();
}

}
