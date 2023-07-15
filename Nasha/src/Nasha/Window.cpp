#include "Window.h"

namespace Nasha{
    Window::Window(int width, int height, std::string windowName)
        :m_width{width}, m_height{height}, m_windowName{std::move(windowName)}{
        std::cout << "Initiating Window" << std::endl;
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        std::cout << "Window destroyed" << std::endl;
    }

    void Window::initWindow(){
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_window =  glfwCreateWindow(m_width,
                                     m_height,
                                     m_windowName.c_str(),
                                     nullptr,
                                     nullptr);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);

    }

    bool Window::shouldClose(){
        return glfwWindowShouldClose(m_window);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if(glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("Failed to Create a window Surface");
        }
    }

    VkExtent2D Window::getExtent() {
        return {static_cast<uint32_t>(m_width),
                static_cast<uint32_t>(m_height)};
    }

    void Window::frameBufferResizeCallback(GLFWwindow *refWindow, int width, int height) {
        auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(refWindow));
        window ->frameBufferResized = true;
        window ->m_width = width;
        window ->m_height = height;
    }
}