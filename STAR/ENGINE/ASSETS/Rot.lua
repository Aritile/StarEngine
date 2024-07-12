Rot = {
    transformComponent = nil;
    --rotationSpeed = Vector3.new(0.01, 0, 0);
    --currentRotation = Vector3.new(0, 0, 0);
}

-- Use this for initialization
function Rot:Start()
    transformComponent = entity:GetComponent("TransformComponent")
end

-- Update is called once per frame
function Rot:Update()
    --currentRotation = currentRotation + rotationSpeed
    transformComponent:AddRotationYawPitchRoll(Vector3.new(0.1, 0.0, 0.0))
end
