Sun = {}
Sun.__index = Sun

function Sun:appendUpdate(dt)
    self.cpp.rotation.y = self.cpp.rotation.y + dt / 10
end

function Sun:new(shader, image)
    local cpp = objects.RenderObject.new(shader, image)
    local self = setmetatable({}, Sun)
    cpp.lua_instance = self
    self.cpp = cpp
    _G.keep_alive = _G.keep_alive or {}
    table.insert(_G.keep_alive, self)

    cpp.meshes[1].sizeOffset = cpp.meshes[1].sizeOffset * 1392000000
    cpp.cullPriority = objects.CullPriority.High

    return cpp
end

Earth = {}
Earth.__index = Earth

function Earth:appendUpdate(dt)
    -- rotation.y += deltaTime / 10;
end

function Earth:new(shader, image)
    local cpp = objects.RenderObject.new(shader, image)
    local self = setmetatable({}, Earth)
    cpp.lua_instance = self
    self.cpp = cpp
    _G.keep_alive = _G.keep_alive or {}
    table.insert(_G.keep_alive, self)

    cpp.meshes[1].sizeOffset = cpp.meshes[1].sizeOffset * 12756000
    cpp.position.y = cpp.position.y - bigvars.Bigint:new("6378000") -- the size divided by two
    cpp.cullPriority = objects.CullPriority.High

    return cpp
end

local pos = bigvars.Bigint.getHoweverManyDigits(0)

local sun = Sun:new(shader1, image1)
sun.position.x = sun.position.x - bigvars.Bigint:new("150000000000")
sun.position.x = sun.position.x + pos

local earth = Earth:new(shader1, image1)
earth.position.x = earth.position.x + pos
earth.position.y = earth.position.y - bigvars.Bigint:new(10)

--mySun:setupObject(shader1, image1)

-- put all non overridden render objects in here so they wont get garbage collected, this thing isn't special and you can really put it in any table
sceneObjects = {}

-- makes the cubes

local chunks = objects.MeshChunks:new(shader1, image1)
--chunks.position.y = chunks.position.y + bigvars.Bigint:new(200)
chunks.position.x = chunks.position.x + pos
table.insert(sceneObjects, chunks)

local cube = objects.RenderObject:new(shader1, image1)
cube.position.x = cube.position.x + pos
table.insert(sceneObjects, cube)

local cube2 = objects.RenderObject:new(shader1, image1)
cube2.position.x = cube2.position.x - bigvars.Bigint:new(10)
cube2.position.x = cube2.position.x + pos
table.insert(sceneObjects, cube2)

local cube3 = objects.RenderObject:new(shader1, image1)
cube3.position.x = cube3.position.x + bigvars.Bigint:new(10)
cube3.position.x = cube3.position.x + pos
table.insert(sceneObjects, cube3)

NUM_TEMPS = 11;
temp = {};

for i = 1, NUM_TEMPS do
    table.insert(temp, objects.RenderObject:new(shader1, image1))
    temp[i].position.z = temp[i].position.z + bigvars.Bigint:new(10 * i)
    temp[i].position.x = temp[i].position.x + pos
end
