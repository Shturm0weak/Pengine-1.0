local _IsLanded = false;
local _Transform = nil;
local _A2d = nil;
local _R2d = nil;
local _Bc2d = nil;
local _This = nil;
local _Rb2d = nil;
local _Speed = 7;
local _Direction = 1;
local _IsStopped = false;
local _JumpForce = 150;
local _DoubleJump = true;
local _Scale = { x = 0, y = 0, z = 0 };
local _Velocity = 0;

function IsLanded()
	_IsLanded = false;

	local index = 0;

	local HalfBc2dSize = { x = _Bc2d:GetSize().x * _Scale.x * 1.1, y = _Bc2d:GetSize().y * _Scale.y * 1.1 };

	local hit1 = Hit2D.Create();
	local startVec = _Bc2d:GetPosition();
	local endVec = vec3.Add(_Bc2d:GetPosition(), { x = 0, y = -HalfBc2dSize.y, z = 0 });
	Raycast.RayCast2D(startVec, endVec, hit1, { "Player" });
	if hit1:GetPosition().x ~= 0 and hit1:GetPosition().y ~= 0 then
		--Visualizer.DrawLine(startVec, { x = hit1:GetPosition().x, y = hit1:GetPosition().y, z = 0 }, { x = 0, y = 0, z = 1, w = 1 });
		index = 1;
	end

	if index == 0 then
		local hit2 = Hit2D.Create();
		startVec = vec3.Add(_Bc2d:GetPosition(), { x = -HalfBc2dSize.x, y = 0, z = 0 });
		endVec = vec3.Add(_Bc2d:GetPosition(), { x = -HalfBc2dSize.x, y = -HalfBc2dSize.y, z = 0 });
		Raycast.RayCast2D(startVec, endVec, hit2, { "Player" });
		if hit2:GetPosition().x ~= 0 and hit2:GetPosition().y ~= 0 then
			--Visualizer.DrawLine(startVec, { x = hit2:GetPosition().x, y = hit2:GetPosition().y, z = 0 }, { x = 0, y = 0, z = 1, w = 1 });
			index = 2;
		end
	end

	if index == 0 then
		local hit3 = Hit2D.Create();
		startVec = vec3.Add(_Bc2d:GetPosition(), { x = HalfBc2dSize.x, y = 0, z = 0 });
		endVec = vec3.Add(_Bc2d:GetPosition(), { x = HalfBc2dSize.x, y = -HalfBc2dSize.y, z = 0 });
		Raycast.RayCast2D(startVec, endVec, hit3, { "Player" });
		if hit3:GetPosition().x ~= 0 and hit3:GetPosition().y ~= 0 then
			--Visualizer.DrawLine(startVec, { x = hit3:GetPosition().x, y = hit3:GetPosition().y, z = 0 }, { x = 0, y = 0, z = 1, w = 1 });
			index = 3;
		end
	end

	if index > 0 then
		_IsLanded = true;
		_DoubleJump = true;
	end
end

function Movement(dt)
	_IsStopped = false;	

	_Scale = _Transform:GetScale();
	local position = _Transform:GetPosition();

	_Velocity = Input.GetJoyStickAxis(0)	 --Left Stick X
	position.x = position.x + _Speed * dt * _Velocity;
	_Transform:Translate(position);

	if math.abs(_Velocity) < 0.00006 then
		_IsStopped = true;
	elseif _Velocity > 0 then
		_Direction = 1;
	elseif _Velocity < 0 then
		_Direction = -1;
	end

	local buttonA = Input.IsJoyStickPressed(0);
	if buttonA == true then
		if _Rb2d ~= nil then
			if _IsLanded == true then
				_Rb2d:ApplyLinearImpulseToCenter({ x = 0, y = _JumpForce }, true);
			elseif _DoubleJump == true then
				_DoubleJump = false;
				_Rb2d:SetLinearVelocity({ x = 0, y = 0 });
				_Rb2d:ApplyLinearImpulseToCenter({ x = 0, y = _JumpForce }, true);
			end
		end
	end
end

function Animate()
	if _Direction == 1 then
		_Bc2d:SetOffset({ x = -0.300, y = -1.346 });
		_R2d:SetUV(_A2d:GetOriginalUV());
	else
		_Bc2d:SetOffset({ x = 0.300, y = -1.346 });
		_R2d:SetUV(_A2d:GetReversedUV());
	end

	if _IsLanded then
		if _IsStopped == true then
			_A2d:SetSpeed(12);
			_A2d:SetCurrentAnimation(Animation2D.FromA2d(_A2d, "KnightIdle"));
		else
			_A2d:SetSpeed(12 * math.abs(_Velocity));
			_A2d:SetCurrentAnimation(Animation2D.FromA2d(_A2d, "KnightRun"));
		end
	else
		_A2d:SetSpeed(12);
		if _Transform:GetPositionDelta().y > 0 then
			_A2d:SetCurrentAnimation(Animation2D.FromA2d(_A2d, "KnightJump"));
		else
			_A2d:SetCurrentAnimation(Animation2D.FromA2d(_A2d, "KnightFall"));
		end
	end
end

function OnUpdate(dt)
	_Rb2d:SetAllowToSleep(false);

	Animate();

	IsLanded();

	Movement(dt);
end

function OnStart()
	_This = GameObject.This();
	_Transform = Transform.Get(_This);
	_Rb2d = RigidBody2D.Get(_This);
	_Rb2d:SetFixedRotation(true);
	_A2d = Animator2D.Get(_This);
	_R2d = Renderer2D.Get(_This);
	_Bc2d = BoxCollider2D.Get(_This);
	print("OnStart");
end