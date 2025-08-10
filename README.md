# OpenGL_lerning_1_Draw_triangle
OpenGLを用いて、初歩的な三角形を描画することができた。  
マウス入力とキー入力も実装できたが、座標系の計算がいまいち理解できていない。  
次は、グラフィックスの根本的な機能（シェーディング、テクスチャマッピング、アニメーションなど）の実装を目指す。  

visual studio 2022を開発環境とし、c++での開発を行っている。  
c++によるデスクトップ開発にチェックを入れ、からのプロジェクトを作成し、導入する。  
  
利用したパッケージは、glad, glfw-3.4.bin.WIN64, glm-1.0.1の三つである。(以下を参照)  
GLAD : https://glad.dav1d.de/  
GLFW : https://www.glfw.org/download.html  
GLM  : https://github.com/g-truc/glm/releases  

※gladは3.3を使用  
  
それぞれのサイトから、各ファイルをダウンロードし、適当な場所へ展開する。  
visual studioの上段にある「プロジェクト」から「（プロジェクト名）のプロパティ」を選択する。  
  
左側のツリービューで、「構成プロパティ」>「VC++ ディレクトリ」を選択。  
右側の設定一覧で、「インクルードディレクトリ」を探し、選択。  
「インクルードディレクトリ」の右端に表示される下矢印をクリックし、「編集」を選択し、以下を追加。  
・C:\Users\あなたのユーザー名\Documents\OpenGL\packages\glad\include  
・C:\Users\あなたのユーザー名\Documents\OpenGL\packages\glfw-3.4.bin.WIN64\include  
・C:\Users\あなたのユーザー名\Documents\OpenGL\packages\glm-1.0.1  
※Documentsで、OpenGLというプロジェクト用のファイル内に、packagesという専用のディレクトリに各ファイルを配下しているときのパス指定である。   

次に、引き続き「構成プロパティ」>「VC++ ディレクトリ」を選択したまま、「ライブラリディレクトリ」を探し、選択。  
右端の下矢印をクリックし、「<編集>」を選択し、以下を追加。  
・C:\Users\あなたのユーザー名\Documents\OpenGL\packages\glfw-3.4.bin.WIN64\lib-vc2022  
※※Visual Studio 2022を使っている場合。バージョンによってlib-vc2019など、となる場合があります。ご自身のGLFWフォルダ内を確認し、適切なフォルダを選択すること。  

  
左側のツリービューで、「構成プロパティ」>「リンカー」>「入力」を選択。  
右側の設定一覧で、「追加の依存ファイル」を探し、選択。  
右端の下矢印をクリックし、「<編集>」を選択し、以下の行を追加。  
opengl32.lib  
glfw3.lib  
  
プロパティウィンドウの下部にある「適用」をクリックし、その後「OK」をクリックしてプロパティウィンドウを閉じる。  
  
適用されているかの確認  
以下のプログラムを作成し、ビルドが適切に通るかで判断する。  
  
#include <glad/glad.h>  
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>  
  
int main() {  
    return 0;  
}  
  
  
ここで、環境の実装は終わり。  
