#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
	float x;
	float y;
}vect2_t;

typedef struct
{
	float x;
	float y;
	float z;
}vect3_t;

//Add fuctions to manipulate vectors 2d and 3d.
vect3_t vect3_rotate_x(vect3_t v, float angle);
vect3_t vect3_rotate_y(vect3_t v, float angle);
vect3_t vect3_rotate_z(vect3_t v, float angle);


#endif // !VECTOR_H

