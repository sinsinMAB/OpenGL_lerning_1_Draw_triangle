#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model; // ★新しく追加★: モデル行列を受け取る
uniform mat4 view;
uniform mat4 projection; // ★新しく追加★: 射影行列を受け取る

out vec3 ourColor; // ★追加★: フラグメントシェーダーに渡す色

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // 頂点位置をモデル行列で変換
    ourColor = aColor; // 頂点の色をフラグメントシェーダーに渡す
}