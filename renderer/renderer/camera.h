#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"

typedef struct {
	vect3_t position;
	vect3_t direction;
	vect3_t forward_velocity;
	float yaw;
	float pitch;

} camera_t;


void init_camera(vect3_t position, vect3_t direction, vect3_t fwd_velocity, float yaw, float pitch);
vect3_t get_camera_position(void);
vect3_t get_camera_direction(void);
vect3_t get_camera_fwd_velocity(void);
float get_camera_yaw(void);
float get_camera_pitch(void);

vect3_t get_camera_look_at_target(void);

void set_camera_direction(vect3_t direction);
void set_camera_position(vect3_t position);
void set_camera_position_x(float x);
void set_camera_position_y(float y);
void set_camera_position_z(float z);
void set_camera_fwd_velocity(vect3_t fwd_velocity);
void set_camera_yaw(float yaw);
void set_camera_pitch(float pitch);
#endif 

