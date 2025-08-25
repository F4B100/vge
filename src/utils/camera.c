//
// Created by fabio on 8/21/2025.
//

#include "camera.h"

#include <string.h>

void updateView(pVgeCamera camera) {
	vec3 front = {
		cosf(camera->yaw) * cosf(camera->pitch),
		-1.0f * sinf(camera->pitch),
		sinf(camera->yaw) * cosf(camera->pitch),
	};
	glm_vec3_add(front, camera->position, front);
	glm_lookat(camera->position, front, camera->up, camera->view);
}

void updatePerspective(pVgeCamera vgeCamera) {
	glm_perspective(vgeCamera->fov, vgeCamera->aspect, vgeCamera->nearPlane, vgeCamera->farPlane, vgeCamera->perspective);
};

pVgeCamera vgeCameraCreate(float iYaw, float iPitch, float fov, float aspect, float nearPlane, float farPlane) {
	pVgeCamera camera = malloc(sizeof(vgeCamera));
	camera->yaw = iYaw;
	camera->pitch = iPitch;
	camera->fov = fov;

	camera->position[0] = 0.0f;
	camera->position[1] = 0.0f;
	camera->position[2] = 0.0f;

	camera->up[0] = 0.0f;
	camera->up[1] = 1.0f;
	camera->up[2] = 0.0f;

	updateView(camera);

	camera->aspect = aspect;
	camera->nearPlane = nearPlane;
	camera->farPlane = farPlane;
	camera->aspect = aspect;

	updatePerspective(camera);

	return camera;
}

void cameraDestroy(pVgeCamera vgeCamera) {
	free(vgeCamera);
}

void cameraSetPositon(pVgeCamera vgeCamera, vec3 newPos) {
	memcpy(vgeCamera->position, newPos, sizeof(vec3));
	updateView(vgeCamera);
}

void cameraMove(pVgeCamera vgeCamera, vec3 delta) {
	glm_vec3_add(delta, vgeCamera->position, vgeCamera->position);
	updateView(vgeCamera);
}

void cameraRotate(pVgeCamera vgeCamera, float yaw, float pitch) {
	vgeCamera->yaw += yaw;
	vgeCamera->pitch += pitch;
	updateView(vgeCamera);
}

void cameraSetRotation(pVgeCamera vgeCamera, float yaw, float pitch) {
	vgeCamera->yaw = yaw;
	vgeCamera->pitch = pitch;
	updateView(vgeCamera);
}

void cameraSetUp(pVgeCamera vgeCamera, vec3 up) {
	memcpy(vgeCamera->up, up, sizeof(vec3));
	updateView(vgeCamera);
}

vec4 *getViewMatrix(pVgeCamera vgeCamera) {
	return vgeCamera->view;
}

void cameraSetFOV(pVgeCamera vgeCamera, float fov) {
	vgeCamera->fov = fov;
	updatePerspective(vgeCamera);
}

void cameraSetAspectRatio(pVgeCamera vgeCamera, float aspect) {
	vgeCamera->aspect = aspect;
	updatePerspective(vgeCamera);
}

void cameraSetNear(pVgeCamera vgeCamera, float nearPlane) {
	vgeCamera->nearPlane = nearPlane;
	updatePerspective(vgeCamera);
}

void cameraSetFar(pVgeCamera vgeCamera, float farPlane) {
	vgeCamera->farPlane = farPlane;
	updatePerspective(vgeCamera);
}

vec4 *getPerspectiveMatrix(pVgeCamera vgeCamera) {
	return vgeCamera->perspective;
}
