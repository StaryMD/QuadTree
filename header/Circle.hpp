#pragma once

#include <SFML/Graphics.hpp>

struct Circle {
	sf::Vector2f position, velocity, acceleration;
	sf::Color color;
	float mass;
	static constexpr float radius = 1.0f;

	Circle();
	Circle(const sf::Vector2f &);

	void draw();
	void update();
};
