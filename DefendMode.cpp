#include "DefendMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

DefendMode::DefendMode() {


	//----- allocate OpenGL resources -----
	{ //vertex buffer:
		glGenBuffers(1, &vertex_buffer);
		//for now, buffer will be un-filled.

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}

	{ //vertex array mapping buffer for color_texture_program:
		//ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
		glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

		//set vertex_buffer_for_color_texture_program as the current vertex array object:
		glBindVertexArray(vertex_buffer_for_color_texture_program);

		//set vertex_buffer as the source of glVertexAttribPointer() commands:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		//set up the vertex array object to describe arrays of DefendMode::Vertex:
		glVertexAttribPointer(
			color_texture_program.Position_vec4, //attribute
			3, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 0 //offset
		);
		glEnableVertexAttribArray(color_texture_program.Position_vec4);
		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

		glVertexAttribPointer(
			color_texture_program.Color_vec4, //attribute
			4, //size
			GL_UNSIGNED_BYTE, //type
			GL_TRUE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 4*3 //offset
		);
		glEnableVertexAttribArray(color_texture_program.Color_vec4);

		glVertexAttribPointer(
			color_texture_program.TexCoord_vec2, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 4*3 + 4*1 //offset
		);
		glEnableVertexAttribArray(color_texture_program.TexCoord_vec2);

		//done referring to vertex_buffer, so unbind it:
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//done setting up vertex array object, so unbind it:
		glBindVertexArray(0);

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}

	{ //solid white texture:
		//ask OpenGL to fill white_tex with the name of an unused texture object:
		glGenTextures(1, &white_tex);

		//bind that texture object as a GL_TEXTURE_2D-type texture:
		glBindTexture(GL_TEXTURE_2D, white_tex);

		//upload a 1x1 image of solid white to the texture:
		glm::uvec2 size = glm::uvec2(1,1);
		std::vector< glm::u8vec4 > data(size.x*size.y, glm::u8vec4(0xff, 0xff, 0xff, 0xff));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		//set filtering and wrapping parameters:
		//(it's a bit silly to mipmap a 1x1 texture, but I'm doing it because you may want to use this code to load different sizes of texture)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//since texture uses a mipmap and we haven't uploaded one, instruct opengl to make one for us:
		glGenerateMipmap(GL_TEXTURE_2D);

		//Okay, texture uploaded, can unbind it:
		glBindTexture(GL_TEXTURE_2D, 0);

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}
}

DefendMode::~DefendMode() {

	//----- free OpenGL resources -----
	glDeleteBuffers(1, &vertex_buffer);
	vertex_buffer = 0;

	glDeleteVertexArrays(1, &vertex_buffer_for_color_texture_program);
	vertex_buffer_for_color_texture_program = 0;

	glDeleteTextures(1, &white_tex);
	white_tex = 0;
}

bool DefendMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	
	glm::vec2 clip_mouse;
	switch (evt.type) {
		case SDL_MOUSEMOTION:
			//convert mouse from window pixels (top-left origin, +y is down) to clip space ([-1,1]x[-1,1], +y is up):
			clip_mouse = glm::vec2(
				(evt.motion.x + 0.5f) / window_size.x * 2.0f - 1.0f,
				(evt.motion.y + 0.5f) / window_size.y *-2.0f + 1.0f
			);
			paddle.mouse_pos = clip_to_court * glm::vec3(clip_mouse, 1.0f);
			return true;
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			clip_mouse = glm::vec2(
				(evt.button.x + 0.5f) / window_size.x * 2.0f - 1.0f,
				(evt.button.y + 0.5f) / window_size.y *-2.0f + 1.0f
			);

			if (evt.button.button == SDL_BUTTON_LEFT) {
				paddle.handle_click(
					(clip_to_court * glm::vec3(clip_mouse, 1.0f)), 
					evt.type == SDL_MOUSEBUTTONDOWN
				);
				return true;
			}

			break;
	}

	return false;
}

void DefendMode::update(float elapsed) {

	static std::mt19937 mt; //mersenne twister pseudo-random number generator

	
	size_t i = 0;
	while (i < balls.size()) {
		Ball *b = balls[i];
		
		// update ball positions
		b->update(elapsed);

		// if the paddle is touching a ball, delete it
		if (paddle.is_colliding(*b)) {
			// https://stackoverflow.com/a/46282890
			balls[i] = balls.back();
			delete b;
			balls.pop_back();
			player_score++;
		// if any balls made it to the center, delete them
		} else if (glm::length(b->position) < this->center_radius) {
			// https://stackoverflow.com/a/46282890
			balls[i] = balls.back();
			delete b;
			balls.pop_back();
			player_lives--;
			// player ran out of lives, end game immediately
			if (player_lives == 0) {
				for (Ball *bb : balls) {
					delete bb;
				}
				balls.clear();
				
				break;
			}
		} else {
			i++;
		}

	}

	// spawn new balls
	if (player_lives > 0) {
		ball_timer += elapsed;
		while(ball_timer > ball_period) {
			ball_timer -= ball_period;

			float start_angle = mt() * 6.283185f;
			glm::vec2 start_pos = glm::vec2(
				glm::cos(start_angle) * 10.0f,
				glm::sin(start_angle) * 10.0f
			);

			Ball *b = new Ball(start_pos, this->ball_speed);
			this->ball_speed += this->ball_speed_inc;
			
			balls.push_back(b);

		}
		paddle.update(elapsed);
	}

}

