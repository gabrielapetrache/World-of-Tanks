#include "lab_m1/Tanks/tanks.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tank::Tank()
{
}


Tank::~Tank()
{
}


void Tank::Init()
{
    camera = new implemented::CameraT();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    cameraMini = new implemented::CameraT();
    // make the camera watch from above
    cameraMini->Set(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

    cameraFOV = RADIANS(60);
    left = -5.0f;
    right = 5.0f;
    bottom = -3.0f;
    top = 3.0f;
    rotate = 0;
    rotateTurret = 0;
    projectileX = 0;
    projectileY = 0;
    projectileZ = 0;
    launchedProjectile = false;
    timeSince = 1.0f;
    gameTimer = 0.0f;
    score = 0;
    shownScore = 0;
    direction = glm::vec3(0, 0, 0);
    buildingPositions = std::vector<glm::vec3>();
    buildingSizes = std::vector<glm::vec3>();
    enemyPositions = std::vector<glm::vec3>();
    enemyIsAlive = std::vector<bool>();
    timeSinceDirectionChange = std::vector<float>();
    enemyLaunchedProjectile = std::vector<bool>();
    enemyTimeSinceLastProjectile = std::vector<float>();
    enemyHealth = std::vector<float>();
    enemyProjectileX = std::vector<float>();
    enemyProjectileY = std::vector<float>();
    enemyProjectileZ = std::vector<float>();
    enemyProjectileDirection = std::vector<glm::vec3>();
    enemyDirection = std::vector<glm::vec3>();
    tankHealth = 3;
    gameOver = false;
    srand(time(NULL));

    projectionMatrix = glm::perspective(cameraFOV, window->props.aspectRatio, 0.01f, 200.0f);
    cameraTargetPosition = camera->GetTargetPosition();

    glm::ivec2 resolution = window->GetResolution();
    resolution /= 2;

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tracks");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "tracks.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("body");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "body.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("head");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "head.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tun");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "tun.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader *shader = new Shader("MainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tanks", "shaders", "VertexShaderMain.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tanks", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader *shader = new Shader("TankShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tanks", "shaders", "VertexShaderTank.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tanks", "shaders", "FragmentShaderTank.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // add buildings
    AddRandomBuildings();
    AddRandomEnemies();
}

void Tank::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.698f, 0.882f, 0.91f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    resolution /= 2;
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tank::Update(float deltaTimeSeconds) {
    if (gameOver && shownScore == 0) {
        for (int i = 0; i < enemyIsAlive.size(); i++) {
            if (!enemyIsAlive[i]) {
                score += 1;
            }
        }
        printf("Score: %d\n", score);
        shownScore = 1;
    }

    gameTimer += deltaTimeSeconds;
    if (gameTimer > 60.f) {
        gameOver = 1;
    }
    glm::ivec2 resolution = window->GetResolution() / 2;
    glViewport(0, 0, resolution.x, resolution.y);
    glClear(GL_COLOR_BUFFER_BIT);

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.257f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
        RenderSimpleMesh(meshes["plane"], shaders["MainShader"], modelMatrix, glm::vec3(0.63f, 0.74f, 0.51f), 0);
    }

    // add buildings
    for (int i = 0; i < buildingPositions.size(); i++) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, buildingPositions[i]);
        modelMatrix = glm::scale(modelMatrix, buildingSizes[i]);
        if (i % 2 == 0) {
            RenderSimpleMesh(meshes["box"], shaders["MainShader"], modelMatrix, glm::vec3(0.502f, 0.498f, 0.329f), 0);
        } else {
            RenderSimpleMesh(meshes["box"], shaders["MainShader"], modelMatrix, glm::vec3(0.541f, 0.541f, 0.541f), 0);
        }
    }

    // add enemies
    for (int i = 0; i < enemyPositions.size(); i++) {
        std::vector<glm::vec3> otherEnemyPositions;
        for (int j = 0; j < enemyPositions.size(); j++) {
            if (j != i) {
                otherEnemyPositions.push_back(enemyPositions[j]);
            }
        }
        otherEnemyPositions.push_back(cameraTargetPosition);
        double rotationAngleEnemy = atan2(enemyDirection[i].x, enemyDirection[i].z);
        if (enemyIsAlive[i] && !gameOver) {
            if (timeSinceDirectionChange[i] > 5.0f) {
                glm::vec3 newDirection;
                do {
                    int directionZ = int(generateRandomNumber());
                    int directionX = int(generateRandomNumber());
                    newDirection = glm::normalize(glm::vec3(directionX, 0, directionZ));
                } while (!checkPositionBuilding(enemyPositions[i] + newDirection * deltaTimeSeconds, buildingPositions,
                                                buildingSizes) &&
                         !checkPositionEnemy(enemyPositions[i] + newDirection * deltaTimeSeconds, enemyPositions));
                enemyDirection[i] = newDirection;
                timeSinceDirectionChange[i] = 0.0f;
            } else if (checkPositionBuilding(enemyPositions[i], buildingPositions, buildingSizes) ||
                       checkPositionEnemy(enemyPositions[i], otherEnemyPositions)) {
                // if the enemy is on a building, rotate it 180 degrees
                enemyDirection[i].x = -enemyDirection[i].x;
                enemyDirection[i].z = -enemyDirection[i].z;
            } else {
                timeSinceDirectionChange[i] += deltaTimeSeconds;
            }
            rotationAngleEnemy = atan2(enemyDirection[i].x, enemyDirection[i].z);
            enemyPositions[i] -= enemyDirection[i] * deltaTimeSeconds;
        }


        // check if the enemy is close enough to the tank to launch a projectile
        float distanceToTank = glm::distance(enemyPositions[i], cameraTargetPosition);
        float enemyRotateTurret = rotationAngleEnemy;

        if (distanceToTank < 6) {
            glm::vec3 directionToTank = glm::normalize(cameraTargetPosition - enemyPositions[i]);
            float angle = glm::acos(glm::dot(glm::normalize(enemyDirection[i]), -directionToTank));
            // if the angle is negative, the enemy is on the right of the tank and the turret should rotate clockwise
            // if the angle is positive, the enemy is on the left of the tank and the turret should rotate counterclockwise
            if (glm::cross(glm::normalize(enemyDirection[i]), -directionToTank).y < 0) {
                angle = -angle;
            }
            enemyRotateTurret += angle;
            enemyProjectileDirection[i] = -directionToTank;
        }

        launchEnemyProjectile(i, deltaTimeSeconds);

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, enemyPositions[i]);
        modelMatrix = glm::rotate(modelMatrix, (float) rotationAngleEnemy, glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));

        RenderSimpleMesh(meshes["tracks"], shaders["TankShader"], modelMatrix, glm::vec3(0.812f, 0.812f, 0.812f),
                         enemyHealth[i]);
        RenderSimpleMesh(meshes["body"], shaders["TankShader"], modelMatrix, glm::vec3(0.839f, 0.651f, 0.314f),
                         enemyHealth[i]);
        {
            glm::mat4 modelMatrix2 = glm::mat4(1);
            modelMatrix2 = glm::translate(modelMatrix2, enemyPositions[i]);
            modelMatrix2 = glm::rotate(modelMatrix2, (float) enemyRotateTurret, glm::vec3(0, 1, 0));
            modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.3f));
            RenderSimpleMesh(meshes["head"], shaders["TankShader"], modelMatrix2, glm::vec3(1.0f, 0.816f, 0.486f),
                             enemyHealth[i]);
        }

        {
            glm::mat4 modelMatrix2 = glm::mat4(1);
            modelMatrix2 = glm::translate(modelMatrix2, enemyPositions[i]);
            modelMatrix2 = glm::rotate(modelMatrix2, (float) enemyRotateTurret, glm::vec3(0, 1, 0));
            modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.3f));
            RenderSimpleMesh(meshes["tun"], shaders["TankShader"], modelMatrix2, glm::vec3(0.812f, 0.812f, 0.812f),
                             enemyHealth[i]);
        }
    }

    // time since last projectile
    if (timeSince < 1.0f) {
        timeSince += deltaTimeSeconds;
    }
    // add projectile
    launchProjectile();


    // add tank
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, cameraTargetPosition);
        modelMatrix = glm::rotate(modelMatrix, (rotate), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderSimpleMesh(meshes["tracks"], shaders["TankShader"], modelMatrix, glm::vec3(0.812f, 0.812f, 0.812f), tankHealth);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, cameraTargetPosition);
        modelMatrix = glm::rotate(modelMatrix, (rotate), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderSimpleMesh(meshes["body"], shaders["TankShader"], modelMatrix, glm::vec3(0.106f, 0.271f, 0.063f), tankHealth);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, cameraTargetPosition);
        modelMatrix = glm::rotate(modelMatrix, (rotateTurret), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderSimpleMesh(meshes["head"], shaders["TankShader"], modelMatrix, glm::vec3(0.133f, 0.459f, 0.22f), tankHealth);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, cameraTargetPosition);
        modelMatrix = glm::rotate(modelMatrix, (rotateTurret), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderSimpleMesh(meshes["tun"], shaders["TankShader"], modelMatrix, glm::vec3(0.812f, 0.812f, 0.812f), tankHealth);
    }

}

