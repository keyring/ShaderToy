void main()
{
	vec2 uv = (vec2(gl_FragCoord.x, gl_FragCoord.y) / iResolution.xy);
	float dis = sqrt(uv.x*uv.x + uv.y*uv.y);
	float sinFactor = .005* sin(dis* 100. + iGlobalTime*10.);
	vec2 uv1 = normalize(uv);
	vec2 offset = uv1 * sinFactor;
	vec2 dv = offset + vec2(gl_FragCoord.x, gl_FragCoord.y)/iResolution.xy;
	vec4 color = texture2D(iChannel0, dv);
	gl_FragColor = color;
}