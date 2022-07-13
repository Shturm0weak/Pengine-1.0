local target = nil;
local this = nil;
local targetTransform = nil;
local thisTransform = nil;
local thisRb2d = nil;

function OnUpdate(dt)
	thisRb2d:SetAllowToSleep(false);

	speed = 10;

	local position = thisTransform:GetPosition();

	local y = Input.GetJoyStickAxis(1)  --Left Stick Y
	position.y = position.y - speed * dt * y;
	thisTransform:Translate(position);

	local x = Input.GetJoyStickAxis(0)	 --Left Stick X
	position.x = position.x + speed * dt * x;
	thisTransform:Translate(position);

	position = targetTransform:GetPosition();

	y = Input.GetJoyStickAxis(3)  --Left Stick Y
	position.y = position.y - speed * dt * y;
	targetTransform:Translate(position);

	x = Input.GetJoyStickAxis(2)	 --Left Stick X
	position.x = position.x + speed * dt * x;
	targetTransform:Translate(position);

	local hit = Hit2D.Create();
	local startvec = thisTransform:GetPosition();
	local endvec = targetTransform:GetPosition();

	Raycast.RayCast2D(startvec, endvec, hit, { "Player" });

	local point = hit:GetPosition();
	if point.x ~= 0 and point.y ~= 0 then
		Visualizer.DrawLine(startvec, { x = point.x, y = point.y, z = 0 }, { x = 0, y = 0, z = 1, w = 1 });
	else
		Visualizer.DrawLine(startvec, endvec, { x = 1, y = 0, z = 0, w = 1 });
	end

	Visualizer.DrawCircle({ x = 0.3, y = 0.5 }, thisTransform:GetTransform(), { x = 0, y = 0, z = 1, w = 1 });
	Visualizer.DrawCircle({ x = 0.3, y = 0.5 }, targetTransform:GetTransform(), { x = 0, y = 0, z = 1, w = 1 });
end

function OnStart()
	target = GameObject.Create("Target");
	this = GameObject.This();
	thisTransform = Transform.Get(this);
	targetTransform = Transform.Get(target);
	Renderer2D.Add(target);
	thisRb2d = RigidBody2D.Get(this);
	thisRb2d:SetFixedRotation(true);
end