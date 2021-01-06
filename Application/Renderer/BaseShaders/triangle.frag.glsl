#version 450
#extension GL_EXT_nonuniform_qualifier : require




vec3 camPos = { 0,0,0};



struct material
{
	int albedoTexture;
	vec4 albdoFactor;

	int metalic_RoughnessTexture;
	float metalicFactor;
	float roughnessFactor;

	int normalTexture;
	float normalScale;

	int occlusionTexture;
	float occlusionStrength;

	int emissiveTexture;
	vec3 emissiveFactor;
};

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
  int normalBufferIndex;
  int uvBufferIndex;

  int materialIndex;

  mat4 model;
} RenderData;

layout(set = 2, binding = 0) uniform sampler2D texSampler[];
layout(set = 2, binding = 1) buffer materials
{
    material m[];
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 outputColor;

const float PI = 3.14159265359;
vec3 lightDir = vec3(1, 0, 1);
float ambientLight = 0.2;

vec3 materialcolor()
{
	vec3 color;

  if(m[RenderData.materialIndex].albedoTexture == -1)
  {
    color = m[RenderData.materialIndex].albdoFactor.xyz;
  }
  else
  {
    color = texture(texSampler[m[RenderData.materialIndex].albedoTexture],uv).xyz;
  }

  return color;
}

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom);
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04), materialcolor(), metallic); // * material.specular
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	return F;
}

// Specular BRDF composition --------------------------------------------
vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
{
	// Precalculate vectors and dot products
	vec3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, metallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}




void main()
{
  vec3 N = normalize(nor);
  vec3 V = normalize(camPos - pos);

  float roughness = m[RenderData.materialIndex].roughnessFactor;
  float metallic = m[RenderData.materialIndex].metalicFactor;

  // Add striped pattern to roughness based on vertex position
  //#ifdef ROUGHNESS_PATTERN
  //roughness = max(roughness, step(fract(pos.y * 2.02), 0.5));
  //#endif

  // Specular contribution
  vec3 Lo = vec3(0.0);
  for (int i = 0; i < 1; i++)
  {
    vec3 L = normalize(lightDir - pos);
    Lo += BRDF(L, V, N, metallic, roughness);
  };

  // Combine with ambient
  vec3 color = materialcolor() * ambientLight;
  color += Lo;

  // Gamma correct
  color = pow(color, vec3(0.4545));

  outputColor = vec4(color, 1.0);



//    vec3 normalizedLight = normalize(lightDir);
//    vec3 rotatedNormal = vec3(RenderData.model * vec4(normal, 0));
//    float lightIntensity = dot(normalizedLight, normalize(rotatedNormal)) + ambientLight;

//    color *= lightIntensity;

//    outputColor = color;
}
