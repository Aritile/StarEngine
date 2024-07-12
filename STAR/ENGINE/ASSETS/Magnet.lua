Magnet = {
    generalComponent = nil;
    transformComponent = nil;
    rigidbodyComponent = nil;
    target = nil;
}

-- Use this for initialization
function Magnet:Start()
    generalComponent = entity:GetComponent("GeneralComponent")
    transformComponent = entity:GetComponent("TransformComponent")
    rigidbodyComponent = entity:GetComponent("RigidbodyComponent")
    target = Vector3.new(0, 0, 0)
end

-- Update is called once per frame
function Magnet:Update()
    Console:Info(generalComponent:GetName().ToString())
    direction = Vector3.new(target.x - transformComponent:GetPosition().x, target.y - transformComponent:GetPosition().y, target.z - transformComponent:GetPosition().z)
    rigidbodyComponent:AddForce(Vector3.new(direction:Normalize().x * 15, direction:Normalize().y * 15, direction:Normalize().z * 15))
end
