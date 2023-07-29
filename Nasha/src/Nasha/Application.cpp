#include "Application.h"

#include <chrono>

namespace Nasha{
    Application::Application() {
        loadGameObjects();
    }
    Application::~Application()= default;

    void Application::run() {
        SimpleRenderSystem simpleRenderSystem{g_vkSetup, g_renderer.getSwapChainRenderPass()};
        Camera camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!g_window.shouldClose()) {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
            cameraController.moveInPlaneXZ(g_window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = g_renderer.getAspectRatio();
//            camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
            if (auto commandBuffer = g_renderer.beginFrame()) {
                g_renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, g_gameObjects, camera);
                g_renderer.endSwapChainRenderPass(commandBuffer);
                g_renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(g_vkSetup.device());
    }

    void Application::loadGameObjects() {
        std::shared_ptr<Model> model = Model::createModelFromFile(g_vkSetup, "../Nasha/src/Nasha/models/smooth_vase.obj");
        auto cube = GameObject::createGameObject();
        cube.model = model;
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = glm::vec3(3.0f);
        g_gameObjects.push_back(std::move(cube));
    }
}
