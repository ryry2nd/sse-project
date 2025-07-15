Sun = {}
Sun.__index = Sun

function Sun:appendUpdate(dt)
    self.cpp.rotation.y = self.cpp.rotation.y + dt / 10
end

function Sun:new(shader, slimShader, image)
    local cpp = Objects.RenderObject.new(shader, slimShader, image)
    local self = setmetatable({}, Sun)
    cpp.lua_instance = self
    self.cpp = cpp
    _G.keep_alive = _G.keep_alive or {}
    table.insert(_G.keep_alive, self)

    cpp.meshes[1].sizeOffset = cpp.meshes[1].sizeOffset * 1392000000
    cpp.cullPriority = Objects.CullPriority.High

    return cpp
end

Earth = {}
Earth.__index = Earth

function Earth:appendUpdate(dt)
    -- rotation.y += deltaTime / 10;
end

function Earth:new(shader, slimShader, image)
    local cpp = Objects.RenderObject.new(shader, slimShader, image)
    local self = setmetatable({}, Earth)
    cpp.lua_instance = self
    self.cpp = cpp
    _G.keep_alive = _G.keep_alive or {}
    table.insert(_G.keep_alive, self)

    cpp.meshes[1].sizeOffset = cpp.meshes[1].sizeOffset * 12756000
    cpp.position.y = cpp.position.y - bigvars.Bigint:new("6378000") -- the size divided by two
    cpp.cullPriority = Objects.CullPriority.High

    return cpp
end

-- Fps_counter = {}
-- Fps_counter.__index = Fps_counter

-- function Fps_counter:new(shader, image)
--     local cpp = Objects.RenderObject2d.new(shader)
--     local self = setmetatable({} Fps_counter)
--     cpp.lua_instance = self
--     self.cpp = cpp
--     _G.keep_alive = _G.keep_alive or {}
--     table.insert(_G.keep_alive, self)
-- end

-- put all non overridden render objects in here so they wont get garbage collected, this thing isn't special and you can really put it in any table
sceneObjects = {}

local pos = bigvars.Bigint.getHoweverManyDigits(0)

Objects.globalCamera.position.x = Objects.globalCamera.position.x + pos
Objects.globalCamera.position.z = Objects.globalCamera.position.z - bigvars.Bigint:new(10)
Objects.globalCamera.rotation.y = Objects.globalCamera.rotation.y + 180

-- local playerCube = Objects.RenderObject:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
-- playerCube:makeParent(Objects.globalCamera)
-- playerCube.position = bigvars.BigVec3:new(glm.vec3:new(0, -1, 0))
-- table.insert(sceneObjects, playerCube)

local sun = Sun:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
sun.position.x = sun.position.x - bigvars.Bigint:new("150000000000")
sun.position.x = sun.position.x + pos

local earth = Earth:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
earth.position.x = earth.position.x + pos
earth.position.y = earth.position.y - bigvars.Bigint:new(10)

-- makes the cubes

-- local chunks = objects.MeshChunks:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
-- --chunks.position.y = chunks.position.y + bigvars.Bigint:new(200)
-- chunks.position.x = chunks.position.x + pos
-- table.insert(sceneObjects, chunks)

local cube = Objects.RenderObject:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
cube.position.x = cube.position.x + pos
table.insert(sceneObjects, cube)

local cube2 = Objects.RenderObject:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
cube2.position.x = cube2.position.x - bigvars.Bigint:new(10)
cube2.position.x = cube2.position.x + pos
table.insert(sceneObjects, cube2)

local cube3 = Objects.RenderObject:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish"))
cube3.position.x = cube3.position.x + bigvars.Bigint:new(10)
cube3.position.x = cube3.position.x + pos
table.insert(sceneObjects, cube3)

NUM_TEMPS = 11;
temp = {};

for i = 1, NUM_TEMPS do
    table.insert(temp,
        Objects.RenderObject:new(get_shader("basic_shader"), get_shader("point_shader"), get_image("Fish")))
    temp[i].position.z = temp[i].position.z + bigvars.Bigint:new(10 * i)
    temp[i].position.x = temp[i].position.x + pos
end
