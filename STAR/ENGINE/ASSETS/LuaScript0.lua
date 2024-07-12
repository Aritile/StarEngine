LuaScript0 = {}

-- Use this for initialization
function LuaScript0:Start()
	x = entity:FindByName("Cube")
	x:GetComponent("GeneralComponent"):SetName("xd")
end

-- Update is called once per frame
function LuaScript0:Update()
end