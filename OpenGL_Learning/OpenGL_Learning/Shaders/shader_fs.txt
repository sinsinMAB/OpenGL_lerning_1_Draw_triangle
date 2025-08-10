#version 330 core
out vec4 FragColor;

in vec3 ourColor; // ★追加★: 頂点シェーダーから受け取った色

void main()
{
    FragColor = vec4(ourColor, 1.0f); // 受け取った色をそのまま出力
}