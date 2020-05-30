#include <math.h>

#include "game001.h"

//multiplies m1 and m2, saving the result in dest.
float *mat4Multiply(const float *m1, const float *m2, float *dest)
{
	float tmp[16];
	int i, j, k;

	for(i = 0; i < 16; i++)
		tmp[i] = 0;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			for(k = 0; k < 4; k++)
				tmp[k*4+i] += m1[j*4+i] * m2[k*4+j];

	for(i = 0; i < 16; i++)
		dest[i] = tmp[i];

	return dest;
}

//sets the array m to the identity matrix
float *mat4Identity(float *m)
{
	int i, j;

	for(i = 0, j = 5; i < 16; i++, j++) {
		if(j > 4) {
			j = 0;
			m[i] = 1;
		} else {
			m[i] = 0;
		}
	}

	return m;
}

//multiplies m by a translation matrix created from x and y.
float *mat4Translate(float *m, float x, float y)
{
	float trans[16];
	mat4Identity(trans);

	trans[12] = x;
	trans[13] = y;

	mat4Multiply(m, trans, m);

	return m;
}

//multiplies m by a scaling matrix created from x and y.
float *mat4Scale(float *m, float x, float y)
{
	float scale[16];
	mat4Identity(scale);
	
	scale[0] = x;
	scale[5] = y;

	mat4Multiply(m, scale, m);

	return m;
}

//multiplies m by a 2D rotation matrix.
float *mat4Rotate(float *m, float angle)
{
	float rotate[16], c, s;

	c = cosf(angle);
	s = sinf(angle);

	mat4Identity(rotate);

	rotate[0] = c;
	rotate[1] = s;
	rotate[4] = -s;
	rotate[5] = c;

	mat4Multiply(m, rotate, m);

	return m;
}

//creates an orthographic projection which is stored in m
float *mat4Ortho(float *m, float left, float right, float bottom, float top)
{
	mat4Identity(m);

	m[0] = 2/(right-left);
	m[5] = 2/(top-bottom);
	m[10] = -1;

	m[12] = -(right+left)/(right-left);
	m[13] = -(top+bottom)/(top-bottom);
	m[14] = 0;

	return m;
}
