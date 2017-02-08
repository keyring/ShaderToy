uniform lowp float iGlobalTime;
varying highp vec2 position;
void main() {
    float t = iGlobalTime;
    lowp float i = 1. - (pow(abs(position.x), 4.) + pow(abs(position.y), 4.));
    i = smoothstep(sin(t) - 0.8, sin(t) + 0.8, i);
    i = floor(i * 20.) / 20.;
    gl_FragColor = vec4(position * .5 + .5, i, i);
}