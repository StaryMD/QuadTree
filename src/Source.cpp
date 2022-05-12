#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <string>

#include "Circle.hpp"
#include "Quad_Tree.hpp"

sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Quad Tree", sf::Style::Fullscreen);
bool show_quad_borders = false;
size_t max_depth = 4;
size_t max_num_points = 4;
float elapsed_time;

int main() {
	Quad_Tree qtree(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(window.getSize().x - 1, window.getSize().y - 1));

	bool LMB_was_pressed = false;
	bool RMB_was_pressed = false;
	bool left_arrow_was_pressed = false;
	bool right_arrow_was_pressed = false;
	bool up_arrow_was_pressed = false;
	bool down_arrow_was_pressed = false;
	int obj_count = 0;

	const float wanted_time_per_frame = 1.0f / 60.0f;
	float time_since_last_frame = 0.0f;

	sf::Font font;
	font.loadFromFile("fonts/Lato-Regular.ttf");

	sf::Text text;
	text.setFillColor(sf::Color::Green);
	text.setFont(font);

	
	constexpr int fps_buff_len = 120;
	float last_fps[fps_buff_len] = { 0.0f };
	int last_fps_idx = fps_buff_len;
	auto min_fps = [&last_fps]() {
		float min = 100000.0f;

		for (float &x: last_fps)
			min = std::min(min, x);
		
		return min;
	};
	auto avg_fps = [&last_fps]() {
		float sum = 0.0f;

		for (float &x: last_fps)
			sum += x;
		
		return sum / fps_buff_len;
	};


	sf::Event event;
	std::chrono::system_clock::time_point last_frame_start = std::chrono::system_clock::now();
	while (window.isOpen()) {
		std::chrono::duration<float> elapsed_time_duration = std::chrono::system_clock::now() - last_frame_start;
		elapsed_time = elapsed_time_duration.count();
		time_since_last_frame += elapsed_time;
		last_frame_start = std::chrono::system_clock::now();

		while (window.pollEvent(event)) {
#pragma GCC diagnostic ignored "-Wswitch"
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			if (!LMB_was_pressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				qtree.push(Circle(sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)));
				obj_count++;
			}
		}
		else {
			LMB_was_pressed = false;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
			if (!RMB_was_pressed) {
				show_quad_borders = !show_quad_borders;
				RMB_was_pressed = true;
			}
		}
		else {
			RMB_was_pressed = false;
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			if (!left_arrow_was_pressed) {
				left_arrow_was_pressed = true;
				max_depth = std::max(0, (int)max_depth - 1);
			}
		}
		else {
			left_arrow_was_pressed = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			if (!right_arrow_was_pressed) {
				right_arrow_was_pressed = true;
				max_depth++;
			}
		}
		else {
			right_arrow_was_pressed = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			if (!up_arrow_was_pressed) {
				up_arrow_was_pressed = true;
				max_num_points++;
			}
		}
		else {
			up_arrow_was_pressed = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			if (!down_arrow_was_pressed) {
				down_arrow_was_pressed = true;
				max_num_points = std::max(1, (int)max_num_points - 1);
			}
		}
		else {
			down_arrow_was_pressed = false;
		}
		
		sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bool((elapsed_time = 0.0f));
		sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && bool(elapsed_time *= 0.4f);
		sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && bool(elapsed_time *= 0.5f);

		float fps = 1.0f / elapsed_time;
		last_fps[++last_fps_idx % fps_buff_len] = fps;
		
		std::string fps_str = std::to_string(int(avg_fps())) + " " + std::to_string(int(min_fps()));
		std::string obj_str = std::to_string(obj_count);

		if (time_since_last_frame >= wanted_time_per_frame) {
			time_since_last_frame = 0.0f;
			
			int color = 0;
			window.clear(sf::Color(color, color, color));

			qtree.draw();

			text.setString(fps_str + "\n" + obj_str + "\n" + std::to_string(max_depth) + "\n" + std::to_string(max_num_points));

			window.draw(text);
			window.display();
		}

		qtree.update();
		qtree.check_quads();

		//window.setTitle("Quad Tree | FPS: " + fps_str + " | Objs: " + obj_str);
	}

	return 0;
}
