#ifndef MZ_CLASSES_H
#define MZ_CLASSES_H

#include <raylib.h>
#include <chrono>
#include <string>

struct GridNode {
	bool m_visited;
	bool m_north;
	bool m_south;
	bool m_east;
	bool m_west;

	GridNode();
};

struct Point {
	int x;
	int y;
};

enum Jusification {
	LEFT,
	CENTER,
	RIGHT
};

class Entity {
	public:
		Entity(float posX, float posY, float width, float height, Color color);
		virtual void draw();

		float m_posX, m_posY;
		Color m_color;

	protected:
		float m_width, m_height;
};

class Text : public Entity {
	public:
		Text(float posX, float posY, float size, Color color, const char* value, Jusification alignment, Font font);
		void draw();

		const char* m_value;
		Jusification m_alignment;
		Font m_font;
};

class Display : public Text {
	public:
		Display(float posX, float posY, float size, const char* label, Color labelColor, const char* value, Color valueColor, Jusification alignment, Font font);
		void draw();
		void updateVal(int value);

		Color m_labelColor;
		const char* m_label;

	private:
		char m_buf[4];
};

class TextBox : public Entity {
	public:
		TextBox(float posX, float posY, float size, Color labelColor, std::string label, Color valueColor, Font font);
		void draw();

		float m_size;
		std::string m_label, m_value;
		Font m_font;
		Color m_valueColor;
};

class TexturedEntity : public Entity {
	public:
		TexturedEntity(float posX, float posY, float width, float height, Texture texture, bool frame);
		void toggleFrame();
		void draw();

		int m_rotation;
		bool m_flipped;
	protected:
		bool m_frame;
		Texture m_texture;
};

class Player : public TexturedEntity {
	public:
		Player(float posX, float posY, float width, float height, Texture texture, bool frame);
		void draw();

		float m_oldPosX, m_oldPosY;
		bool m_canMove;

	private:
		std::chrono::high_resolution_clock::time_point m_lastAnim;
};

#endif