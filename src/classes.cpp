#include "classes.h"

#include <raylib.h>
#include <chrono>
#include <cmath>
using namespace std;

GridNode::GridNode() {
	m_visited = false;
	m_north = false;
	m_south = false;
	m_east = false;
	m_west = false;
}

Entity::Entity(float posX, float posY, float width, float height, Color color) : m_posX{ posX }, m_posY{ posY }, m_width{ width }, m_height{ height }, m_color{ color } {}
void Entity::draw() {
	DrawRectangle(m_posX, m_posY, m_width, m_height, m_color);
}
Text::Text(float posX, float posY, float size, Color color, const char* value, Jusification alignment, Font font) : Entity(posX, posY, size, 0, color), m_value{ value }, m_alignment{ alignment }, m_font{ font } {}
void Text::draw() {
	float posX = 0;
	if (m_alignment == LEFT) posX = m_posX;
	else if (m_alignment == CENTER) posX = m_posX - MeasureTextEx(m_font, m_value, m_width, 1).x / 2;
	else if (m_alignment == RIGHT) posX = m_posX - MeasureTextEx(m_font, m_value, m_width, 1).x;

	DrawTextEx(m_font, m_value, { posX, m_posY - m_width / 2 }, m_width, 1, m_color);
}

Display::Display(float posX, float posY, float size, const char* label, Color labelColor, const char* value, Color valueColor, Jusification alignment, Font font) : Text(posX, posY, size, valueColor, value, alignment, font), m_label{ label }, m_labelColor{ labelColor } {}
void Display::draw() {
	float posX = 0;
	float trueWidth = MeasureTextEx(m_font, m_label, m_width, 1).x + MeasureTextEx(m_font, m_value, m_width, 1).x;
	if (m_alignment == LEFT) posX = m_posX;
	else if (m_alignment == CENTER) posX = m_posX - trueWidth / 2;
	else if (m_alignment == RIGHT) posX = m_posX - trueWidth;

	DrawTextEx(m_font, m_label, { posX, m_posY - m_width / 2 }, m_width, 1, m_labelColor);
	DrawTextEx(m_font, m_value, { posX + MeasureTextEx(m_font, m_label, m_width, 1).x, m_posY - m_width / 2 }, m_width, 1, m_color);
}
void Display::updateVal(int value) {
	sprintf_s(m_buf, "%d", value);
	m_value = m_buf;
}

TextBox::TextBox(float posX, float posY, float size, Color labelColor, string label, Color valueColor, Font font) : Entity(posX, posY, size, 0, labelColor), m_font{ font }, m_label{ label }, m_value{ "" }, m_valueColor{ valueColor } {}
void TextBox::draw() {
	DrawTextEx(m_font, m_label.c_str(), {m_posX, m_posY - m_width / 2}, m_width, 1, m_color);
	DrawTextEx(m_font, m_value.c_str(), { m_posX + MeasureTextEx(m_font, m_label.c_str(), m_width, 1).x, m_posY - m_width / 2}, m_width, 1, m_valueColor);
}

TexturedEntity::TexturedEntity(float posX, float posY, float width, float height, Texture texture, bool frame) : Entity(posX, posY, width, height, WHITE), m_texture{ texture }, m_rotation{ 0 }, m_flipped{ false }, m_frame{ frame } {}
void TexturedEntity::toggleFrame() {
	m_frame = !m_frame;
}
void TexturedEntity::draw() {
	DrawTexturePro(m_texture, { 0, 0, (float)m_texture.width * m_frame, (float)m_texture.height * m_frame }, {m_posX + m_width / 2, m_posY + m_height / 2, m_width, m_height}, {m_width / 2, m_height / 2}, m_rotation, m_color);
}

Player::Player(float posX, float posY, float width, float height, Texture texture, bool frame) : TexturedEntity(posX, posY, width, height, texture, frame), m_canMove{ true }, m_lastAnim{ chrono::high_resolution_clock::now() }, m_oldPosX{ posX }, m_oldPosY{ posY } {}
void Player::draw() {
	if ((m_oldPosX != m_posX || m_oldPosY != m_posY) && (chrono::high_resolution_clock::now() - m_lastAnim).count() > (1.0 / 15.0 * 1000000000)) {
		m_frame = !m_frame;
		m_lastAnim = chrono::high_resolution_clock::now();
	}
	if (m_oldPosX == m_posX && m_oldPosY == m_posY) {
		m_frame = false;
	}
	DrawTexturePro(m_texture, { (m_frame ? m_texture.width / 2.0f : 0.0f), 0, m_flipped ? -m_texture.width / 2.0f : m_texture.width / 2.0f, (float)m_texture.height }, { m_posX + m_width / 2, m_posY + m_height / 2, m_width, m_height }, { m_width / 2, m_height / 2 }, m_rotation, m_color);
}