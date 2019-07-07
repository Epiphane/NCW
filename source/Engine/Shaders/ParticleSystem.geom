#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;

in float gType[];
in vec3 gPosition[];
in vec3 gVelocity[];
in float gAge[];

out float fType;
out vec3 fPosition;
out vec3 fVelocity;
out float fAge;

uniform float uDeltaTimeMillis;
uniform float uTick;
uniform sampler1D uRandomTexture;
uniform float uLauncherLifetime;
uniform float uShellLifetime;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

vec3 GetRandomDir(float TexCoord)
{
    vec3 Dir = texture(uRandomTexture, TexCoord).xyz;
    Dir -= vec3(0.5, 0.5, 0.5);
    return Dir;
}

#define dist 0.2

void main()
{
    float Age = Age0[0] + uDeltaTimeMillis;
    
    if (Type0[0] == PARTICLE_TYPE_LAUNCHER) {
        if (Age >= uLauncherLifetime /*- (uLauncherLifetime - uDeltaTimeMillis * uPlayerSpeed / 1000)*/) {
            fType = PARTICLE_TYPE_SHELL;
            fAge = 0.0;
            
            // Used to spawn particles around the emitters
            vec3 randomVec = GetRandomDir(uTick / 1000.0);
            randomVec.z = 0.0;
            randomVec.x = randomVec.x / 20;
            randomVec.y = randomVec.y / 15;
            fPosition = Position0[0] + randomVec;
            vec3 Dir = GetRandomDir(uTick/1000.0);
            Dir.y = max(Dir.y, 0.5);
            Dir.x = min(Dir.x, -0.3);
            Dir.z = min(Dir.z, -0.5);
            fVelocity = normalize(Dir) / 5;
            EmitVertex();
            
            randomVec = GetRandomDir(uTick / 1000.0);
            randomVec.z = 0.0;
            randomVec.x = randomVec.x / 20;
            randomVec.y = randomVec.y / 15;
            fPosition = Position0[0] + randomVec;
             Dir = GetRandomDir(uTick/1000.0);
            Dir.y = max(Dir.y, 0.5);
            Dir.x = min(Dir.x, -0.3);
            Dir.z = min(Dir.z, -0.5);
            
            EmitVertex();
            Age = 0.0;
        }
        
        fType = PARTICLE_TYPE_LAUNCHER;
        fPosition = Position0[0];
        fVelocity = Velocity0[0];
        fAge = Age;
        EmitVertex();
        EndPrimitive();
    }
    else {
        float DeltaTimeSecs = uDeltaTimeMillis / 1000.0f;
        float t1 = Age0[0] / 1000.0;
        float t2 = Age / 1000.0;
        vec3 DeltaP = DeltaTimeSecs * Velocity0[0];
        vec3 DeltaV = vec3(DeltaTimeSecs) * vec3(0.0, 3.0, 0.0);
        
        if (Type0[0] == PARTICLE_TYPE_SHELL)  {
            if (Age < uShellLifetime) {
                
                fType = PARTICLE_TYPE_SHELL;
                fPosition = Position0[0] + DeltaP;
                fVelocity = Velocity0[0] + DeltaV;
                fAge = Age;
                EmitVertex();
                EndPrimitive();
            }
            
        }
        
    }
}