# 实验报告 - Shadow Mapping
学号：16340300 姓名：赵佳乐
## 实验要求
### Basic:
1. 实现方向光源的Shadowing Mapping:
    + 要求场景中至少有一个object和一块平面(用于显示shadow)
    + 光源的投影方式任选其一即可
    + 在报告里结合代码，解释Shadowing Mapping算法
2. 修改GUI
### Bonus:
1. 实现光源在正交/透视两种投影下的Shadowing Mapping
2. 优化Shadowing Mapping (可结合References链接，或其他方法。优化方式越多越好，在报告里说明，有加分)

## 实现效果见演示视频

## 算法描述


+ 首先生成深度贴图，深度贴图是从光的透视图里渲染的深度纹理，最近的深度是被光线照射到的地方，当深度值大于该值时，则被遮挡，显示为阴影。
    - 创建一个帧缓冲对象和一个2D纹理，提供给帧缓冲的深度缓冲使用：

        	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
            unsigned int depthMapFBO;
            glGenFramebuffers(1, &depthMapFBO);
            // create depth texture
            unsigned int depthMap;
            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    - 把生成的深度纹理作为帧缓冲的深度缓冲

            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    - 生成贴图

            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = 1.0f, far_plane = 7.5f;
            if (tag == 1) {
                lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
            }
            if (tag == 0) {
                lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            }
            lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            // render scene from light's point of view
            simpleDepthShader.use();
            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            renderScene(simpleDepthShader);

+ 第二步渲染至深度缓冲，即以光的透视图进行场景渲染，该着色器，目的是把顶点变换到光空间

        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 lightSpaceMatrix;
        uniform mat4 model;

        void main()
        {
            gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
        }

+ 第三步阴影的渲染

    - 顶点着色器
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec2 aTexCoords;

            out vec2 TexCoords;

            out VS_OUT {
                vec3 FragPos;
                vec3 Normal;
                vec2 TexCoords;
                vec4 FragPosLightSpace;
            } vs_out;

            uniform mat4 projection;
            uniform mat4 view;
            uniform mat4 model;
            uniform mat4 lightSpaceMatrix;

            void main()
            {
                vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
                vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
                vs_out.TexCoords = aTexCoords;
                vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
                gl_Position = projection * view * model * vec4(aPos, 1.0);
            }

    + 像素着色器,其他地方与之前类似，加上一个计算阴影的部分

            float ShadowCalculation(vec4 fragPosLightSpace)
            {
                // perform perspective divide
                vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
                // transform to [0,1] range
                projCoords = projCoords * 0.5 + 0.5;
                // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
                float closestDepth = texture(shadowMap, projCoords.xy).r; 
                // get depth of current fragment from light's perspective
                float currentDepth = projCoords.z;
                // calculate bias (based on depth map resolution and slope)
                vec3 normal = normalize(fs_in.Normal);
                vec3 lightDir = normalize(lightPos - fs_in.FragPos);
                float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
                // check whether current frag pos is in shadow
                // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
                // PCF
                float shadow = 0.0;
                vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
                for(int x = -1; x <= 1; ++x)
                {
                    for(int y = -1; y <= 1; ++y)
                    {
                        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                        shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
                    }    
                }
                shadow /= 9.0;
                
                // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
                if(projCoords.z > 1.0)
                    shadow = 0.0;
                    
                return shadow;
            }

## Bonus
+ 两种不同的投影方式
    
        if (tag == 1) {
			lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
		}
		if (tag == 0) {
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		}

+ 改进阴影质量
    - 阴影失真，即阴影出现条纹，可以在计算阴影时设置一个偏移量解决：

            float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
    - 阴影出现锯齿，可以用PCF方案解决，即多次采样计算均值：

            float shadow = 0.0;
            vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
            for(int x = -1; x <= 1; ++x)
            {
                for(int y = -1; y <= 1; ++y)
                {
                    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                    shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
                }    
            }
            shadow /= 9.0;