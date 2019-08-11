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

vec4 GetRotationFromMatrix(mat4 matrix)
{
    // Logic taken in part from glm/ext/matrix_decompose.inl

    mat3 local = mat3(
        matrix[0].xyz,
        matrix[1].xyz,
        matrix[2].xyz
    );

    // Prune out scale and shear
    local[0] = normalize(matrix[0].xyz);

    float zSkew = dot(local[0], matrix[1].xyz);
    local[1] = matrix[1].xyz - local[0] * zSkew;
    local[1] = normalize(local[1]);

    float ySkew = dot(local[0], matrix[2].xyz);
    local[2] = matrix[2].xyz - local[0] * ySkew;
    float xSkew = dot(local[1], matrix[2].xyz);
    local[2] -= local[1] * xSkew;
    local[2] = normalize(local[2]);

    vec4 result = vec4(0, 0, 0, 0);
    float trace = local[0].x + local[1].y + local[2].z;
    if (trace > 0)
    {
        float root = sqrt(trace + 1);

        result.w = 0.5 * root;
        root = 0.5 / root;

        result.x = root * (local[1].z - local[2].y);
        result.y = root * (local[2].x - local[0].z);
        result.z = root * (local[0].y - local[1].x);
    }
    else
    {
        int i = 0;
        int j = 1;
        int k = 2;
        if (local[1].y > local[0].x)
        {
            i = 1;
            j = 2;
            k = 0;
        }
        if (local[2].z > local[i][i])
        {
            i = 2;
            j = 0;
            k = 1;
        }

        float root = sqrt(local[i][i] - local[j][j] - local[k][k] + 1);

        result[i] = 0.5 * root;
        root = 0.5 / root;

        result[j] = root * (local[i][j] + local[j][i]);
        result[k] = root * (local[i][k] + local[k][i]);
        result.w = root * (local[j][k] - local[k][j]);
    }

    
	// 	return vec<3, T, Q>(pitch(x), yaw(x), roll(x));
	// }

	// GLM_FUNC_QUALIFIER T roll(tquat<T, Q> const& q)
	// 	return static_cast<T>(atan(static_cast<T>(2) * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z));
	// }

	// GLM_FUNC_QUALIFIER T pitch(tquat<T, Q> const& q)
	// 	//return T(atan(T(2) * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z));
	// 	const T y = static_cast<T>(2) * (q.y * q.z + q.w * q.x);
	// 	const T x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

	// 	if(detail::compute_equal<T>::call(y, static_cast<T>(0)) && detail::compute_equal<T>::call(x, static_cast<T>(0))) //avoid atan2(0,0) - handle singularity - Matiis
	// 		return static_cast<T>(static_cast<T>(2) * atan(q.x,q.w));

	// 	return static_cast<T>(atan(y,x));
	// }

	// GLM_FUNC_QUALIFIER T yaw(tquat<T, Q> const& q)
	// 	return asin(clamp(static_cast<T>(-2) * (q.x * q.z - q.w * q.y), static_cast<T>(-1), static_cast<T>(1)));

    float tmp1 = 1.0 - result.w * result.w;
    if (tmp1 <= 0)
    {
        return vec4(0, 0, 1, acos(result.w) * 2.0);
    }

    float tmp2 = 1.0 / sqrt(tmp1);
    return vec4(result.x * tmp2, result.y * tmp2, result.x * tmp2, acos(result.w) * 2.0);
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

        for (int emit = 0; emit < nEmit && emit < MAX_VERTICES; ++emit) {
            fType = TYPE_PARTICLE;
 
            // NOTE rand1 can be reused, but the x value may
            // show a _stunning_ correlation to the age with
            // which each particle is spawned.
            vec3 rand1 = GetRandomVec3(uTick + emit * 200);
            fAge = spawnAgeMin + spawnAgeRange * (rand1.x + 0.5);
            
            fPosition = vec3(gPosition[0]);
            fVelocity = vec3(0, 0, 0);
            fRotation = GetRotationFromMatrix(uModelMatrix);//vec4(0, 0, 1, 0);
 
            if (uShape == SHAPE_POINT) {
                // Spawn particles at the emitter point, unmoving.
                // fVelocity.z = 1;
                //fPosition.y += 20;
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