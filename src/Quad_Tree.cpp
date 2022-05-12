#include "Quad_Tree.hpp"
#include "Circle.hpp"

extern sf::RenderWindow window;
extern bool show_quad_borders;
extern float elapsed_time;
extern size_t max_num_points;
extern size_t max_depth;

Quad_Tree::Quad_Tree(const sf::Vector2f &start, const sf::Vector2f &end) {
	root = new Node(NULL, start, end);
	root->depth = 1;
}

Quad_Tree::Node::Node(Node *parent_, const sf::Vector2f &start_, const sf::Vector2f &end_) {
	start = start_;
	end = end_;
	parent = parent_;

	if (parent)
		depth = parent->depth + 1;
	else
		depth = 0;

	is_leaf = true;

	child[0] = NULL;
	child[1] = NULL;
	child[2] = NULL;
	child[3] = NULL;
}

void Quad_Tree::push(const Circle &circle) {
	Node *it = root;

	while (!it->is_leaf)
		it = it->advance(circle);
	
	it->insert(circle);
}

Quad_Tree::Node *Quad_Tree::Node::advance(const Circle &circle) {
	auto &position = circle.position;

	if (position.x < (start.x + end.x) * 0.5f)
		if (position.y < (start.y + end.y) * 0.5f)
			return child[0];
		else
			return child[2];
	else
		if (position.y < (start.y + end.y) * 0.5f)
			return child[1];
		else
			return child[3];
}

void Quad_Tree::Node::insert(const Circle &circle) {
	circles.push_back(circle);

	if (depth <= max_depth && circles.size() > (int)max_num_points) {
		overflow();
	}
}

void Quad_Tree::Node::overflow() {
	this->is_leaf = false;

	child[0] = new Node(this, start, (start + end) * 0.5f);
	child[1] = new Node(this, sf::Vector2f((start.x + end.x) * 0.5f, start.y), sf::Vector2f(end.x, (start.y + end.y) * 0.5f));
	child[2] = new Node(this, sf::Vector2f(start.x, (start.y + end.y) * 0.5f), sf::Vector2f((start.x + end.x) * 0.5f, end.y));
	child[3] = new Node(this, (start + end) * 0.5f, end);

	for (auto &circle: circles) {
		if (circle.position.x < (start.x + end.x) * 0.5f)
			if (circle.position.y < (start.y + end.y) * 0.5f)
				child[0]->insert(circle);
			else
				child[2]->insert(circle);
		else
			if (circle.position.y < (start.y + end.y) * 0.5f)
				child[1]->insert(circle);
			else
				child[3]->insert(circle);
	}
	circles.clear();
}

bool Quad_Tree::Node::is_outside(Circle &circle) {
	auto &position = circle.position;

	return position.x < start.x || position.y < start.y || position.x > end.x || position.y > end.y;
}

void draw_rect(sf::Vector2f &start, const sf::Vector2f &end) {
	sf::Vertex line[2] = { sf::Vector2f(start.x, end.y), end };
	window.draw(line, 2, sf::Lines);
	
	line[0] = sf::Vector2f(end.x, start.y);
	window.draw(line, 2, sf::Lines);
}

void Quad_Tree::draw(Node *node) {
	if (show_quad_borders)
		draw_rect(node->start, node->end);

	if (!node->is_leaf) {
		draw(node->child[0]);
		draw(node->child[1]);
		draw(node->child[2]);
		draw(node->child[3]);
	}
	else {
		for (auto &circle: node->circles)
			circle.draw();
	}
}

inline float magnitude_sq(const sf::Vector2f &a) {
	return a.x * a.x + a.y * a.y;
}
inline float sqr(float x) {
	return x * x;
}
inline float dot_product(const sf::Vector2f &a, const sf::Vector2f &b) {
	return a.x * b.x + a.y * b.y;
}
inline float dist_squared(sf::Vector2f &a, sf::Vector2f &b) {
	return sqr(a.x - b.x) + sqr(a.y - b.y);
}

void Quad_Tree::update(Node *node) {
	if (!node->is_leaf) {
		update(node->child[0]);
		update(node->child[1]);
		update(node->child[2]);
		update(node->child[3]);
	}
	else {
		for (auto it1 = node->circles.begin(); it1 != node->circles.end(); it1++) {
			it1->update();
			
			for (auto it2 = it1 + 1; it2 != node->circles.end(); it2++) {
				const float dist_sq = dist_squared(it1->position, it2->position);

				if (dist_sq < sqr(it1->radius + it2->radius)) {
					const float constant = 2.0f * dot_product(it1->velocity - it2->velocity, it1->position - it2->position) / (dist_sq * (it1->mass + it2->mass));

					sf::Vector2f vel1 = it1->velocity - it2->mass * constant * (it1->position - it2->position);
						it2->velocity = it2->velocity - it1->mass * constant * (it2->position - it1->position);

					it1->velocity = vel1;
				}
			}
		}
	}
}

size_t Quad_Tree::check_quads(Node *node) {
	size_t ans;

	if (!node->is_leaf) {
		ans = check_quads(node->child[0]) +
			  check_quads(node->child[1]) +
			  check_quads(node->child[2]) +
			  check_quads(node->child[3]);

		if (ans <= max_num_points || node->depth > max_depth) {
			for (auto &circle: node->child[0]->circles)
				node->circles.push_back(circle);
			delete node->child[0];

			for (auto &circle: node->child[1]->circles)
				node->circles.push_back(circle);
			delete node->child[1];

			for (auto &circle: node->child[2]->circles)
				node->circles.push_back(circle);
			delete node->child[2];

			for (auto &circle: node->child[3]->circles)
				node->circles.push_back(circle);
			delete node->child[3];

			node->is_leaf = true;
		}
	}
	else {
		ans = node->circles.size();
		
		for (int i = node->circles.size() - 1; i >= 0; i--) {
			auto &circle_it = node->circles.at(i);

			if (node->is_outside(circle_it)) {
				Node *new_home = node->parent;

				while (new_home->is_outside(circle_it))
					new_home = new_home->parent;
				
				while (!new_home->is_leaf)
					new_home = new_home->advance(circle_it);

				new_home->insert(circle_it);

				node->circles.erase(node->circles.begin() + i);
			}
		}

		if (node->depth <= max_depth && node->circles.size() > (int)max_num_points) {
			node->overflow();

			check_quads(node);
		}
	}
	return ans;
}
