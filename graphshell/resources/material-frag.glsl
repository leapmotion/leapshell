uniform vec3 surfaceColor;
uniform vec3 lightPosition;
uniform float ambient;

varying vec3 worldPosition; 
varying vec3 worldNormal;

void main()
{
  vec3 normal = normalize(worldNormal);
  vec3 lightdir = normalize(lightPosition - worldPosition);
  float d = ambient + clamp(dot(lightdir, normal), 0.0, 1.0);
  vec3 color = d * surfaceColor;
  gl_FragColor = vec4(color, 1.0);
}
