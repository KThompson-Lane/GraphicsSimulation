#version 400

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;

in  vec3 in_Position;  // Position coming in
in  vec2 in_TexCoord;  // texture coordinate coming in
in  vec3 in_Normal;    // vertex normal used for lighting

out vec2 ex_TexCoord;  // exiting texture coord
out vec3 ex_Normal;    // exiting normal transformed by the normal matrix
out vec3 ex_PositionEye; //Exiting position in eyespace

uniform vec3 deformation;

void main(void)
{
	vec3 newVertexPos = in_Position;
	if(deformation != 0.0f)
	{
		newVertexPos =  (in_Position + deformation) + (in_Normal * deformation);
	}

	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(newVertexPos, 1.0);

	ex_TexCoord = in_TexCoord;
	
	ex_Normal = NormalMatrix*in_Normal; 

	ex_PositionEye = vec3((ModelViewMatrix * vec4(newVertexPos, 1.0)));
}