#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>    // sin�֐��̂��߂�cmath�𖾎��I�ɃC���N���[�h

#include "Shader.h" // �쐬����Shader�N���X�̃w�b�_�[���C���N���[�h

// ��GLM�̃w�b�_�[���C���N���[�h��
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale �̂��߂ɕK�v
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr �̂��߂ɕK�v

// --- �֐��v���g�^�C�v�錾 ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// --- �O���[�o���ϐ� (�K�v�ɉ�����) ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float yaw = -90.0f;  // yaw: ���E�̉�]�p�B-90.0f��Z���̕������������i�����ݒ�j
float pitch = 0.0f;   // pitch: �㉺�̉�]�p
float lastX = 800.0f / 2.0; // �O��̃}�E�X�J�[�\����X���W
float lastY = 600.0f / 2.0; // �O��̃}�E�X�J�[�\����Y���W
bool firstMouse = true;   // �v���O�����J�n��̏���}�E�X���͂��ǂ����𔻒肷��t���O

// ���J�����̃O���[�o���ϐ� (�V�����ǉ�) ��
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);   // �J�����̏����ʒu (Z���ŉ���3.0f)
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);  // �J�����̎������� (Z���̕��̕����A-1.0�͎�O)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   // �J�����̏���� (Y���̐��̕���)

float deltaTime = 0.0f; // �O�̃t���[�����猻�݂̃t���[���܂ł̎���
float lastFrame = 0.0f; // �O�̃t���[���̎���

