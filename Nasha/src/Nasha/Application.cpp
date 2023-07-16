#include "Application.h"

namespace Nasha{
    Application::Application() {
        loadGameObjects();
    }
    Application::~Application(){}

    void Application::run() {
        SimpleRenderSystem simpleRenderSystem{g_vkSetup, g_renderer.getSwapChainRenderPass()};
        while (!g_window.shouldClose()) {
            glfwPollEvents();
            if (auto commandBuffer = g_renderer.beginFrame()) {
                g_renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, g_gameObjects);
                g_renderer.endSwapChainRenderPass(commandBuffer);
                g_renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(g_vkSetup.device());
    }

    void Application::loadGameObjects() {
        std::vector<Model::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        auto lveModel = std::make_shared<Model>(g_vkSetup, vertices);

        auto triangle = GameObject::createGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2D.translation.x = .2f;
        triangle.transform2D.scale = {2.f, .5f};
        triangle.transform2D.rotation = .25f * glm::two_pi<float>();

        g_gameObjects.push_back(std::move(triangle));
    }
}
