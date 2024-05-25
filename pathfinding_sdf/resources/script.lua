local tilemap = require "Tilemap"

local controlKeywords = {
    "function", "if", "then", "else", "elseif", "end", 
    "for", "while", "do", "repeat", "until", "return", 
    "break", "goto", "in"
}
local literalKeywords = { "nil", "true", "false" }
local expressionKeywords = { "and", "or", "not" }
local scopingKeywords = { "local" }
local keywords = {}
local function feedKeywords(list, color) 
    color = "[color=" .. color .. "]"
    for i, word in ipairs(list) do 
        keywords[word] = color 
    end 
end
feedKeywords(controlKeywords, "408f")
feedKeywords(literalKeywords, "00af")
feedKeywords(expressionKeywords, "444f")
feedKeywords(scopingKeywords, "006f")

local function syntaxHighlightLua(text)
    local minIndention = 255
    local lines = {}
    for line in text:gmatch "[^\r\n]*" do
        local indention = line:match "^%s*"
        if indention then
            minIndention = math.min(minIndention, #indention)
        end
        lines[#lines+1] = line
    end
    for i=1,#lines do
        local line = lines[i]
        line = line:sub(minIndention + 1)
        line = line:gsub("%w+", function(word)
            local color = keywords[word]
            if color then
                return color .. word .. "[/color]"
            end
        end)
        lines[i] = ("[color=4448]%02d[/color] %s"):format(i, line)
    end
    return table.concat(lines, "\n")
end

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

local mapWidth, mapHeight = map:getSize()

function GetBlockedMap(map)
    local blockedMap = {}
    local w,h = map:getSize()
    -- local m = {}
    for y=0,h - 1 do
        local index = y * w
        for x=0,w - 1 do
            local tileId = map:getValue(x, y)
            blockedMap[index + x] = tileId < 3 or tileId == 5
            -- m[#m + 1] = blockedMap[index + x] and "_" or "1"
        end
        -- m[#m+1] = "\n"
    end
    -- print(table.concat(m, ""))
    return blockedMap
end

local trimCache = {}
local function trim(s)
    if trimCache[s] then
        return trimCache[s]
    end
    local result = {}
    for line in s:gmatch("[^\r\n]*") do
        local trimmed = line:match("^%s*|?(.-)%s*$")
        table.insert(result, trimmed)
    end
    if result[#result] == "" then
        table.remove(result)
    end
    local trimmed = table.concat(result, "\n")
    trimCache[s] = trimmed
    return trimmed
end

local guyPos = {x = 400.5, y = 100}
local guyTarget = {x = 400.5, y = 100}
local guySpeed = 1
local guyIsMoving = false
local function guyUpdate(dt)
    local dx = guyTarget.x - guyPos.x
    local dy = guyTarget.y - guyPos.y
    local len = math.sqrt(dx * dx + dy * dy)
    if len > 10 then
        guyIsMoving = true
        guyPos.x = guyPos.x + dx / len * guySpeed
        guyPos.y = guyPos.y + dy / len * guySpeed
    else
        guyIsMoving = false
    end
end

local function guyDraw()
    local x, y = guyPos.x, guyPos.y
    local t = GetTime()
    if guyIsMoving then
        y = y - math.abs(math.sin(t * 15) * 8)
    end
    local animationTime = math.floor(t * 2) % 4;
    Sprite(0, 464, 16, 16, x - 16, y - 28)
    Sprite(16 * animationTime, 448, 16, 16, x - 14, y - 48);
end


local function eval(value, arg)
    if type(value) == "function" then
        return value(arg)
    end
    return value
end

local codeExecutorRunning = false
local codeExecutorAlpha = 255
local codeExecutorShowLine = nil
local function codeExecutor(code, x, y, w, h, fontsize, contextEnv, stackInfoHandler)
    local func, err = load(code, nil, "t", contextEnv)
    if not func then
        print(err)
        return
    end

    local highlightedText = trim(err or syntaxHighlightLua(code))
    w = w or 400
    fontsize = fontsize or 20
    x = x or math.floor((GetScreenSize() - w) / 2)
    y = y or 15
    h = h or 200
    local coro
    local currentStep = 1
    local totalLineExec = 0
    local getInfosAt
    function getInfosAt(step)
        coro = coroutine.create(func)
        local stepCount = 0
        local stackInfo = { line = 0}
        local stackStart = 2
        local cloneCache = {}
        local function clone(value)
            if type(value) ~= "table" then
                return value
            end
            if cloneCache[value] then
                return cloneCache[value]
            end
            local result = {}
            cloneCache[value] = result
            for k,v in pairs(value) do
                result[k] = clone(v)
            end
            return result
        end
        debug.sethook(coro, function(event, line)
            stepCount = stepCount + 1
            if stepCount == step then
                stackInfo.line = line
                for stackIndex = stackStart, 200 do
                    local info = {}
                    local func = debug.getinfo(stackIndex, "fnl")
                    if not func then
                        break
                    end
                    info.func = func
                    info.locals = {}
                    for valueIndex = 1, 200 do
                        local name, value = debug.getlocal(stackIndex, valueIndex)
                        if not name then
                            break
                        end
                        if name ~= "(temporary)" then
                            info.locals[#info.locals+1] = {name = name, value = clone(value) }
                        end
                    end
                    stackInfo[#stackInfo+1] = info
                end

            end
        end, "l")
        local suc, err = coroutine.resume(coro)
        if not suc then
            print(err)
        end
        return stackInfo
    end
    local t1 = GetTime()
    coro = coroutine.create(func)
    debug.sethook(coro, function(event, line)
        totalLineExec = totalLineExec + 1
    end, "l")
    local suc, err = coroutine.resume(coro)
    if not suc then
        print(err)
    end
    local dt = GetTime() - t1
    print("Executed code in ",dt)

    local stepsPerSecond = 50
    local nextStep = GetTime() + 1 / stepsPerSecond
    local currentLine = 1
    local currentStackInfo = {}
    return function(step)
        if step.activeTime == 0 then
            currentStep = 0
            nextStep = 0
        end
        if GetTime() > nextStep and currentStep < totalLineExec and getInfosAt and codeExecutorRunning then
            nextStep = GetTime() + 1 / stepsPerSecond
            currentStep = currentStep + 1
            currentStackInfo = getInfosAt(currentStep)
            currentLine = currentStackInfo.line
        end
        if stackInfoHandler then
            stackInfoHandler(step, currentStackInfo)
        end
        SetColorAlpha(codeExecutorAlpha)
        SetColor(240,230,200,255)
        DrawRectangle(x, y, w, h)
        local currentLineY = 8 + math.abs(fontsize) * (currentLine - 1)
        BeginScissorMode(x, y, w, h)
        SetColor(255,190,90,255)
        DrawRectangle(x, currentLineY, w, math.abs(fontsize))
        --print(currentLineY)
        
        SetColor(0,0,0,255)
        DrawTextBoxAligned(highlightedText, fontsize, x+8, y+8, w-16, h-16, 0, 0)
        SetColor(220,200,130,255)
        local stackWindowWidth = 250
        local stackWindowX = x + w - stackWindowWidth
        DrawRectangle(stackWindowX, y, stackWindowWidth, h)
        SetColor(0,0,0,255)
        DrawTextBoxAligned("Stack", 20, stackWindowX + 10, y + 5, stackWindowWidth - 20, 30, 0, 0)
        local stackY = y + 30
        for i=1,#currentStackInfo do
            SetColor(64,64,64,255)
            DrawRectangle(stackWindowX, stackY - 5, stackWindowWidth, 22)
            local info = currentStackInfo[i]
            local func = info.func
            local locals = info.locals
            SetColor(255,255,255,255)
            DrawTextBoxAligned(("%d: %s"):format(#currentStackInfo - i, func.name or "<???>"), 15, stackWindowX + 10, stackY, stackWindowWidth - 20, 20, 0, 0)
            stackY = stackY + 20
            for j=1,#locals do
                local localInfo = locals[j]
                SetColor(0,0,0,255)
                DrawTextBoxAligned(localInfo.name, 15, stackWindowX + 10, stackY, stackWindowWidth - 20, 20, 0, 0)
                SetColor(0,0,0,255)
                DrawTextBoxAligned(tostring(localInfo.value), 15, stackWindowX + 10, stackY, stackWindowWidth - 20, 20, 1, 0)
                stackY = stackY + 20
            end
        end
        EndScissorMode()
        -- draw progress bar
        SetColor(128,128,200,255)
        local progressBarWidth = w - stackWindowWidth
        local progressBarHeight = 20
        DrawRectangle(x, y + h, progressBarWidth, progressBarHeight)
        SetColor(255,190,90,255)
        local progress = currentStep / totalLineExec
        DrawRectangle(x+2, y + h + 2, (progressBarWidth - 4) * progress, progressBarHeight - 4)
        SetColorAlpha(255)
    end
end

local function detachedBubble(text, x, y, w, h, fontsize, linespacing)
    w = w or 400
    fontsize = fontsize or 20
    linespacing = linespacing or 1
    local lineHeight = math.floor(math.abs(fontsize) * linespacing + .5)
    if not h then
        h = 40
        for line in text:gmatch("[^\r\n]*") do
            h = h + lineHeight
        end
    end
    text = trim(text)
    x = x or math.floor((GetScreenSize() - w) / 2)
    y = y or 15
    return function()
        local posX = math.floor(guyPos.x - x)
        posX = math.max(16, math.min(w - 16, posX))

        SetColor(255,255,255,255)
        DrawBubble(x,y, w, h, 270, posX, h + 16)
        SetColor(0,0,32,255)
        SetLineSpacing(lineHeight - math.abs(fontsize))
        DrawTextBoxAligned(text, fontsize, x, y, w, h, 0.5, 0.5, lineHeight)
        SetLineSpacing(0)
    end
end

local function speechBubblePointAt(text, x, y, w, h, arrowAngle, arrowX, arrowY, fontsize)
    w = w or 300
    fontsize = fontsize or 20
    if not h then
        h = 40
        for line in text:gmatch("[^\r\n]*") do
            h = h + fontsize
        end
    end
    text = trim(text)
    return function() 
        x,y = math.floor(x), math.floor(y)
        arrowX = math.floor(arrowX)
        SetColor(255,255,255,255)
        DrawBubble(x,y, w, h, arrowAngle, arrowX, arrowY)
        SetColor(0,0,32,255)
        DrawTextBoxAligned(text, fontsize, x, y, w, h, 0.5, 0.5)
    end
end

local function speechBubble(text, w, h, fontsize)
    w = w or 300
    fontsize = fontsize or 20
    if not h then
        h = 40
        for line in text:gmatch("[^\r\n]*") do
            h = h + fontsize
        end
    end
    text = trim(text)
    return function() 
        local x, y = guyPos.x - w - 40, math.max(5, guyPos.y - h - 20)
        local arrowX = w + 16
        local arrowAngle = 180
        if x < 0 then
            x = guyPos.x + 40
            arrowX = -32
            arrowAngle = 0
        end
        x,y = math.floor(x), math.floor(y)
        arrowX = math.floor(arrowX)
        local arrowY = math.floor(guyPos.y - y - 40)
        SetColor(255,255,255,255)
        DrawBubble(x,y, w, h, arrowAngle, arrowX, arrowY)
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

local function elasticOut(t)
    return 2 ^ (-10 * t) * math.sin((t * 10 - 0.75) * (2 * math.pi) / 3) + 1
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

local function delayTween(duration, f)
    return function(t)
        return f(t - duration)
    end
end
local function drawArrowLine(startX, startY, endX, endY, tween, r,g,b,a)
    return function(self)
        local time = self.activeTime
        local tween = eval(tween, time)
        if tween < 0 then
            return
        end
        local startX = eval(startX, time)
        local startY = eval(startY, time)
        local endX = eval(endX, time)
        local endY = eval(endY, time)
        local x = lerp(startX, endX, tween)
        local y = lerp(startY, endY, tween)
        local dx, dy = endX - startX, endY - startY
        local len = math.sqrt(dx * dx + dy * dy)
        local nx, ny = dx / len, dy / len
        local rx, ry = -ny, nx
        local aw = 8
        local ah = 16
        SetColor(0,0,0,a)
        DrawLine(startX, startY, x, y, 9)
        DrawTriangle(
            x + rx * (aw + 4) - nx * 2, 
            y + ry * (aw + 4) - ny * 2, 
            x + nx * (ah + 4),
            y + ny * (ah + 4), 
            x - rx * (aw + 4) - nx * 2, 
            y - ry * (aw + 4) - ny * 2)

        SetColor(r,g,b,a)
        DrawLine(startX, startY, x, y, 5)
        DrawTriangle(x + rx * aw, y + ry * aw, x + nx * ah, y + ny * ah, x - rx * aw, y - ry * aw)
    end
end

local function moveGuyTo(x, y)
    return function(self)
        -- warp the guy to the target to avoid clipping
        -- would be cool to have a warp effect if it's needed
        local dx, dy = guyPos.x - x, guyPos.y - y
        local dist = math.sqrt(dx * dx + dy * dy)
        if dist > 20 then
            guyPos.x = guyTarget.x
            guyPos.y = guyTarget.y
        end
        guyTarget.x = x
        guyTarget.y = y
    end
end

local function drawMapInfoSquares(minX, minY, maxX, maxY)
    return function (step)
        for x = minX, maxX - 1 do 
            for y = minY, maxY - 1 do 
                local tileId = map:getValue(x + 1, y + 1)
                if tileId < 3 or tileId == 5 then
                    SetColor(128,0,0,128)
                    local px = x * 32 + 16
                    local py = y * 32 + 16
                    local tOffset = x + y
                    local t = math.min(step.activeTime * 2 - tOffset * .1, 1)
                    if t > 0 then
                        t = elasticOut(t)
                        local s = 32 * t
                        DrawRectangle(px + 16 - s * .5, py + 16 - s * .5, s, s)
                    end
                end
            end
        end
    end
end

local function drawGrid(minX, minY, maxX, maxY)
    return function(step)
        
        SetColor(0,0,0,128)
        local centerX = 32 * (maxX - minX) * .5 + 16
        local centerY = 32 * (maxY - minY) * .5 + 16
        for x=minX, maxX do
            local px = x * 32 + 16
            local offset = math.abs((px - centerX) * .0025)
            local t = math.min(1, (step.activeTime - offset) * .25)
            if t > 0 then
                t = elasticOut(t);
                local lineLenH = 32 * (maxY - minX) * t
                DrawRectangle(px, centerY - lineLenH * .5, 1, lineLenH)
            end
        end
        for y=minY, maxY do
            local py = y * 32 + 16
            local offset = math.abs((py - centerY)) * .0025
            local t = math.min(1,(step.activeTime - offset) * .25)
            if t > 0 then
                t = elasticOut(t)
                local lineLenW = 32 * (maxX - minX) * t
                DrawRectangle(centerX - lineLenW * .5, py, lineLenW, 1)
            end
        end
    end
end

local steps = {
    {
        chapter = "Introduction",
        step = 0,
        draw = speechBubble([[
            Hello World!
            This is a walk through different path finding 
            topics from beginner level to fairly advanced
            topics.
            Press <ENTER> to get to the next step and
            <BACKSPACE> to go back a step.

            You can also open a menu by pressing
            <SPACE> to see a list of all chapters.
            ]], 355)
    },
    {
        step = {0, 2},
        draw = moveGuyTo(400, 100)
    },
    {
        step = 1,
        draw = speechBubble([[
            By the way, the code for this 
            application is open source and 
            contains not only path finding 
            algorithms but also covers tile 
            rendering using auto-tiling and 
            Lua scripting bindings.]])
    },
    {
        step = 2,
        draw = speechBubble [[
                You can go back to a previous step
                any time by pressing BACKSPACE.]]
    },
    {
        chapter = "Path Finding Basics",
        step = 0,
        draw = speechBubble([[
                Chapter 1:
                Path finding basics]], 250, 100, 30)
    },
    {
        step = 1,
        draw = speechBubble([[
            Let's say I want to move to the flag
            down there ...]])
    },
    {
        step = 2,
        draw = speechBubble([[
            ... then this isn't a problem.]])
    },
    {
        step = 3,
        draw = detachedBubble([[
            In this situation however it isn't as simple:
            The plateau is blocking my path.
            
            This is where path finding comes in: I need 
            an algorithm to find a way around the plateau,
            telling me, which directions I need to take.
            
            We will start with simple approaches and work
            through to more advanced algorithms.]])
    },
    {
        step = {4,5},
        draw = drawGrid(0,0,24,13)
    },
    {
        step = 5,
        draw = drawMapInfoSquares(0,0,24,13)
    },
    {
        step = 4,
        draw = detachedBubble [[
            Before we start, we have to describe what we see
            in a way that the computer can understand.

            For that purpose, we'll use a simple grid of squares.
        ]]
    },
    {
        step = 5,
        draw = detachedBubble[[
            Now we mark the squares that I can't walk on.
            This way, we have something the computer 
            can work with.
        ]]
    },
    {
        step = {0, 1},
        activate = moveGuyTo(400, 100)
    },
    {
        step = {2, 5},
        activate = moveGuyTo(512, 298)
    },
    {
        step = {1, 2},
        draw = drawArrowLine(
            function() return guyPos.x end,
            function() return guyPos.y end,
            500, 300, delayTween(.5, easeOutSineTween(0, .8, .5)), 255,128,0,255
        )
    },
    {
        step = 3,
        draw = drawArrowLine(
            function() return guyPos.x end,
            function() return guyPos.y end,
            200, 290, delayTween(.5, easeOutSineTween(0, .8, .5)), 255,128,0,255
        )
    },
    {
        step = {1,2},
        draw = drawAnimatedSprite(0, 432, 16, 16, 
            500, bounceTween(300,280,.25), 1, easeOutElasticTween(0, 1, 1.5), 8, 16, 4, 10)
    },
    {
        step = {3,5},
        draw = drawAnimatedSprite(0, 432, 16, 16, 
            192, bounceTween(300,280,.25), 1, easeOutElasticTween(0, 1, 1.5), 8, 16, 4, 10)
    },

    {
        chapter = "Depth First Search",
        step = {0,5},
        draw = drawGrid(0,0,24,13)
    },
    {
        step = {0,5},
        draw = drawMapInfoSquares(0,0,24,13)
    },
    {
        step = 0,
        draw = detachedBubble([[
                Chapter 2:
                Depth First Search]], nil, nil, nil, nil, 30)
    },
    {
        step = 1,
        draw = detachedBubble[[
            Depth First Search can be implemented
            with a fairly simple recursive algorithm:
        ]]
    },
    {
        step = {0, 5},
        activate = moveGuyTo(512, 298)
    },
    {
        step = {0,5},
        draw = drawAnimatedSprite(0, 432, 16, 16, 
            192, bounceTween(300,280,.25), 1, easeOutElasticTween(0, 1, 1.5), 8, 16, 4, 10)
    },
    {
        step = {2,5},
        draw = codeExecutor([[
            function visit(x, y, visited)
              if x < 0 or x >= width or y < 0 or y >= height then
                return
              end
              local index = x + y * width
              if visited[index] or blocked[index] then
                return
              end
              visited[index] = true
              visit(x - 1, y, visited)
              visit(x + 1, y, visited)
              visit(x, y + 1, visited)
              visit(x, y - 1, visited)
            end
            visit(16, 9, {})]], 0, 0, GetScreenSize(), nil, -20,
            {
                blocked = GetBlockedMap(map),
                width = mapWidth,
                height = mapHeight
            },
            function(step, stackinfo)
                local prevX, prevY
                local visited
                for i=1,#stackinfo do
                    local info = stackinfo[i]
                    local currentX, currentY
                    for j=1, #info.locals do
                        local localInfo = info.locals[j]
                        if localInfo.name == "x" then
                            currentX = localInfo.value
                        elseif localInfo.name == "y" then
                            currentY = localInfo.value
                        elseif localInfo.name == "visited" then
                            visited = localInfo.value
                        end
                    end
                    if currentX and currentY then
                        local cx, cy = currentX * 32, currentY * 32
                        
                        if prevX and prevY then
                            local px, py = prevX * 32, prevY * 32
                            SetColor(0,0,0,255)
                            DrawLine(px, py, cx, cy, 5)
                        end
                        SetColor(255,0,0,255)
                        DrawRectangle(cx - 4, cy - 4, 8, 8)
                        prevX, prevY = currentX, currentY
                    end
                end
                if visited then
                    for y=0,mapHeight - 1 do
                        for x=0,mapWidth - 1 do
                            local index = x + y * mapWidth
                            if visited[index] then
                                SetColor(100,0,200,128)
                                DrawRectangle(x * 32 - 16, y * 32 - 16, 32, 32)
                            end
                        end
                    end
                end
            end)
    },
    {
        step = 2,
        activate = function() codeExecutorShowLine = 1 end,
        draw = speechBubblePointAt([[
            For the purpose of this demonstration, we
            use [color=008f]Lua[/color] as a scripting language. This way
            we can show the algorithm in action.

            We start out with a function called [color=808f]visit[/color].
            But this is only the function declaration.]], 400, 0, 330, nil, 0, -16, 20)
    },
}

-- the steps are relative to each chapter. Let's calculate the absolute
-- step positions here
local chapterOffset = 0
local maxStepInChaper = 0
local chapters = {}
for i, step in ipairs(steps) do
    if step.chapter then
        chapterOffset = maxStepInChaper
        chapters[#chapters+1] = step
        step.chapterIndex = #chapters
    end
    if type(step.step) == "table" then
        step.step = {step.step[1] + chapterOffset, step.step[2] + chapterOffset}
        maxStepInChaper = math.max(maxStepInChaper, step.step[2] + 1)
    elseif type(step.step) == "number" then
        step.step = step.step + chapterOffset
        maxStepInChaper = math.max(maxStepInChaper, step.step + 1)
    end
    step.currentChapter = chapters[#chapters]
end

local frame = 0
local showMenu = false
function draw(dt)
    frame = frame + 1
    SetColor(255,255,255,255)

    map:draw(0,0)
    overlay:draw(0,0)
    guyDraw()
    local currentStep = GetCurrentStepIndex()
    local currentChapter = chapters[1]
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
            if step.draw then
                step:draw()
            end
            step.activeTime = step.activeTime + dt
            currentChapter = step.currentChapter
        elseif step.lastActiveFrame == frame and step.deactivate then
            step:deactivate()
        end
    end

    if IsMenuKeyPressed() then
        showMenu = not showMenu
    end

    guyUpdate(dt)

    local w,h = GetScreenSize()
    local info_w = 400
    local info_h = #chapters * 20 + 80
    local info_y = showMenu and (h - info_h + 10) or (h - 30)
    
    SetColor(0,0,0,255)
    DrawBubble((w-info_w) / 2-1, info_y-1, info_w+2, info_h+2, -1, 0,0)
    SetColor(250,200,100,255)
    DrawBubble((w-info_w) / 2, info_y, info_w, info_h, -1, 0,0)
    SetColor(0,0,0,255)
    local currentChapterText = ("Chapter %d: %s"):format(currentChapter.chapterIndex, currentChapter.chapter)
    DrawTextBoxAligned(currentChapterText, 20, (w-info_w) / 2 + 10, info_y + 2, info_w, 30, 0, 0.5)

    if showMenu then
        SetColor(0,0,0,255)
        DrawRectangle((w-info_w) / 2, info_y + 30, info_w, 2)
        for i=1,#chapters do
            local chapter = chapters[i]
            local y = info_y + 20 * i + 15
            SetColor(0,0,0,255)
            local chapterText = ("%d: %s"):format(i, chapter.chapter)
            DrawTextBoxAligned(chapterText, 20, (w-info_w) / 2 + 10, y, info_w, 30, 0, 0.5)
        end
    end

    if frame < 2 then
        guyPos.x = guyTarget.x
        guyPos.y = guyTarget.y
    end
end