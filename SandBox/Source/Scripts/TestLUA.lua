local target = nil;
local this = nil;
function OnStart()
	target = GameObject.Create("Target");
	this = GameObject.This();
	Renderer2D.Add(target);
end

function OnUpdate(dt)
	--if Input.IsKeyPressed(32) == true then
	--	gameObject = GameObject.This();
	--	gameObject:SetEnabled(not gameObject:IsEnabled());
	--	print(gameObject:IsEnabled());
	--end
	
	--print(gameObject);
	--
	--if Input.IsKeyPressed(32) == true then
	--	gameObject = GameObject.Create("TestLuaCreateGameObject");
	--end
	--
	--if Input.IsKeyPressed(68) == true then
	--	if gameObject ~= nil then
	--		gameObject:Delete();
	--	end
	--end

	--if Input.IsKeyPressed(32) == true then
	--	local v1 = { x = 100, y = 200, z = 300 };
	--	local v2 = { x = 11, y = 22, z = 33 };
	--	local v3 = vec3.Mulvf(v1, 5);
	--	print(v3.x .. " " .. v3.y .. " " .. v3.z);
	--end

	--if Input.IsKeyPressed(32) == true then
	--	local transform = Transform.Get(GameObject.This());
	--	local gameObject = GameObject.GetComponentOwner(transform);
	--	print(gameObject:GetName() .. " has component " .. GetType(transform));
	--	local owner = GameObject.GetOwner(gameObject);
	--	print(owner:GetName());
	--	owner:RemoveChild(gameObject);
	--end

	--speed = 5;
	--local transform = Transform.Get(GameObject.This());
	--local position = transform:GetPosition();
	--if Input.IsKeyDown(87) == true then	 --W
	--	position.y = position.y + speed * dt;
	--	transform:Translate(position);
	--end
	--if Input.IsKeyDown(83) == true then  --S
	--	position.y = position.y - speed * dt;
	--	transform:Translate(position);
	--end
	--if Input.IsKeyDown(65) == true then	 --A
	--	position.x = position.x - speed * dt;
	--	transform:Translate(position);
	--end
	--if Input.IsKeyDown(68) == true then  --D
	--	position.x = position.x + speed * dt;
	--	transform:Translate(position);
	--end
	--if Input.IsKeyDown(32) == true then  --Space
	--	local scale = transform:GetScale();
	--	transform:Scale(vec3.Addvf(scale, dt));
	--end
	--if Input.IsKeyDown(67) == true then  --C
	--	local scale = transform:GetScale();
	--	transform:Scale(vec3.Subvf(scale, dt));
	--end
	--
	--time = time + dt;
	--local r2d = Renderer2D.Get(GameObject.This());
	--r2d:SetColor({ x = (math.sin(time) + 1) / 2, y = 0, z = 0, w = (math.sin(time) + 1) / 2 });
	--
	--
	--if Input.IsKeyPressed(82) == true then  --R
	--	local texture = Texture.Get("dirt");
	--	r2d:SetTexture(texture);
	--end
	--
	--local rb2d = RigidBody2D.Get(GameObject.This());
	--rb2d:SetAllowToSleep(false);
	--rb2d:SetFixedRotation(true);
	--if Input.IsKeyPressed(82) == true then  --R
	--	local gameObject = GameObject.This();
	--	local childs = gameObject:GetChilds();
	--
	--	print("Owner: " .. gameObject:GetName());
	--	for k,v in pairs(childs) do
	--		local child = GameObject.To(v);
	--		print("Child: " .. child:GetName());
	--	end
	--end

	--if Input.IsKeyPressed(32) == true then
	--	local gameObject = GameObject.Create("TestLuaCreateGameObject");
	--	local r2d = Renderer2D.Add(gameObject);
	--	gameObject:Delete();
	--end

	--if Input.IsKeyPressed(32) == true then
	--	local gameObject = GameObject.This();
	--	local bc2d = BoxCollider2D.Get(gameObject);
	--	local ic2d = ICollider2D.To(bc2d);
	--	ic2d:SetTrigger(true);
	--end

	--if Input.IsKeyPressed(32) == true then
	--	print(GetType(Transform.Get(GameObject.This())));
	--end

	local rb2d = RigidBody2D.Get(GameObject.This());
	rb2d:SetAllowToSleep(false);
	rb2d:SetFixedRotation(true);

	speed = 10;
	local transform = Transform.Get(this);
	local position = transform:GetPosition();

	local y = Input.GetJoyStickAxis(1)  --Left Stick Y
	position.y = position.y - speed * dt * y;
	transform:Translate(position);

	local x = Input.GetJoyStickAxis(0)	 --Left Stick X
	position.x = position.x + speed * dt * x;
	transform:Translate(position);

	transform = Transform.Get(target);
	position = transform:GetPosition();

	y = Input.GetJoyStickAxis(3)  --Left Stick Y
	position.y = position.y - speed * dt * y;
	transform:Translate(position);

	x = Input.GetJoyStickAxis(2)	 --Left Stick X
	position.x = position.x + speed * dt * x;
	transform:Translate(position);

	--if Input.IsJoyStickPressed(0) == true then
	--	local projectile = GameObject.Create("Projectile");
	--	local projectileTransform = Transform.Get(projectile);
	--	local thisTransform = Transform.Get(GameObject.This());
	--	local r2d = Renderer2D.Add(projectile);
	--	projectileTransform:Translate(thisTransform:GetPosition());
	--	AttachScript(projectile, "Source/Scripts/Projectile.lua")
	--end

	local hit = Hit2D.new();
	local startvec = Transform.Get(this):GetPosition();
	--local endvec = { x = Viewport.GetMousePosition().x, y = Viewport.GetMousePosition().y, z = 0 };
	local endvec = Transform.Get(target):GetPosition();
	Raycast.RayCast2D(startvec, endvec, hit, { "Player" });
	local point = hit:GetPosition();
	if point.x ~= 0 and point.y ~= 0 then
		Visualizer.DrawLine(startvec, { x = point.x, y = point.y, z = 0 }, { x = 0, y = 0, z = 1, w = 1 });
	else
		Visualizer.DrawLine(startvec, endvec, { x = 1, y = 0, z = 0, w = 1 });
	end

	Visualizer.DrawCircle({ x = 0.3, y = 0.5 }, Transform.Get(this):GetTransform(), { x = 0, y = 0, z = 1, w = 1 });
	Visualizer.DrawCircle({ x = 0.3, y = 0.5 }, Transform.Get(target):GetTransform(), { x = 0, y = 0, z = 1, w = 1 });
end