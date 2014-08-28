varying vec3 worldNormal;
varying vec3 worldPosition;
varying vec4 vertexColor;

uniform vec3 lightPosition1;
uniform vec3 lightPosition2;
uniform vec3 cameraPosition;

uniform float ambientFactor;
uniform float specularPower;
uniform float specularFactor;
uniform float diffuseFactor;
uniform vec3 rimColor;
uniform float rimStart;
uniform float innerTransparency;
uniform float opacity;

const float rimEnd = 1.0;

void main() {
  vec3 normal = normalize(worldNormal);
  vec3 eyedir = normalize(worldPosition - cameraPosition);

  // compute vectors from this point to each light
  vec3 lightdir1 = normalize(lightPosition1 - worldPosition);
  vec3 lightdir2 = normalize(lightPosition2 - worldPosition);

  // specular
  vec3 r1 = normalize(reflect(lightdir1, normal));
  float s1 = specularFactor * pow(clamp(dot(r1, eyedir), 0.0, 1.0), specularPower);
  vec3 r2 = normalize(reflect(lightdir2, normal));
  float s2 = specularFactor * pow(clamp(dot(r2, eyedir), 0.0, 1.0), specularPower);
  
  // diffuse brightness contribution from each light
  float d1 = diffuseFactor * max(0.0, dot(lightdir1, normal));
  float d2 = diffuseFactor * max(0.0, dot(lightdir2, normal));

  // fade out fragments near the camera
  float distMult = 1.0; // -clamp((worldPosition.z + 125)/50, 0.0, 1.0);

  // fade out fragments near the center of the geometry
  float edgeMult = 1.0 - innerTransparency*abs(dot(normal, eyedir));

  // rim lighting
  float normalToCam = 1.0 - dot(normal, -eyedir);
  float rim = smoothstep(rimStart, rimEnd, normalToCam);

  // combine specular, diffuse, and fade
  vec3 specularColor = vec3(s1 + s2);
  vec3 diffuseColor = (d1 + d2 + ambientFactor)*vertexColor.rgb;

  vec4 color = vec4(specularColor + diffuseColor + rim*rimColor, 1.0);

  gl_FragColor = opacity * vertexColor.a * distMult * edgeMult * color;
}
