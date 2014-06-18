#version 120

varying vec3 outNormal;
varying vec3 outPosition;

uniform vec4 diffuseColor;
uniform vec3 lightPosition;

const float AMBIENT_FACTOR = 0.1;

void main() {
  // calculate diffuse brightness
  vec3 normal = normalize(outNormal);
  vec3 lightDir = lightPosition - outPosition;
  float lightDist = length(lightDir);
  float brightness = max(0.0, dot(lightDir/lightDist, normal));
  float multiplier = 1 + diffuseColor.a*(AMBIENT_FACTOR + (1.0-AMBIENT_FACTOR)*brightness - 1.0);
  gl_FragColor.xyz = multiplier * diffuseColor.xyz;
  gl_FragColor.a = diffuseColor.a;
}
