#include "Circle.hpp"

#include <cmath>
#include <cstdlib>

extern sf::RenderWindow window;
extern float elapsed_time;

const float PI = 3.141592653589795;
const float SPEED = 200.0f;

inline float random_mass() {
	return 100000.0f * rand() / RAND_MAX + 1.0f;
}

inline float random_angle() {
	return 2.0f * PI * rand() / RAND_MAX;
}

Circle::Circle() { }

Circle::Circle(const sf::Vector2f &pos) {
	position = pos;
	mass = random_mass();

	float angle = random_angle();
	velocity = sf::Vector2f(cos(angle), sin(angle)) * SPEED;
	color = sf::Color(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56);
}

void Circle::draw() {
	static struct myShape {
		sf::CircleShape shape;

		myShape() {
			shape.setOrigin(radius, radius);
			shape.setRadius(radius);
		}
	} shape;
	
	shape.shape.setPosition(position);
	shape.shape.setFillColor(color);

	window.draw(shape.shape);
}

void Circle::update() {
//? TRUE for better cellisions will walls
#if false
	float coll_time;

	//? Y
	if (velocity.y > 0.0f)
		coll_time = (1080.0f - position.y - radius) / (velocity.y * elapsed_time);
	else
		coll_time = (radius - position.y) / (velocity.y * elapsed_time);

	if (coll_time > 1.0f)
		position.y += velocity.y * elapsed_time;
	else {
		position.y += velocity.y * elapsed_time * coll_time;
		velocity.y = -velocity.y;
		position.y += velocity.y * elapsed_time * (1.0f - coll_time);
	}

	//? X
	if (velocity.x > 0.0f)
		coll_time = (1920.0f - position.x - radius) / (velocity.x * elapsed_time);
	else
		coll_time = (radius - position.x) / (velocity.x * elapsed_time);

	if (coll_time > 1.0f)
		position.x += velocity.x * elapsed_time;
	else {
		position.x += velocity.x * elapsed_time * coll_time;
		velocity.x = -velocity.x;
		position.x += velocity.x * elapsed_time * (1.0f - coll_time);
	}
#else
	position += velocity * elapsed_time;

	if (position.x + radius > 1920.0f || position.x - radius < 0.0f) {
		velocity.x *= -1;

		position.x += velocity.x * elapsed_time;
	}

	if (position.y + radius > 1080.0f || position.y - radius < 0.0f) {
		velocity.y *= -1;

		position.y += velocity.y * elapsed_time;
	}
#endif
}
