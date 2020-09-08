#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"
#include "Paddle.hpp"
#include "Vertex.hpp"
#include "Ball.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

/*
 * DefendMode is a game mode that implements a single-player game of Pong.
 */

struct DefendMode : Mode {
	DefendMode();
	virtual ~DefendMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	float ball_speed = 1.0f;
	const float ball_speed_inc = 0.02f; // amount to increase speed over time
	const float ball_period = 0.5f; // seconds per ball

	glm::vec2 court_radius = glm::vec2(7.0f, 5.0f);

	float center_radius = 1.0f;
	float ball_timer = 0.0f;
	std::vector<Ball *> balls;
	Paddle paddle;

	//----- opengl assets / helpers ------

	//Shader program that draws transformed, vertices tinted with vertex colors:
	ColorTextureProgram color_texture_program;

	//Buffer used to hold vertex data during drawing:
	GLuint vertex_buffer = 0;

	//Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
	GLuint vertex_buffer_for_color_texture_program = 0;

	//Solid white texture:
	GLuint white_tex = 0;

	//matrix that maps from clip coordinates to court-space coordinates:
	glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
	// computed in draw() as the inverse of OBJECT_TO_CLIP
	// (stored here so that the mouse handling code can use it to position the paddle)

};
