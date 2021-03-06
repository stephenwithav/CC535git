#include "LightEnvironment.h"
#include "shadowMap.h"

LightEnvironment::LightEnvironment(Light* ambient) {
	ambientLight = ambient;
}

void LightEnvironment::setCameraPos(V3 pos) {
	cameraPos = pos;
}

V3 color, viewDirection, lightDirection, colDiff, colSpec, halfway, diffLight;
float lightDistance, specularPower, ambientStrength, diffStrength, specularStrength, lightStrength;

V3 LightEnvironment::getLightingAtVertex(Material* mat, V3 point, V3 uvw, V3 normal) {
	viewDirection = (cameraPos-point).norm();
	lightDirection = lights[0]->position - point;
	lightDistance = lightDirection.len();
	//normalize more efficiently:
	lightDirection = lightDirection.norm();
	specularPower = mat->getSpecularPower();
	colDiff = mat->getDiffuse()->getColorV3(uvw);
	colSpec = V3(1, 1, 1);// true ? V3(0, 0, 0) : V3(mat->getSpecular()->getColor(uvw[0], uvw[1]));
	ambientStrength = 0.1;
	//
	//far off, no light present:
	if (lightDistance > lights[0]->rangeMax) {
		//colDiff = V3(1, 0.5, 0.5);
		color =
			colDiff ^ ambientLight->color * ambientStrength; //ambience
	}
	//full light strength:
	else if (lightDistance < lights[0]->rangeStart) {
		//colDiff = V3(1, 0.5, 0.5);
		//halfway vec for specular:
		halfway = (lightDirection + viewDirection).norm();
		//strengths of elements:
		diffStrength = fmaxf(lightDirection * normal, 0);
		specularStrength = fmaxf(powf(halfway * normal,200/*mat->getSpecularPower()*/), 0);
		//light elements:
		diffLight = lights[0]->color * diffStrength;
		color =
			colDiff ^ ambientLight->color * ambientStrength +	//ambience
			colDiff ^ diffLight.clamp() +						//diffuse light
			colSpec * specularStrength;							//specular
	}
	//in between:
	else {
		//colDiff = V3(1, 0.5, 0.5);
		//strength of the overal light based on distance:
		lightStrength = (1 - ((lightDistance - lights[0]->rangeStart) / (lights[0]->diff)));
		lightStrength = lightStrength < 0 ? 0 : lightStrength;
		//halfway vec for specular:
		halfway = (lightDirection + viewDirection).norm();
		//strengths of elements:
		diffStrength = fmaxf(lightDirection * normal, 0);
		specularStrength = fmaxf(powf(halfway * normal, 200/*mat->getSpecularPower()*/), 0);
		//light elements:
		diffLight = lights[0]->color * diffStrength;
		color =
			colDiff ^ ambientLight->color * ambientStrength +	//ambience
			(colDiff ^ diffLight.clamp()) * lightStrength +		//diffuse light
			(colSpec * specularStrength) * lightStrength;		//specular
	}
	return color.clamp();
}


V3 LightEnvironment::getLightingAtVertexShadow(Material* mat, V3 point, V3 uvw, V3 normal) {
	viewDirection = (cameraPos - point).norm();
	if (lights.size() <= 0)
		return V3(1,0,1);
	lightDirection = lights[0]->position - point;
	lightDistance = lightDirection.len();
	//normalize more efficiently:
	lightDirection = lightDirection.norm();
	specularPower = mat->getSpecularPower();
	//shadow
	colDiff = mat->getDiffuse()->getColorV3(uvw);
	//colDiff = V3(1, 0.5, 0.5);
	//colDiff = shadowMap->dirToColor(shadowMap->getMapColorCoding(point));
	colSpec = V3(1, 1, 1);// true ? V3(0, 0, 0) : V3(mat->getSpecular()->getColor(uvw[0], uvw[1]));
	ambientStrength = 0.1;
	//
	//far off, no light present:
	if (lightDistance > lights[0]->rangeMax) {
		//colDiff = V3(1, 0.5, 0.5);
		color =
			colDiff ^ ambientLight->color * ambientStrength; //ambience
	}
	//full light strength:
	else if (lightDistance < lights[0]->rangeStart) {
		float shadowPower = shadowMap->getMapValue(point);
		//colDiff = V3(1, 0.5, 0.5);
		//halfway vec for specular:
		halfway = (lightDirection + viewDirection).norm();
		//strengths of elements:
		diffStrength = fmaxf(lightDirection * normal, 0);
		specularStrength = fmaxf(powf(halfway * normal, 200/*mat->getSpecularPower()*/), 0);
		//light elements:
		diffLight = lights[0]->color * diffStrength;
		color =
			(colDiff ^ ambientLight->color * ambientStrength +	//ambience
			colDiff ^ diffLight.clamp() +						//diffuse light
			colSpec * specularStrength)*shadowPower;			//specular
	}
	//in between:
	else {
		float shadowPower = shadowMap->getMapValue(point);
		//colDiff = V3(1, 0.5, 0.5);
		//strength of the overal light based on distance:
		lightStrength = (1 - ((lightDistance - lights[0]->rangeStart) / (lights[0]->diff)))*shadowPower;
		lightStrength = lightStrength < 0 ? 0 : lightStrength;
		//halfway vec for specular:
		halfway = (lightDirection + viewDirection).norm();
		//strengths of elements:
		diffStrength = fmaxf(lightDirection * normal, 0);
		specularStrength = fmaxf(powf(halfway * normal, 200/*mat->getSpecularPower()*/), 0);
		//light elements:
		diffLight = lights[0]->color * diffStrength;
		color =
			colDiff ^ ambientLight->color * ambientStrength +	//ambience
			(colDiff ^ diffLight.clamp()) * lightStrength +		//diffuse light
			(colSpec * specularStrength) * lightStrength;		//specular
	}
	return color.clamp();
}

void LightEnvironment::setAmbient(Light* l) {
	ambientLight = l;
}

void LightEnvironment::addLight(Light* l) {
	lights.insert(lights.end(), l);
}

void LightEnvironment::removeLight(int idx) {
	lights.erase(lights.begin() + idx);
}