function OnUpdate(dt)
	local speed = 10;
	local gameObject = GameObject.This();
	local transform = Transform.Get(gameObject);
	local position = transform:GetPosition();
	local forward = transform:GetRight();
	transform:Translate(vec3.Add(vec3.Mulvf(forward, speed * dt), position));
end

function OnStart()
end