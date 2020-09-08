#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.hpp"

struct Ball {

	Ball(const glm::vec2 &start_pos, float speed) {
		this->position = start_pos;
		this->speed    = speed;
		this->color    = glm::u8vec4(0xC3, (int)(speed * 0x3C), 0x54, 0xFF);
	};
	~Ball();

	void update(float elapsed);
	void draw(std::vector<Vertex> &vertices);

	glm::vec2 position;
	glm::u8vec4 color;
	float speed;

	const float radius = 0.25f;
};
