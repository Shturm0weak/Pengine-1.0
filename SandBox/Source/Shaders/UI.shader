#shader vertex
#version 330 core

layout(location = 0) in vec4 positionA;
layout(location = 1) in vec2 uvA;
layout(location = 2) in int isStaticA;
layout(location = 3) in vec4 colorA;
layout(location = 4) in float isGuiA;
layout(location = 5) in float texIndexA;
layout(location = 6) in int isRelatedToPanelA;
layout(location = 7) in vec2 relatedPanelPosA;
layout(location = 8) in vec2 relatedPanelSizeA;
layout(location = 9) in vec4 roundedCornersA;
layout(location = 10) in vec4 previousRoundedCornersA;
layout(location = 11) in vec2 positionOfRectA;
layout(location = 12) in vec2 sizeOfRectA;

flat out int texIndex;
flat out int isRelatedToPanel;
flat out vec2 relatedPanelPos;
flat out vec2 relatedPanelSize;
flat out vec4 roundedCorners;
flat out vec4 previousRoundedCorners;
flat out vec2 positionOfRect;
flat out vec2 sizeOfRect;
out float isGui;
out vec4 color;
out vec2 uv;

uniform mat4 u_Projection;

void main()
{
	if (isStaticA == 0)
	{
		gl_Position = u_Projection * positionA;
	}
	else if (isStaticA > 1)
	{
		gl_Position = u_Projection * positionA;
	}

	uv = uvA;
	color = colorA;
	isGui = isGuiA;
	texIndex = int(texIndexA);
	isRelatedToPanel = isRelatedToPanelA;
	relatedPanelSize = relatedPanelSizeA;
	relatedPanelPos = relatedPanelPosA;
	roundedCorners = roundedCornersA;
	previousRoundedCorners = previousRoundedCornersA;
	positionOfRect = positionOfRectA;
	sizeOfRect = sizeOfRectA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

flat in int texIndex;
flat in int isRelatedToPanel;
flat in vec2 relatedPanelPos;
flat in vec2 relatedPanelSize;
flat in vec4 roundedCorners;
flat in vec4 previousRoundedCorners;
flat in vec2 positionOfRect;
flat in vec2 sizeOfRect;
in float isGui;
in vec4 color;
in vec2 uv;

uniform vec4 u_outlineColor;
uniform float u_width;
uniform float u_edge;
uniform float u_borderwidth;
uniform float u_borderedge;
uniform float u_RoundedRadius;
uniform vec2 u_offset;
uniform sampler2D u_Texture[32];
//uniform vec2 u_ViewPortSize;

bool IsInsideOfRect(vec2 pos, vec2 size)
{
	if (gl_FragCoord.x > pos.x - size.x * 0.5
		&& gl_FragCoord.x < pos.x + size.x * 0.5
		&& gl_FragCoord.y > pos.y - size.y * 0.5
		&& gl_FragCoord.y < pos.y + size.y * 0.5)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsOutsideOfRadiusButInsideOfRect(vec2 pos, vec2 size, vec4 corners)
{
	if (u_RoundedRadius > 0.001 == false) return false;
	vec2 sizeOfCornerRect = vec2(u_RoundedRadius);
	if (corners.x > 1.0)
	{
		vec2 posTopLeft = vec2(
			pos.x - size.x * 0.5 + sizeOfCornerRect.x * 0.5,
			pos.y + size.y * 0.5 - sizeOfCornerRect.y * 0.5
		);
		if (IsInsideOfRect(posTopLeft, sizeOfCornerRect))
		{
			vec2 translatedPos = vec2(posTopLeft.x + sizeOfCornerRect.x * 0.5, posTopLeft.y - sizeOfCornerRect.y * 0.5);
			vec2 difference = gl_FragCoord.xy - translatedPos;
			float distance = sqrt(difference.x * difference.x + difference.y * difference.y);
			if (distance > u_RoundedRadius)
			{
				return true;
			}
		}
	}
	if (corners.y > 1.0)
	{
		vec2 posBottomLeft = vec2(
			pos.x - size.x * 0.5 + sizeOfCornerRect.x * 0.5,
			pos.y - size.y * 0.5 + sizeOfCornerRect.y * 0.5
		);
		if (IsInsideOfRect(posBottomLeft, sizeOfCornerRect))
		{
			vec2 translatedPos = vec2(posBottomLeft.x + sizeOfCornerRect.x * 0.5, posBottomLeft.y + sizeOfCornerRect.y * 0.5);
			vec2 difference = gl_FragCoord.xy - translatedPos;
			float distance = sqrt(difference.x * difference.x + difference.y * difference.y);
			if (distance > u_RoundedRadius)
			{
				return true;
			}
		}
	}
	if (corners.z > 1.0)
	{
		vec2 posBottomRight = vec2(
			pos.x + size.x * 0.5 - sizeOfCornerRect.x * 0.5,
			pos.y - size.y * 0.5 + sizeOfCornerRect.y * 0.5
		);
		if (IsInsideOfRect(posBottomRight, sizeOfCornerRect))
		{
			vec2 translatedPos = vec2(posBottomRight.x - sizeOfCornerRect.x * 0.5, posBottomRight.y + sizeOfCornerRect.y * 0.5);
			vec2 difference = gl_FragCoord.xy - translatedPos;
			float distance = sqrt(difference.x * difference.x + difference.y * difference.y);
			if (distance > u_RoundedRadius)
			{
				return true;
			}
		}
	}
	if (corners.w > 1.0)
	{
		vec2 posBottomRight = vec2(
			pos.x + size.x * 0.5 - sizeOfCornerRect.x * 0.5,
			pos.y + size.y * 0.5 - sizeOfCornerRect.y * 0.5
		);
		if (IsInsideOfRect(posBottomRight, sizeOfCornerRect))
		{
			vec2 translatedPos = vec2(posBottomRight.x - sizeOfCornerRect.x * 0.5, posBottomRight.y - sizeOfCornerRect.y * 0.5);
			vec2 difference = gl_FragCoord.xy - translatedPos;
			float distance = sqrt(difference.x * difference.x + difference.y * difference.y);
			if (distance > u_RoundedRadius)
			{
				return true;
			}
		}
	}
	return false;
}

void main()
{
	if (isRelatedToPanel > 1)
	{
		if (IsInsideOfRect(relatedPanelPos, relatedPanelSize))
		{
			if (IsOutsideOfRadiusButInsideOfRect(relatedPanelPos, relatedPanelSize, previousRoundedCorners))
			{
				//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
				//return;
				discard;
			}
		}
		else
		{
			//gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
			//return;
			discard;
		}
	}
	if (isGui > 0.5)
	{
		if (IsOutsideOfRadiusButInsideOfRect(positionOfRect, sizeOfRect, roundedCorners))
		{
			//gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
			//return;
			discard;
		}
		else
		{
			vec4 texColor = texture(u_Texture[texIndex], uv);
			gl_FragColor = color * texColor;
		}
	}
	else if (isGui <= 0.5)
	{
		float distance = 1.0 - texture(u_Texture[texIndex], uv).a;
		float alpha = 1.0 - smoothstep(u_width, u_width + u_edge, distance);
		float distance2 = 1.0 - texture(u_Texture[texIndex], uv + u_offset).a;
		float outlinealpha = 1.0 - smoothstep(u_borderwidth, u_borderwidth + u_borderedge, distance2);

		float overallalpha = alpha + (1.0 - alpha) * outlinealpha;
		vec3 overallcolor = mix(vec3(u_outlineColor.r, u_outlineColor.g, u_outlineColor.b), vec3(color.r, color.g, color.b), alpha / overallalpha);

		gl_FragColor = vec4(overallcolor, overallalpha);
		//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
}