function OnDestroy(self)
	print("[LUA] OnDestroy!")
end

function OnCreate(self)
	print("[LUA] OnCreate!")
end

function OnCollisionEnter(self, collision)
	print("[LUA] OnCollisionEnter!")
end

function OnCollisionExit(self)
	print("[LUA] OnCollisionExit!")
end

local function Test_Identifier(self, dt)
	print("[LUA] BEGIN TEST: \"IDENTIFIER\"")

	local name = Identifier.GetDebugName(self)

	if name == "Orc" then
		Identifier.SetDebugName(self, "Berserker Orc")
	end

	print("[LUA] DebugName: " .. name)
end

local function Test_Vector2Color(self, dt)
	print("[LUA] BEGIN TEST: \"VECTOR2 + COLOR\"")

	local u = Vec2(2.0, 1.0)
	local v = Vec2(10.0, 1.0)

	v = Vec2.DivV(v, u) -- 5.0, 1.0

	v = Vec2.AddV(v, Vec2(5.0, 0.0)) -- 10.0, 1.0

	v = Vec2.MulV(v, u) -- 20.0, 1.0

	v = Vec2.SubV(v, Vec2(0.0, 1.0)) -- 20.0, 0.0
		
	v = Vec2.AddN(v, 80.0) -- 100.0, 80.0

	print("[LUA] Vector2: {".. v.X .. ":".. v.Y .."}")


	local color = Color(255, 255, 255, 0)

	local a = color.a

	color.a = a + 255
	
	print("[LUA] Color: {".. color.R .. ":".. color.G ..":".. color.B ..":".. color.A .."}")
end

local function Test_Hierarchy(self, dt)
	print("[LUA] BEGIN TEST: \"SCENE HIERARCHY\"")

	local name = Identifier.GetDebugName(self)

	if Hierarchy.HasParent(self) then
		parent = Hierarchy.GetParent(self)
		parent_name = Identifier.GetDebugName(parent)
		print("[LUA] " .. name .. " has a parent: " .. parent_name .. "!")	
	else
		print("[LUA] " .. name .. " has no parent!")
	end
end

local function Test_Transform(self, dt)
	print("[LUA] BEGIN TEST: \"TRANSFORM\"")

	local name = Identifier.GetDebugName(self)

	local position = Transform.GetPosition(self)

	print("[LUA] " .. name .. " Position: {".. position.X ..":".. position.Y .."}")	

	if position.X < 500 then
		Transform.Translate(self, Vec2(1.0 * dt, 0.0))
	elseif position.X > 500 then
		Transform.Translate(self, Vec2(-1.0 * dt, 0.0))
	end

	local rotation = Transform.GetRotation(self)

	if rotation > 360 then
		Transform.SetRotationLocal(self, 0.0)
	end

	print("[LUA] " .. name .. " Rotation: {".. rotation .."}")	

	Transform.Rotate(self, 1.0)
end

local function Test_Sprite(self, dt)
	print("[LUA] BEGIN TEST: \"SPRITE\"")


	color = Sprite.GetColor(self)

	size = Sprite.GetSize(self)

	print("[LUA] Color: {".. color.R .. ":".. color.G ..":".. color.B ..":".. color.A .."}")

	if color.A >= 255 then
		Sprite.SetColor(self, Color(255, 20, 50, 0))
	elseif color.A < 255 then
		Sprite.SetColor(self, Color(255, 0, 0, color.A + 1))
	end

	Sprite.SetScale(self, Vec2(10.0, 10.0))

	scale = Sprite.GetScale(self)

	Sprite.SetOrigin(self, Vec2((scale.X * size.X) / 2, (scale.Y * size.Y) / 2))
end

local function Test_AnimatedSprite(self, dt)
	print("[LUA] BEGIN TEST: \"ANIMATION\"")

	local name = Identifier.GetDebugName(self)

	local id = Animation.GetAnimatorIdentifier(self)
	local frames = Animation.GetFrameCount(self)

	print("[LUA] " .. name .. " Animator: {".. id .."} with {".. frames.X ..":".. frames.Y .."} frames!")	

	local speed = Animation.GetAnimationSpeed(self)

	if speed >= 1024 then
		Animation.SetAnimationSpeed(self, 1)
	elseif speed < 1024 then
		Animation.SetAnimationSpeed(self, speed + 1)
	end
end

local function Test_Input(self, dt)
	print("[LUA] BEGIN TEST: \"INPUT\"")

	if Input.KeyPressed("W") then
		print("[LUA] \"W\" pressed!")
	elseif Input.KeyPressed("A") then
		print("[LUA] \"a\" pressed!")
	end

	if Input.GetMouseWheel() > 0.0 then
		print("[LUA] Mouse Wheel: Up!")
	elseif Input.GetMouseWheel() < 0.0 then
		print("[LUA] Mouse Wheel: Down!")
		Input.SetMousePosition(Vec2(0.0, 0.0))
	end


	local mouse = Input.GetMousePosition()

	print("[LUA] Mouse Position: {".. mouse.X ..":".. mouse.Y .."}")

end


local function Test_Camera(camera, dt)
	print("[LUA] BEGIN TEST: \"CAMERA\"")

	zoom = camera:GetZoom()
	rotation = camera:GetRotation()

	if zoom < 10 then
		camera:Zoom(0.001)
	elseif zoom >= 10 then
		camera:SetZoom(1.0)
	else
	end

	if rotation < 45 then
		camera:Rotate(1.0)
	elseif zoom >= 45 then
		camera:SetRotation(0.0)
	else
	end

end

local function Test_Scene(dt)
	print("[LUA] BEGIN TEST: \"SCENE\"")

	local scene = Scene.GetActiveScene()

	if(scene:IsReady()) then
		scene_name = scene:GetSceneName()
		entities = scene:GetEntities()

		print("[LUA] Active Scene: {".. scene_name .."}")
		print("[LUA] Entities: {".. #entities .."}")

		camera = scene:GetActiveCamera()

		Test_Camera(camera, dt)

	end
end

local function Test_Physics(dt)
	print("[LUA] BEGIN TEST: \"PHYSICS\"")




	print("[LUA] ... Successful!")
end


function OnUpdate(self, dt)
	print("[LUA] OnUpdate!")

	-- IDENTIFIER
	--Test_Identifier(self, dt)

	-- VECTOR2 + COLOR
	--Test_Vector2Color(self, dt)

	-- SCENEHIERARCHY
	--Test_Hierarchy(self, dt)

	-- TRANSFORM
	--Test_Transform(self, dt)

	-- SPRITE
	--Test_Sprite(self, dt)

	-- ANIMATION
	--Test_AnimatedSprite(self, dt)

	-- INPUT
	--Test_Input(self, dt)

	-- SCENE
	--Test_Scene(dt)

	-- PHYSICS
	Test_Physics(dt)

end