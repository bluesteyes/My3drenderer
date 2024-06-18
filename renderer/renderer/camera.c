#include "camera.h"
#include "matrix.h"

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

vect3_t get_camera_look_at_target(void) {
	//initialize the target looking at the positive z-axis
	vect3_t target = { 0, 0, 1 };

	//apply yaw rotation to vectors
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);

	//create camera rotation matrix based on yaw, pitch and roll
	mat4_t camera_rotation = mat4_identity();
	camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);
	camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);

	//update camera direction based on the camera rotation
	vect4_t camera_direction = mat4_mul_vect4(camera_rotation, vect4_from_vect3(target));
	camera.direction = (vect3_from_vect4(camera_direction));

	//offset the camera position in the direction where the camera is pointing at
	target = vect3_add(camera.position, camera.direction);

	return target;
}

