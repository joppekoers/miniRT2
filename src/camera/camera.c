#include "constants.h"
#include "camera.h"

void camera_pitchyaw(t_camera* self, double pitchd, double yawd)
{
	const double pitch = asin(self->dir.y) + pitchd;
	const double yaw = atan2(self->dir.x, self->dir.z) + yawd;
	const double cos_pitch = cos(pitch);

	self->dir.x = sin(yaw) * cos_pitch;
	self->dir.y = sin(pitch);
	self->dir.z = cos(yaw) * cos_pitch;
	normalize(&self->dir);
}

void camera_leftright(t_camera* self, double d)
{
	const t_vec3 left = unit(cross(self->dir, vec3(0.0, 1.0, 0.0)));
	self->origin = translate(self->origin, left, d);
}

void camera_updown(t_camera* self, double d)
{
	self->origin.y += d;
}

void camera_forwardbackward(t_camera* self, double d)
{
	self->origin = translate(self->origin, self->dir, d);
}

void camera_serialise(t_camera* self, FILE* f)
{
	assert(fwrite(self, sizeof *self, 1, f) == 1);
}

void camera_deserialise(t_camera* self, FILE* f)
{
	assert(fread(self, sizeof *self, 1, f) == 1);
}
