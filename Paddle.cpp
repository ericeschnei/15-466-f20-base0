#include "Paddle.hpp"
#include <iostream>

Paddle::Paddle()    {}
Paddle::~Paddle()   {}

void Paddle::handle_collision(Ball &ball) {
	
}

void Paddle::handle_click(glm::vec2 pos, bool down) {
	
	// double press, no release--
	// just pretend the last press didn't exist
	glm::vec2 pos_paddle = this->clip_to_paddle * glm::vec3(pos, 1);	
	if (down) {
		// transform mouse coords to paddle space
		if (glm::abs(pos_paddle.x) < this->paddle_radius.x && 
			glm::abs(pos_paddle.y) < this->paddle_radius.y) {
			sling_held = true;
			sling_start = pos_paddle;
		}
	} else {
		if (!sling_held) return;
		sling_held = false;

		glm::vec2 sling_start_world = this->paddle_to_clip * glm::vec3(this->sling_start, 1);
		glm::vec2 hit_vector = sling_start_world - pos;
		//glm::vec2 hit_vector_paddle = this->sling_start - pos_paddle;

		glm::vec2 linear_delta =
			this->sling_strength *  
			glm::vec3(hit_vector, 1);
		
		this->linear_velocity += linear_delta;

	}
}

void Paddle::draw(std::vector<Vertex> &vertices) {

	// rotate and move paddle into place
	glm::mat3x2 paddle_to_clip = this->paddle_to_clip;
	

	// lambda to quickly add a point without repeating myself
	auto add_point = [this, &vertices](glm::vec2 const &point) {
		vertices.emplace_back(
			glm::vec3(point, 0.0f),
			this->color,
			glm::vec2(0.0f, 0.0f)
		);
	};
	
	glm::vec2 radius = this->paddle_radius;
	glm::vec2 top_left = paddle_to_clip * glm::vec3(-radius.x,  radius.y, 1);
	glm::vec2 top_rght = paddle_to_clip * glm::vec3( radius.x,  radius.y, 1);
	glm::vec2 bot_left = paddle_to_clip * glm::vec3(-radius.x, -radius.y, 1);
	glm::vec2 bot_rght = paddle_to_clip * glm::vec3( radius.x, -radius.y, 1);
	
	// draw paddle as two CCW triangles
	// (just like in PongMode.cpp)
	add_point(bot_left);
	add_point(bot_rght);
	add_point(top_rght);

	add_point(bot_left);
	add_point(top_rght);
	add_point(top_left);
}

void Paddle::update(float elapsed) {
	this->position += this->linear_velocity  * elapsed;
	this->normal   += this->angular_velocity * elapsed;

	glm::vec2 pos = this->position;
	float normal  = this->normal;

	float cos_theta = glm::cos(normal);
	float sin_theta = glm::sin(normal);

	// world space to paddle space transform
	this->clip_to_paddle =
		// inverse rotation matrix
		glm::mat3x2(
			glm::vec2(cos_theta, -sin_theta),
			glm::vec2(sin_theta, cos_theta),
			glm::vec2(0, 0)
		) *
		// inverse translation matrix
		glm::mat3x3(
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(-pos, 1)
		);


	this->paddle_to_clip = 
		glm::mat3x2(
			glm::vec2(cos_theta, sin_theta),
			glm::vec2(-sin_theta, cos_theta),
			pos
		);
}
