uniform sampler2D positionTexture;
uniform float forceDir;
uniform float dt;

attribute vec4 coords;

varying vec3 vPos;

#if 1
void main() {
  vec3 posOut = texture2D(positionTexture, coords.xy).xyz;
  vec3 posIn = texture2D(positionTexture, coords.zw).xyz;
  vec3 diff = posIn - posOut;
  // float dist = length(diff);
  // vec3 f = sign(diff) * min(abs(diff), 0.1) * dt;
  // vec3 f = (diff / dist) * dt;
  vec3 f = diff * dt;
  // f = sign(f) * log(abs(f) + 1.);
  // f = sign(f) * min(abs(f), 0.01);
  // vPos = vec3(0.);
  if (forceDir > 0.5) {
    vPos = f;
    gl_Position = vec4(coords.xy * 2. - 1., 0.0, 1.0);
  } else {
    vPos = -f;
    gl_Position = vec4(coords.zw * 2. - 1., 0.0, 1.0);
  }
  vPos = clamp(vPos, -5, 5);
}
#else

void main() {
  vec2 coords1 = coords.xy;
  vec2 coords2 = coords.zw;
  vec3 pos1 = texture2D(positionTexture, coords1).xyz;
  vec3 pos2 = texture2D(positionTexture, coords2).xyz;

  vec3 center = 0.5 * (pos1 + pos2);

  //vec3 diff = posIn - posOut;
  // float dist = length(diff);
  // vec3 f = sign(diff) * min(abs(diff), 0.1) * dt;
  // vec3 f = (diff / dist) * dt;
  //vec3 f = diff * dt;
  // f = sign(f) * log(abs(f) + 1.);
  // f = sign(f) * min(abs(f), 0.01);
  // vPos = vec3(0.);
  if (forceDir > 0.5) {
    //vPos = f;
    //vPos = dt * -;
    vec3 diff = (pos1 - center);
    float len = length(diff);
    vPos = dt * -(len - targetLength) * diff / len;

    gl_Position = vec4(coords1 * 2. - vec2(1.0), 0.0, 1.0);
  } else {
    //vPos = -f;
    //vPos = dt * -(pos2 - center);
    vec3 diff = (pos2 - center);
    float len = length(diff);
    vPos = dt * -(len - targetLength) * diff / len;

    gl_Position = vec4(coords2 * 2. - vec2(1.0), 0.0, 1.0);
  }
}

#endif
