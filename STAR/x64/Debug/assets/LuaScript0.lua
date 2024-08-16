LuaScript0 = {
 value = 1.0;
}

-- Use this for initialization
function LuaScript0:Start()
end

-- Update is called once per frame
function LuaScript0:ImGuiNewFrame()
 ImGui.Begin("My Window")
 ImGui.Text("Hello, ImGui from Lua!")
 ImGui.End()
end