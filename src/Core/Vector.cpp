
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

Vector3 getRandomDirection(){
    float randomx = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    float randomy = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    float randomz = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;

    return normalize(Vector3(randomx, randomy, randomz));

    // Alternative (and better) methods from Sebastian Lague
    // https://stackoverflow.com/questions/5825680
    // https://math.stackexchange.com/a/1585996
}