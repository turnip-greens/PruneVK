#pragma once

#include "pvk_window.hpp"
#include "pvk_device.hpp"
#include "pvk_renderer.hpp"
#include "pvk_game_object.hpp"
#include "pvk_buffer.hpp"

//std
#include <memory>
#include <vector>

namespace pvk
{
	struct GlobalUBO
	{
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, 3.f, -1.f });
	};

	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();
	private:
		void loadGameObjects();

		PvkWindow pvkWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		PvkDevice pvkDevice{ pvkWindow };
		PvkRenderer pvkRenderer{ pvkWindow, pvkDevice };

		std::vector<PvkGameObject> gameObjects;
	};
}