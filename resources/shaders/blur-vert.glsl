varying vec2 texCoord;
varying vec2 blurTexCoords[14];
uniform vec2 sample_offset;
uniform vec2 global_offset;

void main()
{
  gl_Position = ftransform();
  gl_TexCoord[0] = gl_MultiTexCoord0;
  texCoord = gl_MultiTexCoord0.st + global_offset;
  blurTexCoords[0] = texCoord + -0.028*sample_offset;
  blurTexCoords[1] = texCoord + -0.024*sample_offset;
  blurTexCoords[2] = texCoord + -0.020*sample_offset;
  blurTexCoords[3] = texCoord + -0.016*sample_offset;
  blurTexCoords[4] = texCoord + -0.012*sample_offset;
  blurTexCoords[5] = texCoord + -0.008*sample_offset;
  blurTexCoords[6] = texCoord + -0.004*sample_offset;
  blurTexCoords[7] = texCoord + 0.004*sample_offset;
  blurTexCoords[8] = texCoord + 0.008*sample_offset;
  blurTexCoords[9] = texCoord + 0.012*sample_offset;
  blurTexCoords[10] = texCoord + 0.016*sample_offset;
  blurTexCoords[11] = texCoord + 0.020*sample_offset;
  blurTexCoords[12] = texCoord + 0.024*sample_offset;
  blurTexCoords[13] = texCoord + 0.028*sample_offset;
}
