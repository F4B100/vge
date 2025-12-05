//
// Created by fabio on 8/21/2025.
//

#ifndef CAMERA_H
#define CAMERA_H
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

typedef struct VgeCamera {
	float yaw;
	float pitch;
	vec3 position;
	vec3 front;
	vec3 up;
	mat4 view;
	float fov;
	float aspect;
	float nearPlane;
	float farPlane;
	mat4 perspective;
}vgeCamera, *pVgeCamera;

pVgeCamera vgeCameraCreate(float iYaw, float iPitch, float fov, float aspect, float nearPlane, float farPlane);
void cameraDestroy(pVgeCamera vgeCamera);

void cameraSetPositon(pVgeCamera vgeCamera, vec3 newPos);
void cameraMove(pVgeCamera vgeCamera, vec3 delta);
void cameraRotate(pVgeCamera vgeCamera, float yaw, float pitch);
void cameraSetRotation(pVgeCamera vgeCamera, float yaw, float pitch);
void cameraGetRotation(pVgeCamera vgeCamera, float *yaw, float *pitch);
void cameraSetUp(pVgeCamera vgeCamera, vec3 up);
void cameraLookAt(pVgeCamera vgeCamera, vec3 target);
vec4 *getViewMatrix(pVgeCamera vgeCamera);
void cameraGetFront(pVgeCamera camera, float *front);
float *cameraGetPos(pVgeCamera camera);

void cameraSetFOV(pVgeCamera vgeCamera, float fov);
void cameraSetAspectRatio(pVgeCamera vgeCamera, float aspect);
void cameraSetNear(pVgeCamera vgeCamera, float nearPlane);
void cameraSetFar(pVgeCamera vgeCamera, float farPlane);
vec4 *getPerspectiveMatrix(pVgeCamera vgeCamera);
#include "../graphics/model/modelDescriptors.h"
void cameraBindMatrices(pVgeCamera camera, pVulkanContext context, pVgeDescriptor descriptor, uint32_t binding, uint32_t offset);


#endif //CAMERA_H
