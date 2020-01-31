// Title: Simple Raymarching Example
// Author: Sam Loeschen https://twitter.com/samloeschen

// Description:
// When I started learning ray marching I basically just wanted pre-built "scene" that I could dive into and start tinkering with,
// so here is a simple example that does exactly that! No step-by-step tutorial, just simple explanations of what everything does.
// If you want to start messing around, the first thing I would do is modify the scene() function with more shapes. You can find 
// more primitive at http://iquilezles.org/www/articles/distfunctions/distfunctions.htm. 

// Things to try:
// 1. Check out ourBox() and ourSphere() to see how I animated the primitives and gave them material IDs.
// 2. See if you can add a shape and material of your own.
// 3. Play with the camera and directional light, and see if you can animate them.


#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 resolution;
uniform float time;

#define RAYMARCH_STEPS 50
#define EPSILON 0.005
#define BACKGROUND vec4(0);
#define MIN_DIST 0.01
#define MAX_DIST 100.0

#define BLUE_MATERIAL_ID 0.5
#define RED_MATERIAL_ID 1.0

// iq's axis to direction rotation. Aligns an axis (z) to point along a direction (d)
mat3 rotationAlign(vec3 d, vec3 z) {
    vec3  v = cross( z, d );
    float c = dot( z, d );
    float k = 1.0/(1.0+c);

    return mat3(v.x*v.x*k + c,     v.y*v.x*k - v.z,    v.z*v.x*k + v.y,
                v.x*v.y*k + v.z,   v.y*v.y*k + c,      v.z*v.y*k - v.x,
                v.x*v.z*k - v.y,   v.y*v.z*k + v.x,    v.z*v.z*k + c    );
}


// composes a view matrix for a direction and global up vector
// based on gluLookAt man page
mat4 viewMatrix (vec3 dir, vec3 up) { 
    vec3 f = normalize(dir);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}

// simple box sdf. for a library of other primitives, see http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
float box (vec3 pos, vec3 size) {
  vec3 d = abs(pos) - size;
  return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// simple sphere sdf. for a library of other primitives, see http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sphere(vec3 pos, float radius) {
  return length(pos) - radius;
}

// returns the distance to our rotating box and the its associated material ID packed into a vec2
vec2 ourBox (vec3 pos) {

    // get a rotation matrix for our box's rotation
    vec3 dir = normalize(vec3(cos(time * 0.5), 0.0, sin(time * 0.5)));
    vec3 axis = normalize(vec3(0.0, 1.0, 0.0)); // up
    mat3 rotation = rotationAlign(dir, axis);

    // rotate our world point with the box's rotation matrix
    pos *= rotation;
    return vec2(box(pos, vec3(1.0)), BLUE_MATERIAL_ID);
}

// returns the distance to our moving sphere and the its associated material ID packed into a vec2
vec2 ourSphere (vec3 pos) {

    // just translate the sphere up and down
    vec3 offset = vec3(sin(time * 0.5), sin(time) * 3.0, cos(time) * 3.0);
    pos += offset;

    // rotate our world point with the box's rotation matrix
    return vec2(sphere(pos, 1.0), RED_MATERIAL_ID);
}

// this is the combined scene sdf function! if you want to add shapes, put them in here.
// it returns a vec2 containing a depth value for the x coordinate, and a material ID for the y coordinate.
vec2 scene (vec3 pos) {
    // test our shapes
    vec2 boxTest        = ourBox(pos);
    vec2 sphereTest     = ourSphere(pos);
    vec2 result         = boxTest;

    // we want to return the nearest surface, so we sort by distance
    result = mix(result, sphereTest, step(sphereTest.x, result.x));

    // if you want to add more shapes, modify the result like so:
    // result = mix(result, yourNewShapeTest, step(yourNewShapeTest.x, result.x));
    return result;
}

// creates a view space ray going out from the camera
vec3 calcRay (float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size * 0.5;
    float z = size.y / tan(radians(fieldOfView) * 0.5);
    return normalize(vec3(xy, -z));
}

// gets an estimated normal of the scene sdf at a given point
// we use the x coord because that is the depth value that is returned from the scene
vec3 calcNormal (vec3 pos) {
    const vec2 eps = vec2(EPSILON, 0);
    return normalize(vec3(scene(pos + eps.xyy).x - scene(pos - eps.xyy).x,
                          scene(pos + eps.yxy).x - scene(pos - eps.yxy).x,
                          scene(pos + eps.yyx).x - scene(pos - eps.yyx).x));
}

// returns a color based on the material id from the scene sample
// right now it uses gross branching, but this could be eliminated with clever usage of step()
vec3 calcMaterial (float id) {
    if (id == RED_MATERIAL_ID)      return vec3(1, 0, 0);
    if (id == BLUE_MATERIAL_ID)     return vec3(0, 0, 1);
    return vec3(1);
}

// raymarches our scene with a provided camera position, camera direction, and simple directional light
vec4 marchScene (vec3 camPos, vec3 camDir, vec3 lightDir) {
    vec3 viewDir = calcRay(45.0, resolution.xy, gl_FragCoord.xy);
    mat4 viewMat = viewMatrix(
        camDir,
        vec3(0.0, 1.0, 0.0)
    );
    vec3 worldDir = (viewMat * vec4(viewDir, 1)).xyz;

    // do our raymarching!
    float depth = MIN_DIST;
    for (int i = 0; i < RAYMARCH_STEPS; i++) {
        vec3 rayPos = camPos + depth * worldDir;

        // sample the scene at this point in the ray.
        // the sample's x coord is our distance value, and the y coord is the material id
        vec2 sceneSample = scene(rayPos);
        float shortestDist = sceneSample.x;
        float materialId = sceneSample.y;

        if (shortestDist < EPSILON) {

            // we hit something, so get estimate our normal to do lighting and return a material color
            vec3 normal = calcNormal(rayPos);
            float light = dot(lightDir, normal) * 0.5 + 0.5; // half lambert
            vec3 surfaceColor = calcMaterial(materialId);
            vec4 outputColor = vec4(surfaceColor * light, 1.0);
            return outputColor;
        }

        // increase the depth of our ray by the shortest distance to an object in the scene.
        // since we know how far away everything is, we don't need to increment our ray in tiny steps!
        depth += shortestDist;

        // we reached our max distance and didn't hit anything, so just return the background
        if (depth > MAX_DIST - EPSILON) {
            return BACKGROUND;
        }
    }
    return BACKGROUND;
}

void main () {
    // let's place the camera 20 units away from the origin on the negative z axis,
    // and orient it to look at the center of the scene
    vec3 camPos = vec3(0, 0, 20); 
    vec3 camDir = normalize(-camPos);

    // we'll have our light coming from above and slightly behind us
    vec3 lightDir = normalize(vec3(0, 1, -0.5));

    gl_FragColor = marchScene(camPos, camDir, lightDir);
}