void DefendMode::draw(glm::uvec2 const &drawable_size) {

	glm::u8vec4 bg_color = glm::u8vec4(0x14, 0x2B, 0x3E, 0xff);
	//---- compute vertices to draw ----

	//vertices will be accumulated into this list and then uploaded+drawn at the end of this function:
	std::vector< Vertex > vertices;

	// draw main circle	
	const int center_num_segments = 48;
	const glm::u8vec4 center_color = glm::u8vec4(0x0A, 0x15, 0x1F, 0xff);

	auto add_triangle = [center_color, &vertices](
		const glm::vec2 &p1, const glm::vec2 &p2
	) {
		// define CCW tri
		vertices.emplace_back(glm::vec3(p1, 0.0f), center_color, glm::vec2(0.0f, 0.0f));
		vertices.emplace_back(glm::vec3(p2, 0.0f), center_color, glm::vec2(0.0f, 0.0f));
		vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), center_color, glm::vec2(0.0f, 0.0f));
	};

	// draw a circle
	glm::vec2 last_pos;
	for (int i = 0; i <= center_num_segments; i++) {
		float angle = ((float) i) / center_num_segments * 6.28319f;
		glm::vec2 pos = glm::vec2(
			center_radius * glm::cos(angle),
			center_radius * glm::sin(angle)
		);

		if (i > 0) {
			add_triangle(last_pos, pos);
		}
		last_pos = pos;
	}


	//inline helper function for rectangle drawing:
	auto draw_rectangle = [&vertices](glm::vec2 const &center, glm::vec2 const &radius, glm::u8vec4 const &color) {
		//draw rectangle as two CCW-oriented triangles:
		vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x+radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));

		vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x-radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
	};

	glm::vec2 score_radius = glm::vec2(0.1f, 0.1f);
	glm::u8vec4 score_color = glm::u8vec4(0x8E, 0xE3, 0xEF, 0xFF);
	glm::u8vec4 lives_color = glm::u8vec4(0xC3, 0x3C, 0x54, 0xEF);
	glm::vec2 score_padding = glm::vec2(0.2f, 1.0f);

	// draw score
	for (size_t i = 0; i < player_score; i++) {
		draw_rectangle(
			glm::vec2(
				-court_radius.x + score_padding.x + (3.0f * i * score_radius.x),
				court_radius.y - score_padding.y
			), 
			score_radius, score_color
		);
	}

	// draw lives
	for (size_t i = 0; i < player_lives; i++) {
		draw_rectangle(
			glm::vec2(
				court_radius.x - score_padding.x - (3.0f * i * score_radius.x),
				court_radius.y - score_padding.y
			),
			score_radius, lives_color
		);
	}


	//solid objects:
	for (Ball *b : balls) { 
		b->draw(vertices);
	}
	paddle.draw(vertices);



	//------ compute court-to-window transform ------

	//compute area that should be visible:
	glm::vec2 scene_min = glm::vec2(
		-court_radius.x,
		-court_radius.y
	);
	glm::vec2 scene_max = glm::vec2(
		court_radius.x,
		court_radius.y
	);

	//compute window aspect ratio:
	float aspect = drawable_size.x / float(drawable_size.y);
	//we'll scale the x coordinate by 1.0 / aspect to make sure things stay square.

	//compute scale factor for court given that...
	float scale = std::min(
		(2.0f * aspect) / (scene_max.x - scene_min.x), //... x must fit in [-aspect,aspect] ...
		(2.0f) / (scene_max.y - scene_min.y) //... y must fit in [-1,1].
	);

	glm::vec2 center = 0.5f * (scene_max + scene_min);

	//build matrix that scales and translates appropriately:
	glm::mat4 court_to_clip = glm::mat4(
		glm::vec4(scale / aspect, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, scale, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(-center.x * (scale / aspect), -center.y * scale, 0.0f, 1.0f)
	);
	//NOTE: glm matrices are specified in *Column-Major* order,
	// so each line above is specifying a *column* of the matrix(!)

	//also build the matrix that takes clip coordinates to court coordinates (used for mouse handling):
	clip_to_court = glm::mat3x2(
		glm::vec2(aspect / scale, 0.0f),
		glm::vec2(0.0f, 1.0f / scale),
		glm::vec2(center.x, center.y)
	);

	//---- actual drawing ----

	//clear the color buffer:
	glClearColor(bg_color.r / 255.0f, bg_color.g / 255.0f, bg_color.b / 255.0f, bg_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	//upload vertices to vertex_buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STREAM_DRAW); //upload vertices array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set color_texture_program as current program:
	glUseProgram(color_texture_program.program);

	//upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(color_texture_program.OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(court_to_clip));

	//use the mapping vertex_buffer_for_color_texture_program to fetch vertex data:
	glBindVertexArray(vertex_buffer_for_color_texture_program);

	//bind the solid white texture to location zero so things will be drawn just with their colors:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, white_tex);

	//run the OpenGL pipeline:
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(vertices.size()));

	//unbind the solid white texture:
	glBindTexture(GL_TEXTURE_2D, 0);

	//reset vertex array to none:
	glBindVertexArray(0);

	//reset current program to none:
	glUseProgram(0);
	

	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

}
