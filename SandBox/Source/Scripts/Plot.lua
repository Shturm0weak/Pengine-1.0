local l = 0.5;
local transforms = {};
local transforms0 = {};
function OnUpdate(dt)
	Visualizer.DrawLine(transforms[0]:GetPosition(), transforms[1]:GetPosition(), { x = 0, y = 0, z = 1, w = 1 });
	Visualizer.DrawLine(transforms[1]:GetPosition(), transforms[2]:GetPosition(), { x = 0, y = 0, z = 1, w = 1 });
	Visualizer.DrawLine(transforms[1]:GetPosition(), transforms[3]:GetPosition(), { x = 0, y = 0, z = 1, w = 1 });
	
	Visualizer.DrawLine(transforms0[0]:GetPosition(), transforms0[1]:GetPosition(), { x = 0, y = 0, z = 0, w = 1 });
	Visualizer.DrawLine(transforms0[1]:GetPosition(), transforms0[2]:GetPosition(), { x = 0, y = 0, z = 0, w = 1 });
	Visualizer.DrawLine(transforms0[1]:GetPosition(), transforms0[3]:GetPosition(), { x = 0, y = 0, z = 0, w = 1 });

end

function Plot0()
	gameObject1 = GameObject.Create("Dot1");
	transform1 = Transform.Get(gameObject1);
	transform1:Translate({ x = 0, y = 0, z = 0.0 });
	transforms0[0] = transform1;
	
	gameObject2 = GameObject.Create("Dot2");
	transform2 = Transform.Get(gameObject2);
	transform2:Translate({ x = 0 + l, y = 0 , z = 0.0 });
	transforms0[1] = transform2;
	
	gameObject3 = GameObject.Create("Dot3");
	transform3 = Transform.Get(gameObject3);
	transform3:Translate({ x = l, y = -2 * l, z = 0.0 });
	transforms0[2] = transform3;

	gameObject4 = GameObject.Create("Dot4");
	transform4 = Transform.Get(gameObject4);
	transform4:Translate({ x = 2 * l, y = 0, z = 0.0 });
	transforms0[3] = transform4;
end

function Plot1()
	gameObject1 = GameObject.Create("Dot1");
	transform1 = Transform.Get(gameObject1);
	transform1:Translate({ x = 0, y = -0.38, z = 0.0 });
	transforms[0] = transform1;
	
	gameObject2 = GameObject.Create("Dot2");
	transform2 = Transform.Get(gameObject2);
	transform2:Translate({ x = 0 + l, y = -0.14 , z = 0.0 });
	transforms[1] = transform2;
	
	gameObject3 = GameObject.Create("Dot3");
	transform3 = Transform.Get(gameObject3);
	transform3:Translate({ x = 0.26 + l, y = -0.14  - 2 * l, z = 0.0 });
	transforms[2] = transform3;

	gameObject4 = GameObject.Create("Dot4");
	transform4 = Transform.Get(gameObject4);
	transform4:Translate({ x = 2 * l, y = 0, z = 0.0 });
	transforms[3] = transform4;
end

function Plot2()
	gameObject1 = GameObject.Create("Dot1");
	transform1 = Transform.Get(gameObject1);
	transform1:Translate({ x = 0, y = 0.1, z = 0.0 });
	transforms[0] = transform1;
	
	gameObject2 = GameObject.Create("Dot2");
	transform2 = Transform.Get(gameObject2);
	transform2:Translate({ x = 0 + l, y = 0.27, z = 0.0 });
	transforms[1] = transform2;
	
	gameObject3 = GameObject.Create("Dot3");
	transform3 = Transform.Get(gameObject3);
	transform3:Translate({ x = 0.3 + l, y = 0.27 - 2 * l, z = 0.0 });
	transforms[2] = transform3;

	gameObject4 = GameObject.Create("Dot4");
	transform4 = Transform.Get(gameObject4);
	transform4:Translate({ x = 2 * l, y = 0, z = 0.0 });
	transforms[3] = transform4;
end

function OnStart()
	Plot0();
	Plot1();
end