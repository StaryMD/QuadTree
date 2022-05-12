#pragma once

#include <SFML/Graphics.hpp>
#include "myVector.hpp"
#include "Circle.hpp"

class Quad_Tree {
	struct Node { 	
		sf::Vector2f start, end;
		myVector<Circle> circles;
		Node *child[4];
		Node *parent;
		bool is_leaf;
		uint8_t depth;

		Node(Node *, const sf::Vector2f &, const sf::Vector2f &);
		Node *advance(const Circle &);
		void overflow();
		void insert(const Circle &);
		bool is_outside(Circle &);
	};

	Node *root;

	void draw(Node *);
	void update(Node *);
	size_t check_quads(Node *);

public:
	Quad_Tree(const sf::Vector2f &, const sf::Vector2f &);
	void push(const Circle &);

	inline void draw() {
		draw(root);
	}

	inline void update() {
		update(root);
	}

	inline void check_quads() {
		check_quads(root);
	}

};
