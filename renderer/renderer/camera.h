#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"

typedef struct {
	vect3_t position;
	vect3_t direction;
} camera_t;


extern camera_t camera;

#endif 
