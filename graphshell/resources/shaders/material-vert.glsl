attribute vec4 vertex;
attribute vec3 normal;

varying vec3 worldPosition;
varying vec3 worldNormal;

void main()
{
  worldPosition   = vertex.xyz;
  worldNormal     = normal.xyz;
  gl_Position     = gl_ModelViewProjectionMatrix * vec4(worldPosition, 1.0);
}
