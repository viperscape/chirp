
in vec3 normal;
in mat3 tbn;
in vec3 position;
in vec2 texcoord;

uniform sampler2D samp_tex;
uniform sampler2D samp_norm;
uniform sampler2D samp_metal;

uniform vec3 camera_position;

// Light struct is a prelude during shader compilation
uniform Light lights[MAX_LIGHTS]; // expects max lights to be a prelude


out vec4 color;
void main()
{
	vec3 diffuse_color = texture2D(samp_tex, texcoord).rgb;
	float mat_shininess = 16;
	
	vec3 normal_map = normalize(texture(samp_norm, texcoord).rgb * 2.0 - 1.0);
	//normal_map.y *= -1.0; //invert green
	normal_map = normalize(tbn * normal_map);
	vec3 norm = normalize(normal);


	vec3 result = vec3(0,0,0);

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		// per light, should be overridden each iteration
		vec3 ambient = vec3(0,0,0);
		vec3 diffuse = vec3(0,0,0);
		vec3 specular = vec3(0,0,0);
		vec3 bump = vec3(0,0,0);
		vec3 light_dir = vec3(0,0,0);
		float nl, light_dist; // temp vars for each light

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

		// add additional ambience
		ambient = lights[i].ambient * diffuse_color;
		
  	
		// diffuse 
		nl = max(dot(norm, light_dir), 0.0);
		diffuse = lights[i].diffuse * nl * diffuse_color;
		
    
		// specular
		if (nl > 0.0)
		{
			vec3 view_dir = normalize(camera_position - position);
			vec3 reflect_dir = reflect(-light_dir, norm);  
			nl = pow(max(dot(view_dir, reflect_dir), 0.0), 32); // shininess

			specular = lights[i].specular * nl;// * spec_color;
		}

		
		// spotlight soft
		if (lights[i].attributes.x == 1)
		{
			float theta = dot(light_dir, normalize(-lights[i].direction)); 
			float epsilon = (lights[i].spot.x - lights[i].spot.y); // cutoff - outercutoff
			float intensity = clamp((theta - lights[i].spot.y) / epsilon, 0.0, 1.0);
			diffuse  *= intensity;
			specular *= intensity;
		}
		

		// bump map
		nl = max(dot(light_dir, normal_map),0.0);
		bump = clamp(0.5 * nl * lights[i].diffuse, 0,1);
		bump *= diffuse_color;

		// attenuate if not a directional
		if (lights[i].attributes.x < 2) 
		{
			float att = 1.0 / 
				(lights[i].attenuation.x + 
				lights[i].attenuation.y * light_dist + 
				lights[i].attenuation.z * light_dist * light_dist);
			ambient *= att;
			diffuse *= att;
			specular *= att;
			bump *= att;
		}
        

		// apply color to frag
		result += ambient + diffuse + specular + bump;
	}

	//vec3 result_gamma = pow(result, vec3(1.0/2.2)); // adjust for 2.2 gamma
    color = vec4(result, 1.0);
}