void Tank::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color,
                            const float health)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    int health_loc = glGetUniformLocation(shader->program, "health");
    glUniform1f(health_loc, health);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(object_color, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = glm::perspective(cameraFOV, window->props.aspectRatio, 0.01f, 200.0f);
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

float Tank::generateRandomNumber() {
    float number = (rand() % 4000 - 2000) / 100.0f;
    // This will generate a number between -20 and 20
    if (number > -2 && number < 2) {
        // If the number is between -2 and 2, regenerate it
        return generateRandomNumber();
    }
    return number;
}

void Tank::launchEnemyProjectile(int index, float deltaTimeSeconds)
{
    float distanceToTank = glm::distance(enemyPositions[index], cameraTargetPosition);

    if ((enemyTimeSinceLastProjectile[index] > 5.f || enemyTimeSinceLastProjectile[index] == 0.0f) && enemyIsAlive[index]
        && distanceToTank < 6.0f && !gameOver){
        enemyLaunchedProjectile[index] = true;
        enemyTimeSinceLastProjectile[index] = 0.0f;
        enemyProjectileX[index] = enemyPositions[index].x;
        enemyProjectileY[index] = enemyPositions[index].y + 0.15f;
        enemyProjectileZ[index] = enemyPositions[index].z;
    }

    if (enemyLaunchedProjectile[index]) {

        for (int i = 0; i < buildingPositions.size(); i++) {
            glm::vec3 buildingPosition = buildingPositions[i];
            glm::vec3 buildingSize = buildingSizes[i];

            if (enemyProjectileX[index] > buildingPosition.x - buildingSize.x - 0.1f / 2.0f &&
                enemyProjectileX[index] < buildingPosition.x + buildingSize.x - 0.1f / 2.0f &&
                enemyProjectileZ[index] > buildingPosition.z - buildingSize.z  - 0.1f / 2.0f &&
                enemyProjectileZ[index] < buildingPosition.z + buildingSize.z - 0.1f / 2.0f) {
                enemyLaunchedProjectile[index] = false;
            }
        }

        float tankLen = 1.5f;
        glm::vec3 Dif = cameraTargetPosition - glm::vec3(enemyProjectileX[index], enemyProjectileY[index], enemyProjectileZ[index]);

        if (glm::length(Dif) < tankLen + 0.1f) {
            tankHealth--;
            if (tankHealth <= 0) {
                gameOver = 1;
            }
            enemyLaunchedProjectile[index] = false;
        }

        if (enemyProjectileX[index] > -6.0f + enemyPositions[index].x && enemyProjectileX[index] < 6.0f + enemyPositions[index].x &&
            enemyProjectileZ[index] > -6.0f + enemyPositions[index].z && enemyProjectileZ[index] < 6.0f + enemyPositions[index].z) {
            enemyProjectileX[index] -= 0.1f * enemyProjectileDirection[index].x;
            enemyProjectileZ[index] -= 0.1f *enemyProjectileDirection[index].z;
            enemyTimeSinceLastProjectile[index] += deltaTimeSeconds;
        } else {
            enemyLaunchedProjectile[index] = false;
        }

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyProjectileX[index], enemyProjectileY[index], enemyProjectileZ[index]));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderSimpleMesh(meshes["sphere"], shaders["MainShader"], modelMatrix, glm::vec3(0.271f, 0.271f, 0.188f), 0);
    }
}

