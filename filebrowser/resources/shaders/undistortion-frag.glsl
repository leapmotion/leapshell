#version 120

// original color
uniform sampler2D colorTex;

// uniform for gamma correction
uniform float gamma;

// uniforms for distortion
uniform bool useDistortion;
uniform sampler2D distortionMap;

uniform vec2 rayScale;
uniform vec2 rayOffset;

vec3 getOrigValue() {
  if (useDistortion) {
    // when using distortion, first look up into the distortion map texture to get our actual pixel location
    vec2 lookup = texture2D(distortionMap, gl_TexCoord[0].st).rg;

    //lookup.y = 1.0 - lookup.y;
    if (lookup.x < 0.0 || lookup.x > 1.0 || lookup.y < 0.0 || lookup.y > 1.0) {
      return vec3(0.0);
    } else {
      return texture2D(colorTex, lookup).rgb;
    }
  } else {
    return texture2D(colorTex, gl_TexCoord[0].st).rgb;
  }
}

void main() {
  // retrieve original value
  vec3 color = getOrigValue();

  // perform gamma correction
  color = pow(color, vec3(1.0 / gamma));

  // perform clamping
  color = clamp(color, vec3(0.0), vec3(1.0));

  gl_FragColor.rgb = vec3(color.r, color.r, color.r);
  gl_FragColor.a = 1.0;
}
