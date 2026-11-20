#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "pvk_camera.hpp"
#include "keyboard_movement_controller.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <chrono>
#include <numeric>

namespace pvk
{
	FirstApp::FirstApp()
	{
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		std::vector<std::unique_ptr<PvkBuffer>> uboBuffers(PvkSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<PvkBuffer>(
				pvkDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		SimpleRenderSystem simpleRenderSystem{ pvkDevice, pvkRenderer.getSwapChainRenderPass() };

        PvkCamera camera{};
        camera.setViewTarget(glm::vec3{ -1.f, -2.f, 2.f }, glm::vec3{0.f, 0.f, 2.5f});

        auto viewerObject = PvkGameObject::createGameObject();

        KeyboardMovementController cameraController{};
		
		static float time = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!pvkWindow.shouldClose())
		{
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
			time += frameTime;

            cameraController.moveInPlaneXZ(pvkWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			//gameObjects[0].transform.rotation.y += glm::radians(360.0f) * frameTime;
			//gameObjects[0].transform.rotation.y =
				//glm::mod(gameObjects[0].transform.rotation.y, glm::two_pi<float>());
			
			//gameObjects[0].transform.translation.y =
				//sinf(time * 2.f) * 0.4f;

            if (glfwGetKey(pvkWindow.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) pvkWindow.close();

            float aspect = pvkRenderer.getAspectRatio();

            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.001f, 1000.f);

			if (auto commandBuffer = pvkRenderer.beginFrame())
			{
				int frameIndex = pvkRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// Update
				GlobalUBO ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				pvkRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				pvkRenderer.endSwapChainRenderPass(commandBuffer);
				pvkRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(pvkDevice.device());
	}

	void FirstApp::loadGameObjects()
	{
        std::shared_ptr<PvkModel> pvkModel = PvkModel::createModelFromFile(pvkDevice, "C:\\Users\\isaac\\Desktop\\C++\\PruneVK\\assets\\models\\smooth_vase.obj");

        auto dragon = PvkGameObject::createGameObject();
		dragon.model = pvkModel;
		dragon.transform.translation = { .0f, .0f, 2.5f };
		dragon.transform.scale = glm::vec3(1.f);
		
		dragon.transform.rotation = { glm::radians(0.f), 0.f, 0.f};

        gameObjects.push_back(std::move(dragon));
	}
}