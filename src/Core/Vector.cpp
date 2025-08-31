
#include "Core/Vector.h"
#include <stdexcept>

// Default constructor that initializes a vector with all zeroes
Vector3::Vector3()
    : x(0.0f), y(0.0f), z(0.0f) {}

// Parameterized constructor
Vector3::Vector3(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_) {}

// Vector addition
Vector3 Vector3::operator+(const Vector3 &other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

// Vector subtraction
Vector3 Vector3::operator-(const Vector3 &other) const
{
    return Vector3(x - other.x, y - other.y, z - other.z);
}

// Division
Vector3 Vector3::operator/(float scalar) const
{
    if (scalar == 0.0f)
    {
        throw std::invalid_argument("Division by zero in Vector3.");
    }
    return Vector3(x / scalar, y / scalar, z / scalar);
}

// Multiplication
Vector3 Vector3::operator*(float coefficient) const
{
    return Vector3(x * coefficient, y * coefficient, z * coefficient);
}

// Length calculation
float Vector3::getLength() const
{
    return std::sqrt(x * x + y * y + z * z);
}



// Dot product
float dot(const Vector3& v1, const Vector3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Cross product
Vector3 cross(const Vector3& v1, const Vector3& v2)
{
    return Vector3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x);
}

Vector3 normalize(const Vector3& v){
float length = v.getLength();
    if (length < 1e-6)
    {
        throw std::invalid_argument("Ray direction vector cannot be zero.");
    }
    return v / length;
}

// TODO move out from here

// float randomValueNormalDistribution(unsigned int seed){
//     srand(seed);
//     float theta = 2 * 3.1415926 * rand()
//     float rho = sqrt(-2 * log)
// }

Vector3 getRandomDirection() {
    // Generate a random theta (azimuthal angle) between [0, 2π]
    float theta = 2.0f * M_PI * ((float)rand() / RAND_MAX);

    // Generate a random value for cosine of phi, between [-1, 1]
    float cosPhi = 2.0f * ((float)rand() / RAND_MAX) - 1.0f;

    // Calculate phi (polar angle), using acos to get angle in [0, π]
    float phi = acos(cosPhi);

    // Convert from spherical to Cartesian coordinates
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    // Return the normalized direction vector
    return Vector3(x, y, z);

    // Alternative (and better) methods from Sebastian Lague
    // https://stackoverflow.com/questions/5825680
    // https://math.stackexchange.com/a/1585996
}

// Return a random vector inside the hemisphere with respect to the normal vector
Vector3 getRandomDirectionInHemisphere(const Vector3& normal) {

        Vector3 randomDir = getRandomDirection();

        // Make sure it's in the hemisphere defined by the normal
        if (dot(randomDir, normal) < 0.0f) {
            randomDir = randomDir * -1.0f; // Flip the direction to be in the same hemisphere as the normal
        }

        return randomDir;
    }

Vector3 lerp(const Vector3& start, const Vector3& end, float t) {
    return start + (end - start) * t;
}

// Default constructor that initializes a vector with all zeroes
Vector2::Vector2()
    : x(0.0f), y(0.0f) {}

// Parameterized constructor
Vector2::Vector2(float x_, float y_)
    : x(x_), y(y_) {}

// Vector addition
Vector2 Vector2::operator+(const Vector2 &other) const
{
    return Vector2(x + other.x, y + other.y);
}

// Vector subtraction
Vector2 Vector2::operator-(const Vector2 &other) const
{
    return Vector2(x - other.x, y - other.y);
}

// Division
Vector2 Vector2::operator/(float scalar) const
{
    if (scalar == 0.0f)
    {
        throw std::invalid_argument("Division by zero in Vector2.");
    }
    return Vector2(x / scalar, y / scalar);
}

// Multiplication
Vector2 Vector2::operator*(float coefficient) const
{
    return Vector2(x * coefficient, y * coefficient);
}

// Length calculation
float Vector2::getLength() const
{
    return std::sqrt(x * x + y * y);
}