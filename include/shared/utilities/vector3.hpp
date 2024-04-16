#pragma once
#include <string>

class Vector3 {
public:
	float x;
	float y;
	float z;
	Vector3() : Vector3(0, 0, 0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	/**
	 * @brief Sets this Vector3's values to the given arguments.
	 * @param x
	 * @param y
	 * @param z
	 */
	void setVector(float x, float y, float z);
	Vector3 operator + (const Vector3& v);
	std::string to_string();
};