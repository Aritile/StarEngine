Player = {
	transformComponent = nil;
	rigidbodyComponent = nil;
}

player = nil;

-- Use this for initialization
function Player:Start()
	transformComponent = entity:GetComponent("TransformComponent")
	rigidbodyComponent = entity:GetComponent("RigidbodyComponent")
	player = entity
end

-- Update is called once per frame
function Player:Update()
	if Input.GetKeyDown(Input.KeyCode.W) then
		rigidbodyComponent:AddForce(transformComponent:GetTransform():Backward() * 15)
	end
	if Input.GetKeyDown(Input.KeyCode.S) then
		rigidbodyComponent:AddForce(transformComponent:GetTransform():Forward() * 15)
	end
	if Input.GetKeyDown(Input.KeyCode.A) then
		rigidbodyComponent:AddForce(transformComponent:GetTransform():Left() * 15)
	end
	if Input.GetKeyDown(Input.KeyCode.D) then
		rigidbodyComponent:AddForce(transformComponent:GetTransform():Right() * 15)
	end
end