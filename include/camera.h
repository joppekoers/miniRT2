#pragma once
#include "bare_minimum.h"

// constants.h includes this header before it defines t_camera
typedef struct s_camera t_camera;

typedef enum __attribute__((__packed__)) e_direction
{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,

	DIRECTION_FORWARD,
	DIRECTION_BACKWARD,

	DIRECTION_PITCHUP,
	DIRECTION_PITCHDOWN,
	DIRECTION_YAWLEFT,
	DIRECTION_YAWRIGHT,
} t_direction;

void camera_pitchyaw(t_camera* self, double pitchd, double yawd);
void camera_leftright(t_camera* self, double d);
void camera_updown(t_camera* self, double d);
void camera_forwardbackward(t_camera* self, double d);
