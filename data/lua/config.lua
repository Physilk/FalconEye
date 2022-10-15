

programConfig = {
    --files to execute
    scripts = {
        "data/lua/functions.lua",
    },

    mainScript = "realtime.lua",
    
    lua_path = "?;?.lua;./data/lua/?;./data/lua/?.lua;./data/lua/generated/?;./data/lua/generated/?.lua",

    defaultRenderOptions = FalconEye.RayCasting.SceneRenderOption(800, 600, 60, 5, 1, ""),
}
