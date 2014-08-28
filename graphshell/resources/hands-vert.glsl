#version 120

varying vec3 worldNormal;
varying vec3 worldPosition;
varying vec4 vertexColor;

attribute vec3 vertex;
attribute vec3 normal;
attribute vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 transformMatrix;
uniform mat4 normalMatrix;

void main()
{
  vec4 pos = (transformMatrix * vec4(vertex, 1.0));
  worldNormal = (normalMatrix * vec4(normal, 0.0)).xyz;
  worldPosition = pos.xyz;
  vertexColor = color;
  gl_Position = projectionMatrix * pos;
}
