local _This = nil;
local _Light = nil;
local _Time = 0.0;

function OnUpdate(dt)
	_Time = _Time + dt;
	speed = math.random(0, 1) * 0.125;
	linear = 0.5 * math.sin(_Time * 2.0 * speed) + 1.5;
	_Fire:SetQuadratic(linear);
	_Fire:SetLinear(linear);
end

function OnStart()
	math.randomseed(os.time());
	_This = GameObject.This();
	_Fire = PointLight2D.Get(_This);
end