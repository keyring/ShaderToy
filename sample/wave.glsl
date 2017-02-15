void main()
{
	vec2 uv = (vec2(gl_FragCoord.x, 1.-gl_FragCoord.y) / iResolution.xy);
	uv.y += .05 * sin(iGlobalTime*10. + uv.x * 10.);
	uv.x += .05 * sin(iGlobalTime*10. + uv.y * 10.);
	vec4 color = texture2D(iChannel0, uv);
	color *= color;
	gl_FragColor = color;
}