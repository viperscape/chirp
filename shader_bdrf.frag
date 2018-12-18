
in vec3 normal;
in mat3 tbn;
in vec3 position;
in vec2 texcoord;

uniform sampler2D samp_tex;
uniform sampler2D samp_norm;
uniform sampler2D samp_metal;
uniform vec4 base_color_factor; // pbr input
uniform float metallic_factor;
uniform float roughness_factor;

uniform vec3 camera_position;

// Light struct is a prelude during shader compilation
uniform Light lights[MAX_LIGHTS]; // expects max lights to be a prelude


vec3 LinearSRGB(vec3 c)
{
	float power = 2.2;
	return pow(c, vec3(power, power, power));
}

vec3 SRGBLinear(vec3 c)
{
	float power = 1.0 / 2.2;
	return pow(c, vec3(power, power, power));
}



const float pi = 3.141592653589793;
const float min_roughness = 0.04;

out vec4 color;
void main()
{
	vec4 base_color = vec4((LinearSRGB(texture2D(samp_tex, texcoord).rgb) * base_color_factor.rgb),
		base_color_factor.a);
	
    vec4 metal_color = texture2D(samp_metal, texcoord);
    float perceptualRoughness = metal_color.g * roughness_factor;
    float metallic = metal_color.b * metallic_factor;
    perceptualRoughness = clamp(roughness_factor, min_roughness, 1.0);
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

	vec3 f0 = vec3(0.04);
    vec3 diffuse_color = base_color.rgb * (vec3(1.0) - f0);
    diffuse_color *= 1.0 - metallic;
    
	vec3 specular_color = mix(f0, base_color.rgb, metallic);
    float reflectance = max(max(specular_color.r, specular_color.g), specular_color.b);
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 spec_r90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	
	vec3 norm = normalize(texture(samp_norm, texcoord).rgb * 2.0 - 1.0);
	norm = normalize(tbn * norm);

	vec3 view_dir = normalize(camera_position - position);
	vec3 result = vec3(0,0,0);

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		vec3 diffuse = vec3(0,0,0);
		vec3 specular = vec3(0,0,0);
		vec3 light_dir = vec3(0,0,0);
		float light_dist;

		if (lights[i].attributes.y < 1) continue; // disabled

		if (lights[i].attributes.x > 1) // directional
		{
			light_dir = lights[i].direction;
		}
		else if (lights[i].attributes.x > 0) // spot
		{
			light_dir = lights[i].position - lights[i].direction;
		}
		else // point light
		{
			light_dir = lights[i].position - position;
		}

		
		light_dist = length(light_dir);
		light_dir = normalize(light_dir);  


		vec3 half = normalize(light_dir + view_dir);
		vec3 reflection = -normalize(reflect(view_dir, norm));
		float NdotL = clamp(dot(norm, light_dir), 0.001, 1.0);
		float NdotV = clamp(abs(dot(norm, view_dir)), 0.001, 1.0);
		float NdotH = clamp(dot(norm, half), 0.0, 1.0);
		float LdotH = clamp(dot(light_dir, half), 0.0, 1.0);
		float VdotH = clamp(dot(view_dir, half), 0.0, 1.0);

		
  		// diffuse color contribution
		vec3 F = specular_color + (reflectance90 - specular_color) * 
			pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
		diffuse = (1.0 - F) * diffuse_color / pi;
		

		// specular contribution
		float r = alphaRoughness;
		float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
		float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
		float G = attenuationL * attenuationV;
		

		float roughnessSq = alphaRoughness * alphaRoughness;
		float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
		float D = roughnessSq / (pi * f * f);

		specular = F * G * D / (4.0 * NdotL * NdotV);

		// spotlight soft
		if (lights[i].attributes.x == 1)
		{
			float theta = dot(light_dir, normalize(-lights[i].direction)); 
			float epsilon = (lights[i].spot.x - lights[i].spot.y); // cutoff - outercutoff
			float intensity = clamp((theta - lights[i].spot.y) / epsilon, 0.0, 1.0);
			diffuse  *= intensity;
			specular *= intensity;
		}
		

		// attenuate if not a directional
		if (lights[i].attributes.x < 2) 
		{
			float att = 1.0 / 
				(lights[i].attenuation.x + 
				lights[i].attenuation.y * light_dist + 
				lights[i].attenuation.z * light_dist * light_dist);
			diffuse *= att;
			specular *= att;
		}
        

		result += NdotL * lights[i].diffuse * (diffuse + specular);
	}

	result = SRGBLinear(result); // adjust gamma
    color = vec4(result, base_color.a);
}