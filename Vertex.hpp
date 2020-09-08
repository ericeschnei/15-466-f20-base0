#include <glm/glm.hpp>

// Vertex struct, used by Paddle, ball, and 
struct Vertex {
	Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
		Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
	glm::vec3 Position;
	glm::u8vec4 Color;
	glm::vec2 TexCoord;
};
static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "PongMode::Vertex should be packed");

