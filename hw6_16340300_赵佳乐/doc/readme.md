

# 实验报告 - Lights and Shading

学号：16340300  姓名：赵佳乐

## 实验要求:

### Basic:

+ 实现Phong光照模型：
    1. 场景中绘制一个cube
    2. 自己写shader实现两种shading: Phong Shading 和 Gouraud 
    3. Shading，并解释两种shading的实现原理
    4. 合理设置视点、光照位置、光照颜色等参数，使光照效果明显显示
+ 使用GUI，使参数可调节，效果实时更改：
    1. GUI里可以切换两种shading
    2. 使用如进度条这样的控件，使ambient因子、diffuse因子、specular因子、反光度等参数可调节，光照效 果实时更改

#### Bonus:
+ 当前光源为静止状态，尝试使光源在场景中来回移动，光照效果实时更改。

## 原理描述

+ Phong光照模型

    Phong光照模型主要由3个分量构成：

    - 环境光照(Ambient Lighting)：即使在黑暗的情况下，世界上通常也仍然有一些光亮（月亮、远处的光），所以物体几乎永远不会是完全黑暗的。为了模拟这个，我们会使用一个环境光照常量，它永远会给物体一些颜色。
    - 漫反射光照(Diffuse Lighting)：模拟光源对物体的方向性影响(Directional Impact)。它是冯氏光照模型中视觉上最显著的分量。物体的某一部分越是正对着光源，它就会越亮。
    - 镜面光照(Specular Lighting)：模拟有光泽物体上面出现的亮点。镜面光照的颜色相比于物体的颜色会更倾向于光的颜色。

    三种光照强度的计算:

    - 环境光:使用光的颜色乘以常量环境因子，再乘以物体的颜色。

            vec3 ambient = ambientFactor * lightColor;
    - 漫反射:计算公式

        $$ I_d=I_pK_d(L·N) $$

        $I_d$为漫反射光照强度，$I_p$为点光源强度，$K_d$为漫反射率，L为入射光线，N为法向量。

            float diff = max(dot(norm, lightDirection), 0.0);
	        vec3 diffuse = diffuseFactor * diff * lightColor;
    
    - 镜面反射:计算公式

        $$ I_s=I_pK_s(R·V)^n $$

        $I_s$为镜面反射光照强度，$I_p$为点光源强度，$K_s$为镜面反射指数，R为反射光线，V为视线。

            float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininessFactor);
	        vec3 specular = specularFactor * spec * lightColor;

+ Phong Shading 和 Gouraud Shading 的异同

    + 相同点：都是按照Phong光照模型实现光照着色器 
    + 不同点：Gouraud Shading在顶点着色器中实现，而Phong Shading是在片段着色器中实现的。
        - Phong Shading - 逐个片段计算着色，看起来更加自然，但是计算量比较大。
        - Gouraud Shading - 在顶点着色器中，顶点会少很多，因此光照计算频率会更低，效率更高；但是片段颜色是由插值计算得的，顶点较少时光照会非常不真实。

+ bonus
    移动光源实现，分别令其绕不同的轴旋转。

        if (lightTag == 0)
			{
				lightPosition = glm::vec3(-1.0, sin(glfwGetTime()) * 1.0f, cos(glfwGetTime()) * 1.0f);
			}
			else if (lightTag == 1)
			{
				lightPosition = glm::vec3(sin(glfwGetTime()) * 1.0f, 1.0f, cos(glfwGetTime()) * 1.0f);
			}
			else {
				lightPosition = glm::vec3(sin(glfwGetTime()) * 1.0f, cos(glfwGetTime()) * 1.0f, 0.0f);
			}

## 实验结果

见MP4文件。
+ ambientFactor : 数值越大，物体越亮。说明环境光越亮，物体越亮。
+ diffuseFactor ： 数值越大，物体越亮。说明漫反射强度越强。
+ specularFactor ： 数值越大，镜面效果越明显。
+ shininess ：数值越大散射得越少，高光点越小。
