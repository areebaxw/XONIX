#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class MySprite {
public:
    Sprite sprite;
    Texture texture;
    string path;
    float x, y;

	MySprite() {
		x = 0;
		y = 0;
	}

    MySprite(string path, float x, float y) {
        this->path = path;

        this->x = x;
        this->y = y;
        if (!texture.loadFromFile(path)) {
            cout << "Error loading texture" << endl;
            // return;
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }
    void setPosition(float x, float y) {
        this->x = x;
        this->y = y;
        sprite.setPosition(x, y);
    }
    void setTexture(string path) {
        this->path = path;
        if (!texture.loadFromFile(path)) {
            cout << "Error loading texture" << endl;
            return;
        }
        sprite.setTexture(texture);
    }
    void setOpacity(float opacity) {
        Color color = sprite.getColor();
        color.a = static_cast<Uint8>(opacity * 255);
        sprite.setColor(color);
    }
    void Draw(RenderWindow& window) {
        window.draw(sprite);
    }

	string getPath() {
		return path;
	}

};