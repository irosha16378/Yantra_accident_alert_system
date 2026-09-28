#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <Arduino.h>

/**
 * @class Vector3D
 * @brief OOP Encapsulation for 3D spatial vectors (Accelerometer / Gyroscope readings)
 * 
 * Demonstrates: Encapsulation, Operator Overloading, Utility Methods
 */
class Vector3D {
public:
    float x;
    float y;
    float z;

    // Constructors
    Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3D(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

    // Vector Magnitude: sqrt(x^2 + y^2 + z^2)
    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Zero out vector
    void zero() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    // Operator Overloading for OOP Math operations
    Vector3D operator+(const Vector3D& rhs) const {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3D operator-(const Vector3D& rhs) const {
        return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(float scalar) const {
        if (std::abs(scalar) < 1e-6f) return Vector3D(0.0f, 0.0f, 0.0f);
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    Vector3D& operator+=(const Vector3D& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    void print(const char* label = "Vector") const {
        Serial.printf("[%s] X: %6.2f | Y: %6.2f | Z: %6.2f\n", label, x, y, z);
    }
};

#endif // VECTOR3D_H
