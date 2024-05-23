local tilemap = require "Tilemap"

local map = tilemap.new(16, 16)
local overlay = tilemap.new(16, 16)
map:parse[[
    1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 0
    1 1 3 3  3 3 3 3  3 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1
    1 3 3 4  4 4 4 3  3 3 3 3  3 3 1 1  1 1 1 1  1 1 1 1  1 1 1 1
    1 3 4 4  4 4 4 4  4 4 4 4  4 3 3 3  3 3 3 3  3 1 1 1  1 1 1 1
    
    1 3 3 3  4 4 4 4  5 5 5 4  4 4 3 3  3 3 3 3  3 3 3 1  1 1 1 1
    1 1 1 3  3 3 5 4  4 4 5 5  4 4 4 3  3 4 5 4  4 3 3 3  1 1 1 1
    1 1 1 1  1 1 1 5  3 5 5 5  4 4 4 3  4 5 5 5  4 3 3 3  1 1 1 1
    1 1 1 1  1 3 5 5  3 4 4 4  4 4 3 3  4 4 5 5  5 5 5 3  3 1 1 1
    
    1 1 5 3  3 3 3 3  3 3 3 4  5 5 4 4  4 3 3 5  5 5 3 3  1 1 1 1
    1 3 5 4  4 4 4 4  4 5 5 5  5 5 5 4  4 4 2 4  3 3 3 1  1 3 1 1
    1 5 5 4  4 4 4 4  4 4 5 5  4 4 4 4  4 4 2 2  2 4 4 1  3 5 5 1
    1 3 3 3  4 4 4 4  4 4 5 5  4 4 4 4  4 4 4 4  4 4 4 1  3 5 5 1
    
    1 1 1 3  4 4 4 4  3 4 4 4  4 4 4 1  3 3 3 3  3 3 3 1  3 5 3 1
    1 1 1 3  3 3 4 4  3 3 3 3  3 3 3 1  5 3 1 1  1 3 1 1  1 3 3 1
    1 1 1 1  1 3 3 1  1 1 3 3  1 1 1 5  5 5 1 1  1 1 1 1  1 3 1 1
    0 1 1 1  1 1 1 1  1 1 1 1  1 1 1 5  5 1 1 1  1 1 1 1  1 1 1 0
]]
overlay:parse[[
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 6 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 6 6 6  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
    0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0
]]

function DrawGuy(x, y)
    local animationTime = math.floor(GetTime() * 2) % 4;
    Sprite(0, 464, 16, 16, x - 16, y - 28)
    Sprite(16 * animationTime, 448, 16, 16, x - 14, y - 48);
end

local trimCache = {}
local function trim(s)
    if trimCache[s] then
        return trimCache[s]
    end
    local result = {}
    for line in s:gmatch("[^\r\n]+") do
        local trimmed = line:match("^%s*(.-)%s*$")
        table.insert(result, trimmed)
    end
    local trimmed = table.concat(result, "\n")
    trimCache[s] = trimmed
    return trimmed
end

local guyPos = {x = 100, y = 100}

local function simpleBubble(text, w, h, fontsize)
    h = h or 80
    w = w or 300
    fontsize = fontsize or 20
    text = trim(text)
    return function() 
        local x, y = 40 + guyPos.x, guyPos.y - 40
        DrawBubble(x,y, w, h, 0, -12, 6)
        SetColor(0,0,32,255)
        DrawTextBoxAligned(text, fontsize, x, y, w, h, 0.5, 0.5)
    end
end

local steps = {
    {
        step = 1,
        draw = function(self) 
            local x, y = 40 + guyPos.x, guyPos.y - 40
            local h = 80
            DrawBubble(x,y, 300, h, 0, -12, 6)
            SetColor(0,0,32,255)
            DrawTextBoxAligned("Hello, World!", 20, x, y, 300, h, 0.5, 0.5)
            if self.activeTime > 2 then
                DrawTextBoxAligned("Press ENTER to continue", 15, x, y + 30, 300, h, 0.5, 0.5)
            end

        end
    },
    {
        step = 2,
        draw = simpleBubble [[
            In this demo, I'll showcase some 
            strategies around path finding.]]
    },
    {
        step = 3,
        draw = simpleBubble [[
                You can go back to a previous step
                any time by pressing BACKSPACE.]]
    },
    {
        step = 4,
        draw = simpleBubble [[
                So let's start with the topic:]]
    },
    {
        step = 5,
        draw = simpleBubble([[Path finding!]], nil, nil, 30)
    },

}

local frame = 0
function draw(dt)
    frame = frame + 1
    SetColor(255,255,255,255)

    map:draw(0,0)
    overlay:draw(0,0)
    DrawGuy(100, 100)
    local currentStep = GetCurrentStepIndex()
    for i, step in ipairs(steps) do
        if step.step == currentStep then
            if step.lastActiveFrame ~= frame then
                if step.activate then
                    step:activate()
                end
                step.activeTime = 0
            end
            step.lastActiveFrame = frame + 1
            step:draw()
            step.activeTime = step.activeTime + dt
        elseif step.lastActiveFrame == frame and step.deactivate then
            step:deactivate()
        end
    end
end