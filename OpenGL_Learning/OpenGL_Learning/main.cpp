#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>    // sin関数のためにcmathを明示的にインクルード

#include "Shader.h" // 作成したShaderクラスのヘッダーをインクルード

// ★GLMのヘッダーをインクルード★
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale のために必要
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr のために必要

// --- 関数プロトタイプ宣言 ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// --- グローバル変数 (必要に応じて) ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float yaw = -90.0f;  // yaw: 左右の回転角。-90.0fでZ軸の負方向を向く（初期設定）
float pitch = 0.0f;   // pitch: 上下の回転角
float lastX = 800.0f / 2.0; // 前回のマウスカーソルのX座標
float lastY = 600.0f / 2.0; // 前回のマウスカーソルのY座標
bool firstMouse = true;   // プログラム開始後の初回マウス入力かどうかを判定するフラグ

// ★カメラのグローバル変数 (新しい追加) ★
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);   // カメラの初期位置 (Z軸で奥に3.0f)
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);  // カメラの視線方向 (Z軸の負の方向、-1.0は手前)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   // カメラの上方向 (Y軸の正の方向)

float deltaTime = 0.0f; // 前のフレームから現在のフレームまでの時間
float lastFrame = 0.0f; // 前のフレームの時刻

// -----------------------------------------------------------------------------
// MAIN 関数
// -----------------------------------------------------------------------------
int main()
{
    // 1. GLFWの初期化 (変更なし)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 2. ウィンドウの作成 (変更なし)
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ここから始まるゲーム作成！！", NULL, NULL); // タイトルを変更
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. GLADの初期化 (変更なし)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 4. OpenGLビューポートの設定 (変更なし)
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // ★マウスカーソルを非表示にし、ウィンドウ内に固定する (新しい追加) ★
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ★マウスの動きを検出するコールバック関数を設定する (新しい追加) ★
    glfwSetCursorPosCallback(window, mouse_callback);

    // --- シェーダーの準備 ---
    // Shaderクラスのインスタンス化 (シェーダーファイルのパスを指定)
    // プロジェクトのルートディレクトリにshader.vsとshader.fsがあることを前提
    // もし別の場所に置く場合は、適切なパスを指定してください。
    // 例: Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    Shader ourShader("Shaders/shader.vs", "Shaders/shader.fs");


    // ★★★ 8. 頂点データの定義 (各三角形ごとに別々の配列で定義) ★★★
    // これにより、それぞれの三角形の頂点データを個別に管理できます。
    // 最初の三角形 (オレンジ) の頂点データ
    float triangle1_vertices[] = {
        // 位置(x, y, z)          色(r, g, b)
        -0.5f, -0.5f, 0.0f,      1.0f, 0.5f, 0.2f, // 左下
         0.5f, -0.5f, 0.0f,      1.0f, 0.5f, 0.2f, // 右下
         0.0f,  0.5f, 0.0f,      1.0f, 0.5f, 0.2f  // 上
    };
    // 2番目の三角形 (青) の頂点データ - 意図的に右にずらしています
    float triangle2_vertices[] = {
        // 位置(x, y, z)          色(r, g, b)
         0.2f, -0.5f, 0.0f,      0.2f, 0.5f, 1.0f, // 左下
         1.2f, -0.5f, 0.0f,      0.2f, 0.5f, 1.0f, // 右下
         0.7f,  0.5f, 0.0f,      0.2f, 0.5f, 1.0f  // 上
    };

    // ★★★ 9. VAOとVBOの作成と設定 (各三角形ごとに独立したセットを生成) ★★★

    // --- 最初の三角形 (オレンジ) 用のVAOとVBO ---
    unsigned int VAO1, VBO1; // 1番目の三角形用のID
    glGenVertexArrays(1, &VAO1); // VAOを1つ生成
    glGenBuffers(1, &VBO1);     // VBOを1つ生成

    // VAO1をバインドし、その後の頂点属性設定をVAO1に記録
    glBindVertexArray(VAO1);
    // VBO1をGL_ARRAY_BUFFERにバインド
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // 頂点データをVBO1に転送
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1_vertices), triangle1_vertices, GL_STATIC_DRAW);

    // 頂点属性ポインタの設定 (位置データ - location 0)
    // 6つのfloatが1頂点分のデータサイズ、オフセット0から始まる
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // 頂点属性0を有効化

    // 頂点属性ポインタの設定 (色データ - location 1)
    // 6つのfloatが1頂点分のデータサイズ、オフセット3*sizeof(float)から始まる
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // 頂点属性1を有効化

    // VAOとVBOのバインドを解除 (設定がVAO1に記録されたので一時的に解除)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 2番目の三角形 (青) 用のVAOとVBO ---
    unsigned int VAO2, VBO2; // 2番目の三角形用のID
    glGenVertexArrays(1, &VAO2); // VAOを1つ生成
    glGenBuffers(1, &VBO2);     // VBOを1つ生成

    // VAO2をバインドし、その後の頂点属性設定をVAO2に記録
    glBindVertexArray(VAO2);
    // VBO2をGL_ARRAY_BUFFERにバインド
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // 頂点データをVBO2に転送
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2_vertices), triangle2_vertices, GL_STATIC_DRAW);

    // 頂点属性ポインタの設定 (位置データ - location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 頂点属性ポインタの設定 (色データ - location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VAOとVBOのバインドを解除
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 10. レンダリングループ (ゲームループ)
    // ★★★ 10. レンダリングループ (各オブジェクトを個別に描画) ★★★
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        // float timeValue = glfwGetTime(); // この行を削除またはコメントアウト
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // timeValue は `currentFrame` を使うか、ループ外で一度定義して更新するか、
        // あるいは `glfwGetTime()` を直接使うようにします。
        // ここでは、既に宣言されている `currentFrame` を `timeValue` の代わりに使うのが自然です。
        // もしくは、`main`関数の先頭で`float timeValue = 0.0f;`と宣言し、ループ内で`timeValue = glfwGetTime();`とする。
        // 今回は`currentFrame`を使いましょう。
        // または、アニメーション計算に直接`glfwGetTime()`を使います。

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();

        // ビュー行列の計算と送信 (変更なし)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        // 射影行列の計算と送信 (変更なし)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // --- 最初の三角形 (オレンジ) の描画 ---
        glm::mat4 model1 = glm::mat4(1.0f);
        // ここで `timeValue` の代わりに `currentFrame` を使用します
        model1 = glm::translate(model1, glm::vec3(sin(currentFrame) * 0.5f, 0.0f, 0.0f));
        model1 = glm::rotate(model1, currentFrame, glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", model1);

        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // --- 2番目の三角形 (青) の描画 ---
        glm::mat4 model2 = glm::mat4(1.0f);
        // ここでも `timeValue` の代わりに `currentFrame` を使用します
        model2 = glm::translate(model2, glm::vec3(0.5f, cos(currentFrame) * 0.3f, 0.0f));
        model2 = glm::rotate(model2, -currentFrame * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", model2);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 11. リソースの解放 (変更なし)
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    // シェーダープログラムはShaderクラスのデストラクタで自動的に解放されるべきですが、
    // 明示的にglDeleteProgram(ourShader.ID); を呼び出す必要はありません。
    // Shaderオブジェクトがスコープを抜けるときに自動的に行われます。

    glfwTerminate();
    return 0;
}

// -----------------------------------------------------------------------------
// コールバック関数: ウィンドウサイズ変更時に呼び出される (変更なし)
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -----------------------------------------------------------------------------
// 入力処理関数: キーボード入力などを処理 (変更なし)
// -----------------------------------------------------------------------------
// ★入力処理関数: キーボード入力などを処理 (大幅修正) ★
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // カメラの移動速度
    float cameraSpeed = 2.5f * deltaTime; // deltaTimeを乗算してフレームレートに依存しない移動速度にする

    // 前方移動 (Wキー)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // 後方移動 (Sキー)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // 左方移動 (Aキー)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        // cameraFrontとcameraUpの外積で右方向ベクトルを計算し、それとカメラの上方向の外積で左方向を計算
        // より正確には、cameraFrontとcameraUpの外積 (cross) で「右方向ベクトル」を計算し、その逆方向が左
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // 右方移動 (Dキー)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        // cameraFrontとcameraUpの外積 (cross) で「右方向ベクトル」を計算
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// ★マウスの動きを処理するコールバック関数 (新しい追加) ★
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // プログラム開始後の初回マウス入力時
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // 前回の座標からのマウスの移動量を計算
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意: y座標はスクリーン座標系で下方向が正なので反転させる
    lastX = xpos;
    lastY = ypos;

    // マウス感度
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // yawとpitchを更新
    yaw += xoffset;
    pitch += yoffset;

    // pitchに制約を設けてカメラが上下反転しないようにする
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // オイラー角からカメラの視線方向ベクトルを計算
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction); // 単位ベクトルに正規化してセット
}