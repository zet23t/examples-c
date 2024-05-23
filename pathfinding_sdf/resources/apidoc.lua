---Sets the current clear color
---@param r integer 0-255
---@param g integer 0-255
---@param b integer 0-255
---@param a integer 0-255
function SetClearColor(r,g,b,a) end

---Sets the current color
---@param r integer 0-255
---@param g integer 0-255
---@param b integer 0-255
---@param a integer 0-255
function SetColor(r,g,b,a) end

---Draws a rectangle using the current color
---@param x integer
---@param y integer
---@param w integer
---@param h integer
function DrawRectangle(x,y,w,h) end

---Draws a speech bubble
---@param x integer
---@param y integer
---@param w integer
---@param h integer
---@param arrow_type 0|1|2|3|4 
---@param arrow_x integer relative to x
---@param arrow_y integer relative to y
function DrawBubble(x,y,w,h, arrow_type, arrow_x, arrow_y) end

---Draws a text within the specified rectangle
---@param text string
---@param fontsize integer
---@param x integer
---@param y integer
---@param w integer
---@param h integer
---@param align_x number
---@param align_y number
---@return number x, number y, number width, number height the actual text boundaries drawn
function DrawTextBoxAligned(text, fontsize, x, y, w, h, align_x, align_y) return 0,0,0,0 end

function IsNextPagePressed() return false end
function IsPreviousPagePressed() return false end
function GetCurrentStepIndex() return 0 end