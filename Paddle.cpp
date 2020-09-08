#include "Paddle.hpp"
#include <iostream>

Paddle::Paddle()    {}
Paddle::~Paddle()   {}

bool Paddle::is_colliding(Ball &ball) {
	// transform ball to paddle coords
	glm::vec2 pos = this->clip_to_paddle * glm::vec3(ball.position, 1);
	pos = glm::abs(pos);

	// x intersection
	if (pos.x < (paddle_radius.x + ball.radius) && pos.y < paddle_radius.y) {
		return true;
	}

	// y intersection
	if (pos.y < (paddle_radius.y + ball.radius) && pos.x < paddle_radius.x) {
		return true;
	}

	// corner intersection
	if (glm::length(paddle_radius - pos) < ball.radius) {
		return true;
	}
	return false;	
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
		glm::vec2 hit_vector_paddle = this->sling_start - pos_paddle;

		glm::vec2 linear_delta =
			(1.f / this->mass) *
			this->sling_strength *  
			glm::vec3(hit_vector, 1);
		
		this->linear_velocity += linear_delta;
		
		float angular_delta =
			this->sling_start.x * 
			hit_vector_paddle.y * 
			this->sling_strength /
			this->moment_of_inertia;
		
		this->angular_velocity += angular_delta;
	}
}

void Paddle::draw(std::vector<Vertex> &vertices) {

	// rotate and move paddle into place
	glm::mat3x2 paddle_to_clip = this->paddle_to_clip;
	
	// lambda to quickly add a point without repeating myself
	auto add_point = [&vertices](glm::vec2 const &point, glm::u8vec4 color) {
		vertices.emplace_back(
			glm::vec3(point, 0.0f),
			color,
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
	add_point(bot_left, color);
	add_point(bot_rght, color);
	add_point(top_rght, color);

	add_point(bot_left, color);
	add_point(top_rght, color);
	add_point(top_left, color);

	float line_radius = 0.05f;
	// draw line from mouse to paddle
	if (sling_held) {
		glm::vec2 sling_start = paddle_to_clip * glm::vec3(this->sling_start, 1);
		glm::vec2 sling_end = this->mouse_pos;

		glm::vec2 sling_norm = glm::normalize(sling_end - sling_start);
		// cc rotate 90deg
		sling_norm = glm::vec2(-sling_norm.y, sling_norm.x);
		sling_norm *= line_radius;

		top_left = sling_end + sling_norm;
		top_rght = sling_end - sling_norm;
		bot_left = sling_start + sling_norm;
		bot_rght = sling_start - sling_norm;

		add_point(bot_left, line_color);
		add_point(bot_rght, line_color);
		add_point(top_rght, line_color);

		add_point(bot_left, line_color);
		add_point(top_rght, line_color);
		add_point(top_left, line_color);



	}
}

void Paddle::update(float elapsed) {
	this->position += this->linear_velocity  * elapsed;
	this->normal   += this->angular_velocity * elapsed;


	// friction
	if (glm::length(this->linear_velocity) <= linear_friction * elapsed) {
		this->linear_velocity = glm::vec2(0.0f, 0.0f);
	} else {
		this->linear_velocity -= 
			glm::normalize(this->linear_velocity) * linear_friction * elapsed;
	}

	if (glm::abs(this->angular_velocity) <= angular_friction * elapsed) {
		this->angular_velocity = 0.0f;
	} else {
		this->angular_velocity -=
			glm::sign(this->angular_velocity) * angular_friction * elapsed;
	}
	
	// regenerate matrices
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
