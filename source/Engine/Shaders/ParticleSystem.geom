#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;
#define MAX_VERTICES 40

in float gType[];
in vec3 gPosition[];
in vec4 gRotation[];
in vec3 gVelocity[];
in float gAge[];

out float fType;
out vec3 fPosition;
out vec4 fRotation;
out vec3 fVelocity;
out float fAge;

uniform mat4 uModelMatrix;
uniform float uDeltaTimeMillis;
uniform float uTick;
uniform sampler1D uRandomTexture;
uniform float uEmitterCooldown;
uniform float uParticleLifetime;

uniform uint uShape;
uniform vec2 uSpawnAge;
uniform vec3 uShapeParam0;
uniform float uShapeParam1;
uniform float uShapeParam2;
uniform float uShapeParam3;

#define TYPE_EMITTER 1.0f
#define TYPE_PARTICLE 2.0f

#define SHAPE_POINT 0u
#define SHAPE_CONE 1u

vec3 GetRandomPositiveVec3(float TexCoord)
{
    return texture(uRandomTexture, mod(TexCoord, 1000) / 1000).xyz;
}
 
vec3 GetRandomVec3(float TexCoord)
{
    return 2.0 * GetRandomPositiveVec3(TexCoord) - vec3(1.0, 1.0, 1.0);
}

void main()
{
    // float DeltaTimeSecs = uDeltaTimeMillis;// / 1000.0f;
    float Age = gAge[0] + uDeltaTimeMillis;
    
    if (gType[0] == TYPE_EMITTER) {
		// Send emitter first so it's always the front
		// and can't get cut off by max vertices restrictions
		float nEmit = floor(Age / uEmitterCooldown);

        fType = TYPE_EMITTER;
        fPosition = gPosition[0];
		fRotation = gRotation[0];
        fVelocity = gVelocity[0];
        fAge = mod(Age, uEmitterCooldown);
        EmitVertex();
        EndPrimitive();

        float spawnAgeRange = uSpawnAge[1] - uSpawnAge[0];
        float spawnAgeMin = uSpawnAge[0];
 
        int emit = 0;
        for (int emit = 0; emit < nEmit && emit < MAX_VERTICES; ++emit) {
            fType = TYPE_PARTICLE;
 
            // NOTE rand1 can be reused, but the x value may
            // show a _stunning_ correlation to the age with
            // which each particle is spawned.
            vec3 rand1 = GetRandomVec3(uTick + emit * 200);
            fAge = spawnAgeMin + spawnAgeRange * (rand1.x + 0.5);
            
            fPosition = vec3(gPosition[0]);
            fVelocity = vec3(0, 0, 0);
            fRotation = vec4(0, 0, 1, 0);
 
            if (uShape == SHAPE_POINT) {
                // Spawn particles at the emitter point, unmoving.
                // fVelocity.z = 1;
            }
            else if (uShape == SHAPE_CONE) {
                vec3 direction = uShapeParam0;
                float radius = uShapeParam1;
                float minHeight = uShapeParam2;
                float maxHeight = uShapeParam3;

                // Spawn in a horizontal disk around the emitter,
                // with a slight weight on the border.

                vec3 offset = vec3(rand1.y, 0, rand1.z);
                if (offset.x * offset.x + offset.z * offset.z > 1) {
                    offset = normalize(offset);
                }
                offset *= radius;

                // Spawn with velocity moving up and towards the center
                vec3 rand2 = GetRandomPositiveVec3(uTick + emit * 200 + 49);
                float climb = minHeight + (maxHeight - minHeight) * rand2.x;
                float converge = 0.3 + 0.7 * rand2.y;
                fVelocity = (vec3(0, climb, 0) - converge * offset) / uParticleLifetime;

                // TODO factor in uShapeParam0 (direction) when
                // computing the offset and velocity, so that
                // it's perpendicular to the provided direction.
                fPosition += offset;

                // Give it a random rotation
                vec3 rand3 = GetRandomVec3(uTick + emit * 200 + 72);
                fRotation = vec4(rand3, 0);
            }
 
            // Place in the world
            fPosition = (uModelMatrix * vec4(fPosition, 1)).xyz;

            EmitVertex();
            EndPrimitive();
        }
    }
    else if (gType[0] == TYPE_PARTICLE) {
        float DeltaTimeSecs = uDeltaTimeMillis / 1000.0f;
        vec3 DeltaP = uDeltaTimeMillis * gVelocity[0];
        
        if (Age < uParticleLifetime) {
            fType = TYPE_PARTICLE;
            fPosition = gPosition[0] + DeltaP;
            fRotation = gRotation[0];// + vec4(0, 0, 0, 0.1);
            fVelocity = gVelocity[0];// + DeltaV;
            fAge = Age;
            EmitVertex();
            EndPrimitive();
        }
    }
}