#include "GL.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.hpp"
#include "Ball.hpp"

struct Paddle {
	
	Paddle();
	~Paddle();
	
	bool is_colliding(Ball &ball);
	void handle_click(glm::vec2 pos, bool down);
	void update(float elapsed);
	void draw(std::vector<Vertex> &vertices);
	
	// colors from coolors.co (highly recommend, btw)
	const glm::u8vec4 color = glm::u8vec4(0x37, 0x71, 0x8E, 0xFF);
	const glm::u8vec4 line_color = glm::u8vec4(0x8E, 0xE3, 0xEF, 0xFF);

	// physics constants
	const float       mass                = 0.5f;  
	const float       linear_friction     = 5.0f; 
	const float       angular_friction    = 1.75f;
	const glm::vec2   paddle_radius       = glm::vec2(1.0f, 0.2f);
	const float       moment_of_inertia   = 0.4f;	
	// force of sling, in N/m (or equivalent)
	const float       sling_strength = 1.0f;
	glm::mat3x2 clip_to_paddle;
	glm::mat3x2 paddle_to_clip;

	// slingshot line
	glm::vec2   sling_start;
	glm::vec2   sling_end;
	bool        sling_held          = false;
	
	glm::vec2   mouse_pos;

	

	glm::vec2   position            = glm::vec2(0.0f, 0.0f);
	glm::vec2   linear_velocity     = glm::vec2(0.0f, 0.0f);
	
	float       normal              = 1.5707963f; // pi over 2
	float       angular_velocity    = 0.f;	
	
};
