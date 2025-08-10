#include "Shader.h" // �쐬�����w�b�_�[�t�@�C�����C���N���[�h
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. �V�F�[�_�[�\�[�X�R�[�h�̃t�@�C������̓ǂݍ���
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ifstream�I�u�W�F�N�g����O���X���[�ł���悤�ɂ���
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // �t�@�C�����J��
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // �t�@�C���̃o�b�t�@���e���X�g���[���ɓ����
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // �t�@�C�������
        vShaderFile.close();
        fShaderFile.close();
        // �X�g���[���𕶎���ɕϊ�
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. �V�F�[�_�[�̃R���p�C��
    unsigned int vertex, fragment;
    // ���_�V�F�[�_�[
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // �t���O�����g�V�F�[�_�[
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 3. �V�F�[�_�[�v���O�����̃����N
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // �V�F�[�_�[�I�u�W�F�N�g�́A�v���O�����Ƀ����N���ꂽ��A���͂�s�v�Ȃ̂ō폜
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// ���V�����ǉ���: 4x4 �s�� (glm::mat4) ��uniform�ϐ��Ƃ��ăV�F�[�_�[�ɓn���֐�
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    // glUniformMatrix4fv: 4x4�̕��������_�s����V�F�[�_�[�ɑ��M����OpenGL�֐�
    // ��1����: uniform�ϐ��̃��P�[�V����
    // ��2����: �s��̐� (1�Ȃ̂�1)
    // ��3����: �s�񂪓]�u����邩 (GLM�͗�D��Ȃ̂�GL_FALSE)
    // ��4����: �s��̍ŏ��̗v�f�ւ̃|�C���^ (glm::value_ptr�Ŏ擾)
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}