#version 120

// original color
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform bool useColor;

// uniform for gamma correction
uniform float gamma;

// uniforms for distortion
uniform bool useDistortion;
uniform sampler2D distortionMap;

uniform vec2 rayScale;
uniform vec2 rayOffset;

//float width = 640;
//float height = 360;

//vec2 blueOffset = vec2(0.25/width, 0.0);
//vec2 greenOffset = vec2(0.25/width, -0.25/height);
//vec2 redOffset = vec2(0.0, -0.25/height);
uniform vec2 blueOffset;
uniform vec2 greenOffset;
uniform vec2 redOffset;

vec3 getColorValue() {
  if (useDistortion) {
    // when using distortion, first look up into the distortion map texture to get our actual pixel location
    vec2 lookup = texture2D(distortionMap, gl_TexCoord[0].st).rg;

    //lookup.y = 1.0 - lookup.y;
    if (lookup.x < 0.0 || lookup.x > 1.0 || lookup.y < 0.0 || lookup.y > 1.0) {
      return vec3(0.0);
    } else {
      float r = texture2D(tex1, lookup + redOffset).r;
      float g = texture2D(tex2, lookup + greenOffset).r;
      float b = texture2D(tex3, lookup + blueOffset).r;
      return vec3(r, g, b);
    }
  } else {
    float r = texture2D(tex1, gl_TexCoord[0].st).r;
    float g = texture2D(tex2, gl_TexCoord[0].st).r;
    float b = texture2D(tex3, gl_TexCoord[0].st).r;
    return vec3(r, g, b);
  }
}

vec3 getGrayscaleValue() {
  if (useDistortion) {
    // when using distortion, first look up into the distortion map texture to get our actual pixel location
    vec2 lookup = texture2D(distortionMap, gl_TexCoord[0].st).rg;

    //lookup.y = 1.0 - lookup.y;
    if (lookup.x < 0.0 || lookup.x > 1.0 || lookup.y < 0.0 || lookup.y > 1.0) {
      return vec3(0.0);
    } else {
      float r = texture2D(tex0, lookup).r;
      return vec3(r, r, r);
    }
  } else {
    float r = texture2D(tex0, gl_TexCoord[0].st).r;
    return vec3(r, r, r);
  }
}

void main() {
  // retrieve original value
  vec3 color;
  if (useColor) {
    color = getColorValue();
  } else {
    color = getGrayscaleValue();
  }

  // perform gamma correction
  color = pow(color, vec3(1.0 / gamma));

  // perform clamping
  color = clamp(color, vec3(0.0), vec3(1.0));

  gl_FragColor.rgb = color;
  gl_FragColor.a = 1.0;
}
