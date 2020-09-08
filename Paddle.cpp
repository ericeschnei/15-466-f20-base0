#include "Paddle.hpp"


Paddle::Paddle()    {}
Paddle::~Paddle()   {}

void Paddle::handle_collision(Ball &ball) {
}

void Paddle::draw(std::vector<Vertex> &vertices) {
	vertices.emplace_back();
	
	glm::vec2   pos = this->position;
	float       normal = this->normal;

	float cos_theta = glm::cos(normal);
	float sin_theta = glm::cos(normal);

	// rotate and move paddle into place
	glm::mat3x2 transform_matrix = glm::mat3x2(
		glm::vec2(cos_theta, sin_theta),
		glm::vec2(-sin_theta, cos_theta),
		pos
	);	

	// lambda to quickly add a point without repeating myself
	auto add_point = [this, &vertices](glm::vec2 const &point) {
		vertices.emplace_back(
			glm::vec3(point, 0.0f),
			this->color,
			glm::vec2(0.0f, 0.0f)
		);
	};
	
	glm::vec2 radius = this->paddle_radius;
	glm::vec2 top_left = transform_matrix * glm::vec3(-radius.x,  radius.y, 1);
	glm::vec2 top_rght = transform_matrix * glm::vec3( radius.x,  radius.y, 1);
	glm::vec2 bot_left = transform_matrix * glm::vec3(-radius.x, -radius.y, 1);
	glm::vec2 bot_rght = transform_matrix * glm::vec3( radius.x, -radius.y, 1);

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

}