void Tank::launchProjectile()
{
    if (launchedProjectile) {
        for (int i = 0; i < buildingPositions.size(); i++) {
            glm::vec3 buildingPosition = buildingPositions[i];
            glm::vec3 buildingSize = buildingSizes[i];

            if (projectileX > buildingPosition.x - buildingSize.x - 0.1f / 2.0f &&
                projectileX < buildingPosition.x + buildingSize.x - 0.1f / 2.0f &&
                projectileZ > buildingPosition.z - buildingSize.z  - 0.1f / 2.0f &&
                projectileZ < buildingPosition.z + buildingSize.z - 0.1f / 2.0f) {
                launchedProjectile = false;
            }
        }

        for (int i = 0; i < enemyPositions.size(); i++) {
            float tankLen = 1.5f;
            glm::vec3 enemyPosition = enemyPositions[i];
            glm::vec3 Dif = enemyPosition - glm::vec3(projectileX, projectileY, projectileZ);

            if (glm::length(Dif) < tankLen + 0.1f) {
                enemyHealth[i]--;
                launchedProjectile = false;
            }

            if (enemyHealth[i] == 0) {
                // make the enemy remain in place
                enemyIsAlive[i] = false;
            }
        }

        if (projectileZ > -6.0f + cameraTargetPosition.z && projectileZ < 6.0f + cameraTargetPosition.z &&
            projectileX > -6.0f + cameraTargetPosition.x && projectileX < 6.0f + cameraTargetPosition.x) {
            projectileZ -= 0.1f * direction.z;
            projectileX -= 0.1f * direction.x;
        } else {
            launchedProjectile = false;
        }

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(projectileX, projectileY, projectileZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderSimpleMesh(meshes["sphere"], shaders["MainShader"], modelMatrix, glm::vec3(0.271f, 0.271f, 0.188f), 0);
    }
}

void Tank::AddRandomBuildings()
{
    int nrBuildings = 30;
    float minDistance = 2.0f;

    for (int i = 0; i < nrBuildings; i++) {

        float x = generateRandomNumber();
        float z = generateRandomNumber();
        // size is a random number between 0.5 and 2.5
        float size = (rand() % 20 + 5) / 10.0f;
        float y = 1.257f + size / 2.0f;
        float distance = sqrt(pow(x - cameraTargetPosition.x, 2) + pow(z - cameraTargetPosition.z, 2));
        if (distance > minDistance) {
            buildingPositions.push_back(glm::vec3(x, y, z));
            buildingSizes.push_back(glm::vec3(size));
        }
    }
}

void Tank::AddRandomEnemies()
{
    int nrEnemies = 5;
    float minDistance = 6.0f;

    for (int i = 0; i < nrEnemies; i++) {
        float x, y, z;
        do {
            x = generateRandomNumber();
            z = generateRandomNumber();
            y = cameraTargetPosition.y;
        } while (checkPositionBuilding(glm::vec3(x, y, z), buildingPositions, buildingSizes) ||
                 checkPositionEnemy(glm::vec3(x, y, z), enemyPositions) ||
                 x > 17.0f || x < -17.0f || z > 17.0f || z < -17.0f);

        float distance = sqrt(pow(x - cameraTargetPosition.x, 2) + pow(z - cameraTargetPosition.z, 2));
        if (distance > minDistance) {
            enemyPositions.push_back(glm::vec3(x, y, z));
            enemyHealth.push_back(3);
            enemyDirection.push_back(glm::vec3(0, 0, 0));
            enemyIsAlive.push_back(true);
            timeSinceDirectionChange.push_back(0.0f);
            enemyLaunchedProjectile.push_back(false);
            enemyTimeSinceLastProjectile.push_back(0.0f);
            enemyProjectileX.push_back(0.0f);
            enemyProjectileY.push_back(0.0f);
            enemyProjectileZ.push_back(0.0f);
            enemyProjectileDirection.push_back(glm::vec3(0, float(cameraTargetPosition.y + 0.15f), 0));
        }
    }
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

bool Tank::checkPositionEnemy(glm::vec3 position, std::vector<glm::vec3> enemyPositions) {
    float tankLen = 1.5f;
    for (int i = 0; i < enemyPositions.size(); i++) {
        glm::vec3 enemyPosition = enemyPositions[i];
        // center of the enemy - center of the tank
        glm::vec3 Dif = enemyPosition - position;

        // If the distance between the tank and the enemy is less than tankLen, return true
        if (glm::length(Dif) < tankLen + 0.1f) {
            return true;
        }
    }
    return false;
}

bool Tank::checkPositionBuilding(glm::vec3 position, std::vector<glm::vec3> buildingPositions, std::vector<glm::vec3> buildingSizes) {
    for (int i = 0; i < buildingPositions.size(); i++) {
        glm::vec3 buildingPosition = buildingPositions[i];
        glm::vec3 buildingSize = buildingSizes[i];
        float tankLen = 1.5f;

        if (position.x > buildingPosition.x - buildingSize.x / 2.0f - tankLen / 2.0f &&
            position.x < buildingPosition.x + buildingSize.x / 2.0f + tankLen / 2.0f &&
            position.z > buildingPosition.z - buildingSize.z / 2.0f - tankLen / 2.0f &&
            position.z < buildingPosition.z + buildingSize.z / 2.0f + tankLen / 2.0f) {
            return true;
        }
    }
    return false;
}



void Tank::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 1.0f;
    if (gameOver) {
        return;
    }

    if (window->KeyHold(GLFW_KEY_W)) {
        if (!checkPositionBuilding(camera->GetTargetPosition() + camera->GetForward() * cameraSpeed * deltaTime,
                                   buildingPositions, buildingSizes) &&
            !checkPositionEnemy(camera->GetTargetPosition() + camera->GetForward() * cameraSpeed * deltaTime,
                                enemyPositions)) {
            camera->MoveForward(cameraSpeed * deltaTime);
            cameraTargetPosition = camera->GetTargetPosition();
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        rotate += cameraSpeed * deltaTime;
        rotateTurret += cameraSpeed * deltaTime;
        camera->RotateThirdPerson_OY(cameraSpeed * deltaTime);
        cameraTargetPosition = camera->GetTargetPosition();
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        if (!checkPositionBuilding(camera->GetTargetPosition() - camera->GetForward() * cameraSpeed * deltaTime,
                                   buildingPositions, buildingSizes) &&
            !checkPositionEnemy(camera->GetTargetPosition() - camera->GetForward() * cameraSpeed * deltaTime,
                                enemyPositions)) {
            camera->MoveForward(-cameraSpeed * deltaTime);
            cameraTargetPosition = camera->GetTargetPosition();
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        rotate -= cameraSpeed * deltaTime;
        rotateTurret -= cameraSpeed * deltaTime;
        camera->RotateThirdPerson_OY(-cameraSpeed * deltaTime);
        cameraTargetPosition = camera->GetTargetPosition();
    }
}


void Tank::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tank::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tank::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !gameOver)
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
        camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
    } else if (!gameOver) {
        rotateTurret -= 0.001f * deltaX;
    }
}


void Tank::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && timeSince >= 1.0f) {
        float turretLen = 0.75f;
        launchedProjectile = true;
        projectileX = cameraTargetPosition.x - turretLen * sin(rotateTurret);
        projectileY = cameraTargetPosition.y + 0.15f;
        projectileZ = cameraTargetPosition.z - turretLen * cos(rotateTurret);
        direction = glm::normalize(glm::vec3(sin(rotateTurret), 0, cos(rotateTurret)));
        timeSince = 0.0f;
    }
}


void Tank::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tank::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tank::OnWindowResize(int width, int height)
{
}
