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

//        glm::mat4 mat4(){
//            auto transform = glm::translate(glm::mat4{1.0f}, translation);
//            transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
//            transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
//            transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});
//            transform = glm::scale(transform, scale);
//            return transform;
//        }
        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4() {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{
                    {
                            scale.x * (c1 * c3 + s1 * s2 * s3),
                                    scale.x * (c2 * s3),
                                                   scale.x * (c1 * s2 * s3 - c3 * s1),
                                                                  0.0f,
                    },
                    {
                            scale.y * (c3 * s1 * s2 - c1 * s3),
                                    scale.y * (c2 * c3),
                                                   scale.y * (c1 * c3 * s2 + s1 * s3),
                                                                  0.0f,
                    },
                    {
                            scale.z * (c2 * s1),
                                    scale.z * (-s2),
                                                   scale.z * (c1 * c2),
                                                                  0.0f,
                    },
                    {translation.x, translation.y, translation.z, 1.0f}};
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
