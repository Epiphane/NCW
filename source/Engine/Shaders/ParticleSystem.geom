#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;

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
uniform float uShellLifetime;

#define EMITTER 1.0f
#define PARTICLE 2.0f

vec3 GetRandomDir(float TexCoord)
{
    vec3 Dir = texture(uRandomTexture, mod(TexCoord, 1000) / 1000).xyz;
    Dir -= vec3(0.5, 0.5, 0.5);
	return Dir;
}

#define dist 0.2

void main()
{
    float Age = gAge[0] + uDeltaTimeMillis;
    
    if (gType[0] == EMITTER) {
		// Send emitter first so it's always the front
		// and can't get cut off by max vertices restrictions
		float nEmit = floor(Age / uEmitterCooldown);

        fType = EMITTER;
        fPosition = gPosition[0];
		fRotation = gRotation[0];
        fVelocity = gVelocity[0];
        fAge = mod(Age, uEmitterCooldown);
        EmitVertex();
        EndPrimitive();

		for (int emit = 0; emit < nEmit; ++emit) {
            fType = PARTICLE;
            
            vec3 offsetRandom = GetRandomDir(uTick + emit * 200);
            fAge = (offsetRandom.y + 0.5) / 4;

			// Temporary: Spawn in a horizontal disk around the emitter,
			// with a slight weight on the border
			offsetRandom.y = 0;
			if (offsetRandom.x * offsetRandom.x + offsetRandom.z * offsetRandom.z > 1) {
				offsetRandom = normalize(offsetRandom);
			}

			fPosition = gPosition[0] + 0.5 * offsetRandom;

			// Place in the world
			fPosition = (uModelMatrix * vec4(fPosition, 1)).xyz;

			// Temporary: Spawn with velocity moving up and towards the center
            vec3 velocityRandom = GetRandomDir(uTick + emit * 200 + 49);
			velocityRandom.x = 0;
			velocityRandom.z = 0;
			velocityRandom.y = 7 + 8 * velocityRandom.y;
			fVelocity = 200 * velocityRandom - 600 * offsetRandom;

			// Give it a random rotation
			fRotation = vec4(GetRandomDir(uTick + emit * 200 + 72), 0);

            EmitVertex();
        }
    }
    else {
        float DeltaTimeSecs = uDeltaTimeMillis / 1000.0f;
        float t1 = gAge[0] / 1000.0;
        float t2 = Age / 1000.0;
        vec3 DeltaP = DeltaTimeSecs * gVelocity[0];
        vec3 Dir = GetRandomDir(uTick/1000.0);
        // Dir.y = max(Dir.y, 0.5);
        // Dir.x = min(Dir.x, -0.3);
        // Dir.z = min(Dir.z, -0.5);
        vec3 DeltaV = vec3(DeltaTimeSecs) * vec3(0.0, 3.0, 0.0) + vec3(0, 1, 0);
        
        if (gType[0] == PARTICLE)  {
            if (Age < uShellLifetime) {
                
                fType = PARTICLE;
                fPosition = gPosition[0] + DeltaP;
				fRotation = gRotation[0] + vec4(0, 0, 0, 1);
                fVelocity = gVelocity[0];// + DeltaV;
                fAge = Age;
                EmitVertex();
                EndPrimitive();
            }
            
        }
        
    }
}