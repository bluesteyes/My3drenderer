#include "camera.h"

camera_t camera;

void init_camera(vect3_t position, vect3_t direction, vect3_t fwd_velocity, float yaw, float pitch) {

	camera.position = position;
	camera.direction = direction;
	camera.forward_velocity = fwd_velocity;
	camera.yaw = yaw;
	camera.pitch = pitch;

}

vect3_t get_camera_position(void) {
	return camera.position;
}

vect3_t get_camera_direction(void) {
	return camera.direction;
}

vect3_t get_camera_fwd_velocity(void) {
	return camera.forward_velocity;
}

float get_camera_yaw(void) {
	return camera.yaw;
}

float get_camera_pitch(void) {
	return camera.pitch;
}

void set_camera_direction(vect3_t direction) {
	camera.direction = direction;
}

void set_camera_position(vect3_t position) {
	camera.position = position;
}

void set_camera_position_x(float x) {
	camera.position.x = x;
}

void set_camera_position_y(float y) {
	camera.position.y = y;
}

void set_camera_position_z(float z) {
	camera.position.z = z;
}


void set_camera_fwd_velocity(vect3_t fwd_velocity) {
	camera.forward_velocity = fwd_velocity;
}

void set_camera_yaw(float yaw) {
	camera.yaw = yaw;
}
void set_camera_pitch(float pitch) {
	camera.pitch = pitch;
}