// -----------------------------------------------------------------------------
// MAIN �֐�
// -----------------------------------------------------------------------------
int main()
{
    // 1. GLFW�̏����� (�ύX�Ȃ�)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 2. �E�B���h�E�̍쐬 (�ύX�Ȃ�)
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "��������n�܂�Q�[���쐬�I�I", NULL, NULL); // �^�C�g����ύX
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. GLAD�̏����� (�ύX�Ȃ�)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 4. OpenGL�r���[�|�[�g�̐ݒ� (�ύX�Ȃ�)
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // ���}�E�X�J�[�\�����\���ɂ��A�E�B���h�E���ɌŒ肷�� (�V�����ǉ�) ��
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ���}�E�X�̓��������o����R�[���o�b�N�֐���ݒ肷�� (�V�����ǉ�) ��
    glfwSetCursorPosCallback(window, mouse_callback);

    // --- �V�F�[�_�[�̏��� ---
    // Shader�N���X�̃C���X�^���X�� (�V�F�[�_�[�t�@�C���̃p�X���w��)
    // �v���W�F�N�g�̃��[�g�f�B���N�g����shader.vs��shader.fs�����邱�Ƃ�O��
    // �����ʂ̏ꏊ�ɒu���ꍇ�́A�K�؂ȃp�X���w�肵�Ă��������B
    // ��: Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    Shader ourShader("Shaders/shader.vs", "Shaders/shader.fs");


    // ������ 8. ���_�f�[�^�̒�` (�e�O�p�`���ƂɕʁX�̔z��Œ�`) ������
    // ����ɂ��A���ꂼ��̎O�p�`�̒��_�f�[�^���ʂɊǗ��ł��܂��B
    // �ŏ��̎O�p�` (�I�����W) �̒��_�f�[�^
    float triangle1_vertices[] = {
        // �ʒu(x, y, z)          �F(r, g, b)
        -0.5f, -0.5f, 0.0f,      1.0f, 0.5f, 0.2f, // ����
         0.5f, -0.5f, 0.0f,      1.0f, 0.5f, 0.2f, // �E��
         0.0f,  0.5f, 0.0f,      1.0f, 0.5f, 0.2f  // ��
    };
    // 2�Ԗڂ̎O�p�` (��) �̒��_�f�[�^ - �Ӑ}�I�ɉE�ɂ��炵�Ă��܂�
    float triangle2_vertices[] = {
        // �ʒu(x, y, z)          �F(r, g, b)
         0.2f, -0.5f, 0.0f,      0.2f, 0.5f, 1.0f, // ����
         1.2f, -0.5f, 0.0f,      0.2f, 0.5f, 1.0f, // �E��
         0.7f,  0.5f, 0.0f,      0.2f, 0.5f, 1.0f  // ��
    };

    // ������ 9. VAO��VBO�̍쐬�Ɛݒ� (�e�O�p�`���ƂɓƗ������Z�b�g�𐶐�) ������

    // --- �ŏ��̎O�p�` (�I�����W) �p��VAO��VBO ---
    unsigned int VAO1, VBO1; // 1�Ԗڂ̎O�p�`�p��ID
    glGenVertexArrays(1, &VAO1); // VAO��1����
    glGenBuffers(1, &VBO1);     // VBO��1����

    // VAO1���o�C���h���A���̌�̒��_�����ݒ��VAO1�ɋL�^
    glBindVertexArray(VAO1);
    // VBO1��GL_ARRAY_BUFFER�Ƀo�C���h
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // ���_�f�[�^��VBO1�ɓ]��
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1_vertices), triangle1_vertices, GL_STATIC_DRAW);

    // ���_�����|�C���^�̐ݒ� (�ʒu�f�[�^ - location 0)
    // 6��float��1���_���̃f�[�^�T�C�Y�A�I�t�Z�b�g0����n�܂�
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // ���_����0��L����

    // ���_�����|�C���^�̐ݒ� (�F�f�[�^ - location 1)
    // 6��float��1���_���̃f�[�^�T�C�Y�A�I�t�Z�b�g3*sizeof(float)����n�܂�
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // ���_����1��L����

    // VAO��VBO�̃o�C���h������ (�ݒ肪VAO1�ɋL�^���ꂽ�̂ňꎞ�I�ɉ���)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 2�Ԗڂ̎O�p�` (��) �p��VAO��VBO ---
    unsigned int VAO2, VBO2; // 2�Ԗڂ̎O�p�`�p��ID
    glGenVertexArrays(1, &VAO2); // VAO��1����
    glGenBuffers(1, &VBO2);     // VBO��1����

    // VAO2���o�C���h���A���̌�̒��_�����ݒ��VAO2�ɋL�^
    glBindVertexArray(VAO2);
    // VBO2��GL_ARRAY_BUFFER�Ƀo�C���h
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // ���_�f�[�^��VBO2�ɓ]��
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2_vertices), triangle2_vertices, GL_STATIC_DRAW);

    // ���_�����|�C���^�̐ݒ� (�ʒu�f�[�^ - location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ���_�����|�C���^�̐ݒ� (�F�f�[�^ - location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VAO��VBO�̃o�C���h������
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 10. �����_�����O���[�v (�Q�[�����[�v)
    // ������ 10. �����_�����O���[�v (�e�I�u�W�F�N�g���ʂɕ`��) ������
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        // float timeValue = glfwGetTime(); // ���̍s���폜�܂��̓R�����g�A�E�g
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // timeValue �� `currentFrame` ���g�����A���[�v�O�ň�x��`���čX�V���邩�A
        // ���邢�� `glfwGetTime()` �𒼐ڎg���悤�ɂ��܂��B
        // �����ł́A���ɐ錾����Ă��� `currentFrame` �� `timeValue` �̑���Ɏg���̂����R�ł��B
        // �������́A`main`�֐��̐擪��`float timeValue = 0.0f;`�Ɛ錾���A���[�v����`timeValue = glfwGetTime();`�Ƃ���B
        // �����`currentFrame`���g���܂��傤�B
        // �܂��́A�A�j���[�V�����v�Z�ɒ���`glfwGetTime()`���g���܂��B

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();

        // �r���[�s��̌v�Z�Ƒ��M (�ύX�Ȃ�)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        // �ˉe�s��̌v�Z�Ƒ��M (�ύX�Ȃ�)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // --- �ŏ��̎O�p�` (�I�����W) �̕`�� ---
        glm::mat4 model1 = glm::mat4(1.0f);
        // ������ `timeValue` �̑���� `currentFrame` ���g�p���܂�
        model1 = glm::translate(model1, glm::vec3(sin(currentFrame) * 0.5f, 0.0f, 0.0f));
        model1 = glm::rotate(model1, currentFrame, glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", model1);

        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // --- 2�Ԗڂ̎O�p�` (��) �̕`�� ---
        glm::mat4 model2 = glm::mat4(1.0f);
        // �����ł� `timeValue` �̑���� `currentFrame` ���g�p���܂�
        model2 = glm::translate(model2, glm::vec3(0.5f, cos(currentFrame) * 0.3f, 0.0f));
        model2 = glm::rotate(model2, -currentFrame * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", model2);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 11. ���\�[�X�̉�� (�ύX�Ȃ�)
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    // �V�F�[�_�[�v���O������Shader�N���X�̃f�X�g���N�^�Ŏ����I�ɉ�������ׂ��ł����A
    // �����I��glDeleteProgram(ourShader.ID); ���Ăяo���K�v�͂���܂���B
    // Shader�I�u�W�F�N�g���X�R�[�v�𔲂���Ƃ��Ɏ����I�ɍs���܂��B

    glfwTerminate();
    return 0;
}

// -----------------------------------------------------------------------------
// �R�[���o�b�N�֐�: �E�B���h�E�T�C�Y�ύX���ɌĂяo����� (�ύX�Ȃ�)
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -----------------------------------------------------------------------------
// ���͏����֐�: �L�[�{�[�h���͂Ȃǂ����� (�ύX�Ȃ�)
// -----------------------------------------------------------------------------
// �����͏����֐�: �L�[�{�[�h���͂Ȃǂ����� (�啝�C��) ��
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // �J�����̈ړ����x
    float cameraSpeed = 2.5f * deltaTime; // deltaTime����Z���ăt���[�����[�g�Ɉˑ����Ȃ��ړ����x�ɂ���

    // �O���ړ� (W�L�[)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // ����ړ� (S�L�[)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // �����ړ� (A�L�[)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        // cameraFront��cameraUp�̊O�ςŉE�����x�N�g�����v�Z���A����ƃJ�����̏�����̊O�ςō��������v�Z
        // ��萳�m�ɂ́AcameraFront��cameraUp�̊O�� (cross) �Łu�E�����x�N�g���v���v�Z���A���̋t��������
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // �E���ړ� (D�L�[)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        // cameraFront��cameraUp�̊O�� (cross) �Łu�E�����x�N�g���v���v�Z
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// ���}�E�X�̓�������������R�[���o�b�N�֐� (�V�����ǉ�) ��
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // �v���O�����J�n��̏���}�E�X���͎�
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // �O��̍��W����̃}�E�X�̈ړ��ʂ��v�Z
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // ����: y���W�̓X�N���[�����W�n�ŉ����������Ȃ̂Ŕ��]������
    lastX = xpos;
    lastY = ypos;

    // �}�E�X���x
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // yaw��pitch���X�V
    yaw += xoffset;
    pitch += yoffset;

    // pitch�ɐ����݂��ăJ�������㉺���]���Ȃ��悤�ɂ���
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // �I�C���[�p����J�����̎��������x�N�g�����v�Z
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction); // �P�ʃx�N�g���ɐ��K�����ăZ�b�g
}