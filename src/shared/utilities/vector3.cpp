#include "shared/utilities/vector3.hpp"

void Vector3::setVector(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3 Vector3::operator + (const Vector3& v) {
	Vector3 tmp;
	tmp.x = x + v.x;
	tmp.y = y + v.y;
	tmp.z = z + v.z;

	return tmp;
}

std::string Vector3::to_string() {
	//	Return a string representation of this vector
	return "("
		+ std::to_string(this->x) + ", "
		+ std::to_string(this->y) + ", "
		+ std::to_string(this->z) + ")";
}