#include "GL.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.hpp"
#include "Ball.hpp"

struct Paddle {
	
	Paddle();
	~Paddle();
	
	void handle_collision(Ball &ball);
	void update(float elapsed);
	void draw(std::vector<Vertex> &vertices);
	
	// colors from coolors.co (highly recommend, btw)
	const glm::u8vec4 color = glm::u8vec4(0xFF, 0x88, 0x00, 0xFF);

	// physics constants
	const float       mass                = 1.0f;  
	const float       linear_friction     = 0.0f; 
	const float       angular_friction    = 0.0f;

	const glm::vec2   paddle_radius       = glm::vec2(1.0f, 0.2f);


	glm::vec2   position            = glm::vec2(0.0f, 0.0f);
	glm::vec2   linear_velocity     = glm::vec2(0.0f, 0.0f);
	
	float       normal              = 1.5707963f; // pi over 2
	float       angular_velocity    = 0.0f;	
	
};
