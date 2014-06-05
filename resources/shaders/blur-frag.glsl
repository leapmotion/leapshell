#version 120

uniform sampler2D color_tex;
uniform vec4 blur_color;
uniform bool override_alpha;

varying vec2 texCoord;
varying vec2 blurTexCoords[14];

void main()
{
  gl_FragColor = vec4(0.0);
  gl_FragColor += texture2D(color_tex, blurTexCoords[0])*0.0044299121055113265;
  gl_FragColor += texture2D(color_tex, blurTexCoords[1])*0.00895781211794;
  gl_FragColor += texture2D(color_tex, blurTexCoords[2])*0.0215963866053;
  gl_FragColor += texture2D(color_tex, blurTexCoords[3])*0.0443683338718;
  gl_FragColor += texture2D(color_tex, blurTexCoords[4])*0.0776744219933;
  gl_FragColor += texture2D(color_tex, blurTexCoords[5])*0.115876621105;
  gl_FragColor += texture2D(color_tex, blurTexCoords[6])*0.147308056121;
  gl_FragColor += texture2D(color_tex, texCoord)*0.159576912161;
  gl_FragColor += texture2D(color_tex, blurTexCoords[7])*0.147308056121;
  gl_FragColor += texture2D(color_tex, blurTexCoords[8])*0.115876621105;
  gl_FragColor += texture2D(color_tex, blurTexCoords[9])*0.0776744219933;
  gl_FragColor += texture2D(color_tex, blurTexCoords[10])*0.0443683338718;
  gl_FragColor += texture2D(color_tex, blurTexCoords[11])*0.0215963866053;
  gl_FragColor += texture2D(color_tex, blurTexCoords[12])*0.00895781211794;
  gl_FragColor += texture2D(color_tex, blurTexCoords[13])*0.0044299121055113265;

  gl_FragColor = blur_color * gl_FragColor;
  if (override_alpha) {
    gl_FragColor.a = 1.0;
  } else {
    gl_FragColor.a = clamp(gl_FragColor.a * 1.25, 0.0, 1.0);
  }
}
