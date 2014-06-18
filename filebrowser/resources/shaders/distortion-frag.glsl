uniform sampler2D Texture0;

varying vec4 oColor;
varying vec2 oTexCoord0;
varying vec2 oTexCoord1;
varying vec2 oTexCoord2;

void main()
{
   float ResultR = texture2D(Texture0, oTexCoord0).r;
   float ResultG = texture2D(Texture0, oTexCoord1).g;
   float ResultB = texture2D(Texture0, oTexCoord2).b;

   float ResultA = texture2D(Texture0, 0.3333*(oTexCoord0 + oTexCoord1 + oTexCoord2)).a;

   gl_FragColor = vec4(ResultR * oColor.r, ResultG * oColor.g, ResultB * oColor.b, ResultA);
   //gl_FragColor = 0.1*gl_FragColor + vec4(0.9);
   //gl_FragColor = clamp(gl_FragColor, vec4(0.9), vec4(0.9));
};
