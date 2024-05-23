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

local guyPos = {x = 400, y = 100}

local function eval(value, arg)
    if type(value) == "function" then
        return value(arg)
    end
    return value
end

local function simpleBubble(text, w, h, fontsize)
    h = h or 80
    w = w or 300
    fontsize = fontsize or 20
    text = trim(text)
    return function() 
        local x, y = guyPos.x - w - 40, guyPos.y - 40
        DrawBubble(x,y, w, h, 180, w + 16, 16)
        SetColor(0,0,32,255)
        DrawTextBoxAligned(text, fontsize, x, y, w, h, 0.5, 0.5)
    end
end

local function drawAnimatedSprite(srcX, srcY, srcW, srcH, 
    dstX, dstY, scaleX, scaleY, pivotX, pivotY, frames, fps)
    return function(self)
        local time = self.activeTime
        local frame = math.floor(time * fps) % frames
        SetColor(255,255,255,255)
        local srcX = eval(srcX, time)
        local srcY = eval(srcY, time)
        local srcW = eval(srcW, time)
        local srcH = eval(srcH, time)
        local dstX = eval(dstX, time)
        local dstY = eval(dstY, time)
        local scaleX = eval(scaleX, time)
        local scaleY = eval(scaleY, time)
        local dstW = eval(scaleX, time) * srcW * 2
        local dstH = eval(scaleY, time) * srcH * 2
        -- compensate for the pivot
        dstX = dstX - pivotX * scaleX * 2
        dstY = dstY - pivotY * scaleY * 2
        Sprite(srcX + frame * srcW, srcY, srcW, srcH, dstX, dstY, dstW, dstH)
    end
end

local function lerp(start, finish, t)
    return start + (finish - start) * t
end

local function easeOutElasticTween(start, finish, duration)
    return function(t)
        t = math.min(1, t / duration)
        local p = 2 ^ (-10 * t) * math.sin((t * 10 - 0.75) * (2 * math.pi) / 3) + 1
        return start + (finish - start) * p
    end
end

local function easeOutSineTween(start, finish, duration)
    return function(t)
        t = math.min(1, t / duration)
        return lerp(start, finish, math.sin(t * math.pi / 2))
    end
end

local function bounceTween(start, bounceMax, duration)
    return function(t)
        t = math.min(1, t / duration)
        local p = math.sin(t * math.pi)
        return lerp(start, bounceMax,  p)
    end
end

local function drawArrowLine(startX, startY, endX, endY, tween, r,g,b,a)
    return function(self)
        local time = self.activeTime
        local startX = eval(startX, time)
        local startY = eval(startY, time)
        local endX = eval(endX, time)
        local endY = eval(endY, time)
        local tween = eval(tween, time)
        local x = lerp(startX, endX, tween)
        local y = lerp(startY, endY, tween)
        local dx, dy = endX - startX, endY - startY
        local len = math.sqrt(dx * dx + dy * dy)
        local nx, ny = dx / len, dy / len
        local rx, ry = -ny, nx
        local aw = 8
        local ah = 16
        SetColor(0,0,0,a)
        DrawLine(startX, startY, x, y, 8)
        DrawTriangle(
            x + rx * (aw + 2) - nx, 
            y + ry * (aw + 2) - ny, 
            x + nx * (ah + 3),
            y + ny * (ah + 3), 
            x - rx * (aw + 2) - nx, 
            y - ry * (aw + 2) - ny)

        SetColor(r,g,b,a)
        DrawLine(startX, startY, x, y, 5)
        DrawTriangle(x + rx * aw, y + ry * aw, x + nx * ah, y + ny * ah, x - rx * aw, y - ry * aw)
    end
end

local steps = {
    {
        step = 1,
        draw = simpleBubble([[Hello World!]], 180, 50, 30)
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
        draw = simpleBubble([[Path finding!]], 200, 50, 30)
    },
    {
        step = 6,
        draw = simpleBubble([[Let's say I want to move the flag down there]], 340, 40, 20)
    },
    {
        step = 6,
        draw = drawArrowLine(
            function() return guyPos.x end,
            function() return guyPos.y end,
            500, 300, easeOutSineTween(0, .8, .5), 255,128,0,255
        )
    },
    {
        step = {6,10},
        draw = drawAnimatedSprite(0, 432, 16, 16, 
            500, bounceTween(300,280,.25), 1, easeOutElasticTween(0, 1, 1.5), 8, 16, 4, 10)
    }

}

local frame = 0
function draw(dt)
    frame = frame + 1
    SetColor(255,255,255,255)

    map:draw(0,0)
    overlay:draw(0,0)
    DrawGuy(guyPos.x, guyPos.y)
    local currentStep = GetCurrentStepIndex()
    for i, step in ipairs(steps) do
        local index = step.step
        if type(index) == "table" then
            if currentStep >= index[1] and currentStep <= index[2] then
                index = currentStep
            end
        end
        if index == currentStep then
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