//#pragma once
//#include <iostream>
//#include <string>
//#include <functional>
//
//#include <glad/glad.h>
//#include <GLFW/glfwWrapper.h>
//#include <stb_image.h>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "util/filesystem.h"
//#include "Defines.h"
//#include "config.h"
//#include "util/registerInput.h"
//#include "util/opQueue.h"
//#include "util/GStorage.H"
//#include "util/lambdaOp.h"
//#include "util/shader.h"
//#include "util/texture.h"
//#include "util/commonUtil.h"
//#include "util/VAO.h"
//#include "util/model.h"
//#include "util/camera.h"
//#include "util/fbo.h"
//
//
//using namespace OpenGL;
//using namespace Oper;
//using namespace Define;
//using namespace std;
//
//
//
//
//
//int main()
//{
//	// glfw: initialize and configure
//	Glfw::GetInstance()->Init(SCR_WIDTH, SCR_HEIGHT, windowTitle.c_str());
//	//Glfw::GetInstance()->LockCursor();
//
//
//
//	//注册相机
//		//------------ 窗口
//	float ratioWH = (float)SCR_WIDTH / (float)SCR_HEIGHT;
//	Camera mainCamera(ratioWH, moveSpeed, rotateSpeed, glm::vec3(0.0f, 0.0f, 4.0f));
//	GStorage<Camera *>::GetInstance()->Register(str_MainCamera.c_str(), &mainCamera);
//
//	////设置几何物体
//	VAO  cubeVAO(CubeVertices, sizeof(CubeVertices), { 3, 3, 2 });
//	VAO  planeVAO(planeVertices, sizeof(planeVertices), { 3, 0, 2 });
//	VAO  framebufferVAO(quadVertices, sizeof(quadVertices), { 3, 3, 2 }, quadIndices, sizeof(quadIndices));
//
//	Shader cubeShader("./shader/advancedOpengl/depthTest.vs", "./shader/advancedOpengl/depthTest.fs");
//	Shader framebufferShader("./shader/advancedOpengl/framebuffer.vs", "./shader/advancedOpengl/framebuffer.fs");
//
//	Texture  texContainer(FileSystem::getPath("resources/textures/container2.png").c_str(), true, false);
//	Texture  texContainerSpec(FileSystem::getPath("resources/textures/container2_specular.png").c_str(), true, false);
//	Texture  planeTex(FileSystem::getPath("resources/textures/marble.jpg").c_str(), true, false);
//
//
//	texContainer.SetName("material.diffuse");
//	texContainerSpec.SetName("material.specular");
//	planeTex.SetName("material.diffuse");
//
//
//	std::vector<Texture>  textures{ texContainer, texContainerSpec };
//	Mesh  cubeMesh(cubeVAO, textures);
//	Mesh  planeMesh(planeVAO, planeTex);
//
//
//	FBO   frameBuffer(SCR_WIDTH, SCR_HEIGHT);
//	Texture   fboColorTexture = frameBuffer.GetColorTexture(0);
//	fboColorTexture.SetName("screenTexture");
//	Mesh    meshFbo(framebufferVAO, fboColorTexture);
//
//
//	auto initOp = new LambdaOp([]() {
//		//开启深度测试
//		glEnable(GL_DEPTH_TEST);
//		glDepthFunc(GL_LESS);
//
//		//开启混合模式
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glBlendEquation(GL_FUNC_ADD);
//
//		////开启背面剔除
//		//glEnable(GL_CULL_FACE);
//		//glCullFace(GL_BACK);
//		//glFrontFace(GL_CCW); // gl_ccw 代表的是逆时针的环绕方式
//
//	}, false);
//
//	initOp->Run();
//
//	//注册时间处理函数（需要和windows相应绑定处理各种输入）
//	auto registerInputOp = new RegisterInput(false);
//	registerInputOp->Run();
//
//
//	//时间
//	float deltaTime = 0.0f; // 当前帧与上一帧的时间差
//	GStorage<float *>::GetInstance()->Register(str_DeltaTime.c_str(), &deltaTime);
//	float lastFrame = 0.0f; // 上一帧的时间
//	auto timeOp = new LambdaOp([&]() {
//		float currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//		//std::cout << currentFrame << std::endl;
//	});
//
//
//
//	//设置物体材质， 环境灯光等
//	auto  settingEnvir = new LambdaOp([&] {
//		cubeShader.use();
//		//绘制之前都必须设置渲染状态, 设置位置, 物体渲染(如果状态不复位可以放置在渲染循环之前)
//		//全局属性面板
//		cubeShader.setBool("blinn", true);
//
//		//材质属性
//		cubeShader.setFloat("material.shininess", material_shininess);
//
//		// 灯光属性
//		cubeShader.setVec3("light.position", lightPos);
//		cubeShader.setVec3("light.ambient", light_ambient);
//		cubeShader.setVec3("light.diffuse", light_diffuse);
//		cubeShader.setVec3("light.specular", light_specular);
//
//	});
//
//	settingEnvir->Run();
//
//	auto geomtryOp = new  LambdaOp([&]() {
//
//		frameBuffer.Use();
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//		glClearColor(0.7, 0.8, 0.9, 1.0);
//		glEnable(GL_DEPTH_TEST);
//
//		cubeShader.use();
//		// 首先绘制不透明物体
//		glm::mat4 model = glm::mat4(1.0f);
//		cubeShader.setMat4("model", model);
//		cubeShader.setMat4("projection", mainCamera.GetProjectionMatrix());
//		cubeShader.setMat4("view", mainCamera.GetViewMatrix());
//		cubeShader.setVec3("viewPos", mainCamera.GetPos());
//		planeMesh.Draw(cubeShader);
//		cubeMesh.Draw(cubeShader);
//
//		//解除帧缓冲绑定
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glClearColor(0.7, 0.8, 0.9, 1.0);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		planeMesh.Draw(cubeShader);
//		cubeMesh.Draw(cubeShader);
//
//		framebufferShader.use();
//		//framebufferShader.setMat4("projection", mainCamera.GetProjectionMatrix());
//		//framebufferShader.setMat4("view", mainCamera.GetViewMatrix());
//		//framebufferShader.setVec3("viewPos", mainCamera.GetPos());
//		//framebufferShader.setMat4("model", model);
//		meshFbo.Draw(framebufferShader);
//
//
//	});
//
//
//	//渲染队列
//	auto renderQueue = new  OpQueue();
//	*renderQueue << geomtryOp;
//
//
//	//swap buffer 
//	auto endOp = new LambdaOp([]() {
//		glfwPollEvents();
//		glfwSwapBuffers(Glfw::GetInstance()->GetWindow());
//	});
//
//	auto opLoop = new OpQueue();
//	*opLoop << timeOp << renderQueue << endOp;
//
//	Glfw::GetInstance()->RenderLoop(opLoop);
//
//	Glfw::GetInstance()->Terminate();
//	return 0;
//}
//
//
