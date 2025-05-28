#include <iostream>
#include <string>
#include<assert.h>
#include<functional>
#include"wrapper/check_error.h"
#include"Application/Application.h"

#include "glframework/shader.h"
#include "glframework/core.h"
#include "glframework/texture.h"
#include "glframework/renderer/renderer.h"
#include "glframework/Scene.h"
#include "glframework/RayTracer.h"

#include "Application/camera/perspectiveCamera.h"
#include "Application/camera/orthographicCamera.h"
#include "Application/camera/trackballControl.h"
#include "Application/camera/gameControl.h"

#include "Application/AssimpLoader.h"

using namespace std;


GLuint vao;
Shader* shader{ nullptr };
Texture* texture = nullptr;
PerspectiveCamera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;
glm::mat4 transform(1.0f);
Renderer* renderer = nullptr;
Scene* scene = nullptr;
MineSweeper* mineSweeper = nullptr;

void resizeCallback(int width, int heigh)
{
	glViewport(0, 0, width, heigh);
}
void keyCallback(int key, int action, int mods)
{
	cameraControl->keyCallback(key, action, mods);
}
void mouseCallback(int button, int action, int mods)
{
	double x, y;
	App->getCursorPos(&x, &y);
	cameraControl->mouseCallback(button, action, x, y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		App->getCursorPos(&x, &y);
		// ������Ҫʵ������׷����ȷ���������Ƭ����
		int width = App->get_width();
		int height = App->get_heigh();

		// ����Ļ����ת��Ϊ��һ���豸����
		float ndcX = (2.0f * x) / width - 1.0f;
		float ndcY = 1.0f - (2.0f * y) / height;

		// ��ͶӰ�õ�����
		glm::mat4 inverseProjection = glm::inverse(camera->getProjectionMatrix());
		glm::mat4 inverseView = glm::inverse(camera->getViewMatrix());

		glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		glm::vec4 rayEye = inverseProjection * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
		glm::vec3 rayWorld = glm::vec3(inverseView * rayEye);
		rayWorld = glm::normalize(rayWorld);

		// ���ߵ����Ϊ���λ��
		glm::vec3 rayOrigin = camera->mPosition;

		// ʹ�� RayTracer::findClosestIntersection ���ҵ�����Ľ���
		RayHit hit = RayTracer::findClosestIntersection(scene, rayOrigin, rayWorld);
		int faceIndex = hit.faceIndex;

		//ȫ��û�˻��ߵ㵽����Ϸ����
		if (faceIndex != -1 && !mineSweeper->handleClick(faceIndex)) {
			// ��Ϸ����
			std::cout << "Game Over!" << std::endl;
		}
	}
}

void cursorCallback(double xpos, double ypos)
{
	cameraControl->cursorCallback(xpos, ypos);
}
void scrollCallback(double yoffset)//
{
	cameraControl->scrollCallback(yoffset);
}


void prepareCamera()
{
	float size = 2.0f;
	camera = new PerspectiveCamera(60.0f, (float)App->get_width() / (float)App->get_heigh(), 0.1f, 1000.0f);
	//camera = new OrthographicCamera(-size, size, size, -size, size, -size);
	cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
}

void prepare()
{
	renderer = new Renderer();
	scene = new Scene();

	auto testModel = AssimpLoader::load("assets/fbx/MineSweeper.fbx");

	scene->addChild(testModel);


	Mesh* mesh = nullptr;
	// ���� testModel �����Ӷ����ҵ���һ�� Mesh ����
	std::function<void(Object*)> findMesh = [&](Object* object) {
		if (object->getType() == ObjectType::Mesh) {
			mesh = static_cast<Mesh*>(object);
			return;
		}
		auto children = object->getChildren();
		for (auto child : children) {
			findMesh(child);
			if (mesh) break;
		}
	};
	findMesh(testModel);

	if (mesh) {

		int mineCount = 32; // ��������
		mineSweeper = new MineSweeper(mesh, mineCount);
		
	}
	else {
		std::cerr << "Error: No Mesh object found in testModel." << std::endl;
	}

}

int main()
{
	if (!App->init(800, 600))
		return -1;

	App->set_resize_callback(resizeCallback);
	App->set_key_callback(keyCallback);
	App->set_mouse_callback(mouseCallback);
	App->set_cursor_callback(cursorCallback);
	App->set_scroll_callback(scrollCallback);

	glViewport(0, 0, 800, 600);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	prepareCamera();
	prepare();
	while (App->update())
	{
		//��Ⱦ
		cameraControl->update();
		renderer->render(scene, camera, mineSweeper);
	
	}

	App->destroy();
	delete texture;
	delete renderer;
	delete scene;
	delete mineSweeper;

	return 0;

}