Cam = {
	transformComponentPlayer = nil;
	transformComponentCam = nil;
}

player = nil;

-- Use this for initialization
function Cam:Start()
	transformComponentPlayer = player:GetComponent("TransformComponent")
	transformComponentCam = entity:GetComponent("TransformComponent")
end

-- Update is called once per frame
function Cam:Update()
	transformComponentCam:LookAt(transformComponentPlayer:GetTransform())
end