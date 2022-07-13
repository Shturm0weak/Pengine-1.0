local r2d = nil;
local this = nil;
local index = 1;

function SetColor()
	if index == 0 then
		r2d:SetColor({ x = 1, y = 1, z = 1, w = 1 });
	elseif index == 1 then
		r2d:SetColor({ x = 1, y = 0, z = 0, w = 1 });
	elseif index == 2 then
		r2d:SetColor({ x = 0, y = 1, z = 0, w = 1 });
	elseif index == 3 then
		r2d:SetColor({ x = 0, y = 0, z = 1, w = 1 });
	elseif index == 4 then
		r2d:SetColor({ x = 1, y = 1, z = 0, w = 1 });
	elseif index == 5 then
		r2d:SetColor({ x = 0, y = 1, z = 1, w = 1 });
	elseif index == 6 then
		r2d:SetColor({ x = 1, y = 0, z = 1, w = 1 });
	end
end

function OnUpdate(dt)
	if Input.IsJoyStickPressed(0) == true then
		SetColor();
		print(index);
		index = index + 1;
		if index == 7 then
			index = 0;
		end
	end
end

function OnStart()
	this = GameObject.This();
	r2d = Renderer2D.Get(this);
end