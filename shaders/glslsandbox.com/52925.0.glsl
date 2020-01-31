#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;



void main( void ) {

	vec2 pos=(gl_FragCoord.xy/resolution.xy)*2.0-1.0;
	pos.x*=resolution.x/resolution.y;
	
	
	
	float col=1.75;
	
	
//code
//float len = length(p);
  //vec2 uv = tc + (p/len)*cos(len*12.0-time*4.0)*0.03;
// from https://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl
//code
	
	
	
	col=mod(pos.x,0.2)+mod(pos.y,0.2);
	
	float len=length(pos);
	vec2 uv=vec2(col)+(pos/len)*cos(len*2.0-time*4.0);
	
	
	
		
	
	vec3 color=vec3(col*abs(sin(time))/8.0,uv);
	
	
	gl_FragColor=vec4(color / (color + 1.0),1.0);
}