#include "../includes.h"

float GetYaw(Unity::Vector3& v1, Unity::Vector3& v2) {
    float deltaX = v2.x - v1.x;
    float deltaZ = v2.z - v1.z;

    return (std::atan2(deltaX, deltaZ) * 180.f) / M_PI;
}

float GetPitch(Unity::Vector3& v1, Unity::Vector3& v2) {
    float deltaX = v2.x - v1.x;
    float deltaY = v2.z - v1.z;
    float deltaZ = v2.y - v1.y;

    float horizontalDistance = std::sqrt(deltaY * deltaY + deltaX * deltaX);
    return -(std::atan2(deltaZ, horizontalDistance) * 180.0f) / M_PI;
}

float Lerp(float start, float end, float t) {
    return start + (end - start) * t;
}

float NormalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}