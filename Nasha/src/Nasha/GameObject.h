#pragma once

#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace Nasha{
//    struct Transform2DComponent{
//        glm::vec2 translation{};
//        glm::vec2 scale{1.f, 1.f};
//        float rotation;
//
//        glm::mat2 mat2() {
//            const float s = glm::sin(rotation);
//            const float c = glm::cos(rotation);
//            glm::mat2 rotMatrix{{c, s}, {-s, c}};
//
//            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
//            return rotMatrix * scaleMat;
//        }
//    };

    struct TransformComponent{
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        glm::mat4 mat4(){
            auto transform = glm::translate(glm::mat4{1.0f}, translation);
            transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
            transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
            transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});
            transform = glm::scale(transform, scale);
            return transform;
        }
    };

    class GameObject {
    public:
        using id_t = unsigned int;

        static GameObject createGameObject(){
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        GameObject(const GameObject&) = delete;
        GameObject &operator=(const GameObject&) = delete;
        GameObject (GameObject&&) = default;
        GameObject &operator=(GameObject&&) = default;

        [[nodiscard]] id_t gameId() const { return m_id; }

    private:
        explicit GameObject(id_t id) : m_id{id} {}

    public:
        std::shared_ptr<Model> model{};
        glm::vec3 color{};
//        Transform2DComponent transform2D{};
        TransformComponent transform{};

    private:
        id_t m_id;
    };
}
