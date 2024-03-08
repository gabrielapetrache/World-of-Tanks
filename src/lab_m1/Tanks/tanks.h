#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tanks/tanks_camera.h"
#include "components/transform.h"


namespace m1
{
    class Tank : public gfxc::SimpleScene
    {
     public:
        Tank();
        ~Tank();

        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                    : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::CameraT *camera;
        implemented::CameraT *cameraMini;
        glm::mat4 projectionMatrix;
        glm::mat4 projectionMatrixMini;

        float cameraFOV;
        float left;
        float right;
        float bottom;
        float top;
        float rotate;
        float rotateTurret;
        float projectileX, projectileY, projectileZ;
        float gameTimer;
        bool launchedProjectile;
        glm::vec3 direction;
        glm::vec3 cameraTargetPosition;
        std::vector<glm::vec3> buildingPositions;
        std::vector<glm::vec3> buildingSizes;
        std::vector<glm::vec3> enemyPositions;
        std::vector<float> timeSinceDirectionChange;
        std::vector<float> enemyHealth;
        std::vector<glm::vec3> enemyDirection;
        std::vector<bool> enemyIsAlive;
        std::vector<bool> enemyLaunchedProjectile;
        std::vector<float> enemyTimeSinceLastProjectile;
        std::vector<float> enemyProjectileX;
        std::vector<float> enemyProjectileY;
        std::vector<float> enemyProjectileZ;
        std::vector<glm::vec3> enemyProjectileDirection;
        bool gameOver;
        float tankHealth;
        float timeSince;
        int score;
        float shownScore;
        glm::mat4 staticViewMatrix = glm::mat4(1);

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color,
                              const float health);

        void AddRandomBuildings();

        void AddRandomEnemies();

        bool checkPositionBuilding(glm::vec3 position, std::vector<glm::vec3> buildingPositions,
                           std::vector<glm::vec3> buildingSizes);

        bool checkPositionEnemy(glm::vec3 position, std::vector<glm::vec3> enemyPositions);

        float generateRandomNumber();
        void launchProjectile();
        void launchEnemyProjectile(int index, float d);
    };
}   // namespace m1
