--[[
    ╔═══════════════════════════════════════════════════════════════╗
    ║           STEAL A BRAINROT - ПОЛНЫЙ СКРИПТ                    ║
    ║                   Версия: 2.0 Premium                         ║
    ║                   Дата: 2025                                  ║
    ╚═══════════════════════════════════════════════════════════════╝
    
    ФУНКЦИОНАЛ:
    ✓ Auto Steal - Автоматическая кража брейнротов
    ✓ Auto Buy - Автоматическая покупка
    ✓ Auto Lock Base - Защита базы
    ✓ Server Hop - Поиск лучших серверов
    ✓ Brainrot Finder - Поиск редких брейнротов
    ✓ ESP - Подсветка игроков и предметов
    ✓ Teleport - Телепортация
    ✓ Speed Boost - Увеличение скорости
    ✓ Anti-Ragdoll - Защита от падений
    ✓ Infinite Jump - Бесконечный прыжок
    ✓ NoClip - Прохождение через стены
    ✓ Auto Farm Money - Автофарм денег
    
    ПРЕДУПРЕЖДЕНИЕ:
    Использование этого скрипта может нарушить ToS Roblox
    и привести к бану аккаунта. Используйте на свой риск!
]]--

-- ════════════════════════════════════════════════════════════════
-- ИНИЦИАЛИЗАЦИЯ СЕРВИСОВ
-- ════════════════════════════════════════════════════════════════

local Players = game:GetService("Players")
local ReplicatedStorage = game:GetService("ReplicatedStorage")
local RunService = game:GetService("RunService")
local TeleportService = game:GetService("TeleportService")
local HttpService = game:GetService("HttpService")
local UserInputService = game:GetService("UserInputService")
local Workspace = game:GetService("Workspace")
local Lighting = game:GetService("Lighting")

local LocalPlayer = Players.LocalPlayer
local PlayerGui = LocalPlayer:WaitForChild("PlayerGui")

-- ════════════════════════════════════════════════════════════════
-- НАСТРОЙКИ
-- ════════════════════════════════════════════════════════════════

local Config = {
    -- Основные
    AutoSteal = false,
    AutoBuy = false,
    AutoLockBase = false,
    AutoFarm = false,
    
    -- Визуальные
    ESP_Enabled = false,
    ESP_Players = true,
    ESP_Brainrots = true,
    ESP_Distance = 1000,
    
    -- Передвижение
    Speed = 16, -- Стандартная скорость
    SpeedEnabled = false,
    InfiniteJump = false,
    NoClip = false,
    
    -- Серверы
    ServerHop = false,
    MinPlayers = 5,
    MaxPing = 200,
    
    -- Брейнроты
    TargetBrainrot = "Any", -- "Rare", "Epic", "Legendary", "Any"
    MinBrainrotValue = 1000,
    
    -- Защита
    AntiRagdoll = true,
    AntiKick = true,
    
    -- UI
    UIVisible = true,
}

-- ════════════════════════════════════════════════════════════════
-- ПЕРЕМЕННЫЕ
-- ════════════════════════════════════════════════════════════════

local Character = LocalPlayer.Character or LocalPlayer.CharacterAdded:Wait()
local HumanoidRootPart = Character:WaitForChild("HumanoidRootPart")
local Humanoid = Character:WaitForChild("Humanoid")

local ESPObjects = {}
local Connections = {}
local OriginalSpeed = 16

-- ════════════════════════════════════════════════════════════════
-- УТИЛИТЫ
-- ════════════════════════════════════════════════════════════════

local function Notify(title, text, duration)
    game.StarterGui:SetCore("SendNotification", {
        Title = title or "Steal Brainrot",
        Text = text or "Уведомление",
        Duration = duration or 3,
        Icon = "rbxassetid://1234567890"
    })
end

local function GetDistance(part1, part2)
    if not part1 or not part2 then return math.huge end
    return (part1.Position - part2.Position).Magnitude
end

local function GetPlayerBase(player)
    -- Находим базу игрока
    local bases = Workspace:FindFirstChild("Bases")
    if bases then
        return bases:FindFirstChild(player.Name)
    end
    return nil
end

local function FindBrainrots()
    -- Поиск всех брейнротов на карте
    local brainrots = {}
    for _, v in pairs(Workspace:GetDescendants()) do
        if v:IsA("Model") and (v.Name:lower():find("brainrot") or v.Name:lower():find("brain")) then
            table.insert(brainrots, v)
        end
    end
    return brainrots
end

local function GetBrainrotValue(brainrot)
    -- Определяем ценность брейнрота
    if brainrot:FindFirstChild("Value") then
        return brainrot.Value.Value
    end
    
    -- Определяем по имени/редкости
    local name = brainrot.Name:lower()
    if name:find("legendary") or name:find("gold") then
        return 100000
    elseif name:find("epic") or name:find("purple") then
        return 50000
    elseif name:find("rare") or name:find("blue") then
        return 10000
    else
        return 1000
    end
end

local function GetBrainrotRarity(brainrot)
    local name = brainrot.Name:lower()
    if name:find("legendary") or name:find("gold") then
        return "Legendary"
    elseif name:find("epic") or name:find("purple") then
        return "Epic"
    elseif name:find("rare") or name:find("blue") then
        return "Rare"
    else
        return "Common"
    end
end

-- ════════════════════════════════════════════════════════════════
-- ESP СИСТЕМА
-- ════════════════════════════════════════════════════════════════

local function CreateESP(object, color, text)
    local billboardGui = Instance.new("BillboardGui")
    billboardGui.Name = "ESP"
    billboardGui.Adornee = object
    billboardGui.Size = UDim2.new(0, 100, 0, 50)
    billboardGui.StudsOffset = Vector3.new(0, 2, 0)
    billboardGui.AlwaysOnTop = true
    billboardGui.Parent = object
    
    local frame = Instance.new("Frame")
    frame.Size = UDim2.new(1, 0, 1, 0)
    frame.BackgroundTransparency = 0.7
    frame.BackgroundColor3 = color
    frame.BorderSizePixel = 0
    frame.Parent = billboardGui
    
    local textLabel = Instance.new("TextLabel")
    textLabel.Size = UDim2.new(1, 0, 1, 0)
    textLabel.BackgroundTransparency = 1
    textLabel.Text = text
    textLabel.TextColor3 = Color3.new(1, 1, 1)
    textLabel.TextScaled = true
    textLabel.Font = Enum.Font.GothamBold
    textLabel.Parent = frame
    
    local uiCorner = Instance.new("UICorner")
    uiCorner.CornerRadius = UDim.new(0, 8)
    uiCorner.Parent = frame
    
    table.insert(ESPObjects, billboardGui)
    return billboardGui
end

local function UpdateESP()
    -- Очистка старых ESP
    for _, esp in pairs(ESPObjects) do
        if esp then esp:Destroy() end
    end
    ESPObjects = {}
    
    if not Config.ESP_Enabled then return end
    
    -- ESP для игроков
    if Config.ESP_Players then
        for _, player in pairs(Players:GetPlayers()) do
            if player ~= LocalPlayer and player.Character then
                local hrp = player.Character:FindFirstChild("HumanoidRootPart")
                if hrp and GetDistance(HumanoidRootPart, hrp) < Config.ESP_Distance then
                    local distance = math.floor(GetDistance(HumanoidRootPart, hrp))
                    CreateESP(hrp, Color3.fromRGB(255, 0, 0), player.Name .. "\n" .. distance .. "m")
                end
            end
        end
    end
    
    -- ESP для брейнротов
    if Config.ESP_Brainrots then
        local brainrots = FindBrainrots()
        for _, brainrot in pairs(brainrots) do
            local primaryPart = brainrot.PrimaryPart or brainrot:FindFirstChildWhichIsA("BasePart")
            if primaryPart and GetDistance(HumanoidRootPart, primaryPart) < Config.ESP_Distance then
                local value = GetBrainrotValue(brainrot)
                local rarity = GetBrainrotRarity(brainrot)
                local distance = math.floor(GetDistance(HumanoidRootPart, primaryPart))
                
                local color = Color3.fromRGB(255, 255, 255)
                if rarity == "Legendary" then
                    color = Color3.fromRGB(255, 215, 0)
                elseif rarity == "Epic" then
                    color = Color3.fromRGB(138, 43, 226)
                elseif rarity == "Rare" then
                    color = Color3.fromRGB(0, 112, 221)
                end
                
                CreateESP(primaryPart, color, rarity .. " Brainrot\n💰 " .. value .. "\n📏 " .. distance .. "m")
            end
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ AUTO STEAL
-- ════════════════════════════════════════════════════════════════

local function AutoSteal()
    if not Config.AutoSteal then return end
    
    local brainrots = FindBrainrots()
    local bestBrainrot = nil
    local bestValue = 0
    local bestDistance = math.huge
    
    -- Находим лучший брейнрот
    for _, brainrot in pairs(brainrots) do
        local value = GetBrainrotValue(brainrot)
        local rarity = GetBrainrotRarity(brainrot)
        local primaryPart = brainrot.PrimaryPart or brainrot:FindFirstChildWhichIsA("BasePart")
        
        if primaryPart then
            local distance = GetDistance(HumanoidRootPart, primaryPart)
            
            -- Проверяем условия
            if value >= Config.MinBrainrotValue then
                if Config.TargetBrainrot == "Any" or Config.TargetBrainrot == rarity then
                    -- Приоритет: высокая ценность и близкое расстояние
                    local score = value / (distance + 1)
                    local bestScore = bestValue / (bestDistance + 1)
                    
                    if score > bestScore then
                        bestBrainrot = brainrot
                        bestValue = value
                        bestDistance = distance
                    end
                end
            end
        end
    end
    
    -- Крадем лучший брейнрот
    if bestBrainrot then
        local primaryPart = bestBrainrot.PrimaryPart or bestBrainrot:FindFirstChildWhichIsA("BasePart")
        if primaryPart then
            -- Телепортация к брейнроту
            HumanoidRootPart.CFrame = primaryPart.CFrame + Vector3.new(0, 3, 0)
            wait(0.5)
            
            -- Попытка взять брейнрот
            local tool = bestBrainrot:FindFirstChildWhichIsA("Tool") or bestBrainrot
            if tool then
                -- Используем RemoteEvent/RemoteFunction для кражи
                local remotes = ReplicatedStorage:GetDescendants()
                for _, remote in pairs(remotes) do
                    if remote:IsA("RemoteEvent") and (remote.Name:lower():find("steal") or remote.Name:lower():find("take")) then
                        pcall(function()
                            remote:FireServer(tool)
                        end)
                    elseif remote:IsA("RemoteFunction") and (remote.Name:lower():find("steal") or remote.Name:lower():find("take")) then
                        pcall(function()
                            remote:InvokeServer(tool)
                        end)
                    end
                end
                
                Notify("Auto Steal", "🎯 Украден " .. GetBrainrotRarity(bestBrainrot) .. " брейнрот!\n💰 Ценность: " .. bestValue, 3)
            end
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ AUTO BUY
-- ════════════════════════════════════════════════════════════════

local function AutoBuy()
    if not Config.AutoBuy then return end
    
    -- Поиск магазина или UI покупок
    local shopUI = PlayerGui:FindFirstChild("Shop") or PlayerGui:FindFirstChild("Store")
    if shopUI then
        -- Ищем RemoteEvent для покупки
        local remotes = ReplicatedStorage:GetDescendants()
        for _, remote in pairs(remotes) do
            if remote:IsA("RemoteEvent") and (remote.Name:lower():find("buy") or remote.Name:lower():find("purchase")) then
                pcall(function()
                    -- Покупаем самый дешевый доступный брейнрот
                    remote:FireServer("Brainrot1", 1) -- Пример
                end)
                Notify("Auto Buy", "✅ Куплен новый брейнрот!", 2)
            end
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ AUTO LOCK BASE
-- ════════════════════════════════════════════════════════════════

local function AutoLockBase()
    if not Config.AutoLockBase then return end
    
    local myBase = GetPlayerBase(LocalPlayer)
    if myBase then
        -- Ищем замок или дверь
        local door = myBase:FindFirstChild("Door") or myBase:FindFirstChild("Lock")
        if door then
            local remotes = ReplicatedStorage:GetDescendants()
            for _, remote in pairs(remotes) do
                if remote:IsA("RemoteEvent") and (remote.Name:lower():find("lock") or remote.Name:lower():find("door")) then
                    pcall(function()
                        remote:FireServer(true) -- Закрываем дверь
                    end)
                end
            end
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ AUTO FARM MONEY
-- ════════════════════════════════════════════════════════════════

local function AutoFarmMoney()
    if not Config.AutoFarm then return end
    
    -- Собираем монеты или ресурсы
    for _, v in pairs(Workspace:GetDescendants()) do
        if v:IsA("BasePart") and (v.Name:lower():find("coin") or v.Name:lower():find("money") or v.Name:lower():find("cash")) then
            if GetDistance(HumanoidRootPart, v) < 100 then
                -- Телепортируемся к монете
                HumanoidRootPart.CFrame = v.CFrame
                wait(0.1)
            end
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ SERVER HOP
-- ════════════════════════════════════════════════════════════════

local function ServerHop()
    local PlaceId = game.PlaceId
    local servers = {}
    
    -- Получаем список серверов
    local success, result = pcall(function()
        return HttpService:JSONDecode(game:HttpGet("https://games.roblox.com/v1/games/" .. PlaceId .. "/servers/Public?sortOrder=Asc&limit=100"))
    end)
    
    if success and result.data then
        for _, server in pairs(result.data) do
            if server.playing >= Config.MinPlayers and server.playing < server.maxPlayers and server.ping < Config.MaxPing then
                table.insert(servers, server.id)
            end
        end
        
        if #servers > 0 then
            local randomServer = servers[math.random(1, #servers)]
            Notify("Server Hop", "🔄 Перемещение на новый сервер...", 2)
            TeleportService:TeleportToPlaceInstance(PlaceId, randomServer, LocalPlayer)
        else
            Notify("Server Hop", "❌ Подходящих серверов не найдено", 3)
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ BRAINROT FINDER
-- ════════════════════════════════════════════════════════════════

local function BrainrotFinder()
    local brainrots = FindBrainrots()
    local rareBrainrots = {}
    
    for _, brainrot in pairs(brainrots) do
        local rarity = GetBrainrotRarity(brainrot)
        if rarity == "Legendary" or rarity == "Epic" then
            table.insert(rareBrainrots, {
                brainrot = brainrot,
                rarity = rarity,
                value = GetBrainrotValue(brainrot)
            })
        end
    end
    
    if #rareBrainrots > 0 then
        local info = "🔍 Найдены редкие брейнроты:\n"
        for i, data in pairs(rareBrainrots) do
            if i <= 3 then -- Показываем только топ 3
                info = info .. data.rarity .. " (💰 " .. data.value .. ")\n"
            end
        end
        Notify("Brainrot Finder", info, 5)
        return true
    else
        Notify("Brainrot Finder", "❌ Редкие брейнроты не найдены\n🔄 Попробуйте Server Hop", 3)
        return false
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИИ ПЕРЕДВИЖЕНИЯ
-- ════════════════════════════════════════════════════════════════

local function ToggleSpeed()
    if Config.SpeedEnabled then
        Humanoid.WalkSpeed = Config.Speed
    else
        Humanoid.WalkSpeed = OriginalSpeed
    end
end

local function ToggleInfiniteJump()
    if Config.InfiniteJump then
        Connections.InfiniteJump = UserInputService.JumpRequest:Connect(function()
            Humanoid:ChangeState(Enum.HumanoidStateType.Jumping)
        end)
    else
        if Connections.InfiniteJump then
            Connections.InfiniteJump:Disconnect()
        end
    end
end

local NoClipConnection = nil
local function ToggleNoClip()
    if Config.NoClip then
        NoClipConnection = RunService.Stepped:Connect(function()
            for _, part in pairs(Character:GetDescendants()) do
                if part:IsA("BasePart") then
                    part.CanCollide = false
                end
            end
        end)
    else
        if NoClipConnection then
            NoClipConnection:Disconnect()
            NoClipConnection = nil
        end
        for _, part in pairs(Character:GetDescendants()) do
            if part:IsA("BasePart") then
                part.CanCollide = true
            end
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ ANTI-RAGDOLL
-- ════════════════════════════════════════════════════════════════

local function SetupAntiRagdoll()
    if Config.AntiRagdoll then
        Humanoid.StateChanged:Connect(function(oldState, newState)
            if newState == Enum.HumanoidStateType.Ragdoll or newState == Enum.HumanoidStateType.FallingDown then
                Humanoid:ChangeState(Enum.HumanoidStateType.GettingUp)
            end
        end)
    end
end

-- ════════════════════════════════════════════════════════════════
-- ФУНКЦИЯ ТЕЛЕПОРТАЦИИ
-- ════════════════════════════════════════════════════════════════

local function TeleportToPlayer(playerName)
    local targetPlayer = Players:FindFirstChild(playerName)
    if targetPlayer and targetPlayer.Character then
        local targetHRP = targetPlayer.Character:FindFirstChild("HumanoidRootPart")
        if targetHRP then
            HumanoidRootPart.CFrame = targetHRP.CFrame + Vector3.new(5, 0, 5)
            Notify("Teleport", "✈️ Телепортация к " .. playerName, 2)
        end
    end
end

local function TeleportToBase()
    local myBase = GetPlayerBase(LocalPlayer)
    if myBase then
        local basePart = myBase:FindFirstChildWhichIsA("BasePart")
        if basePart then
            HumanoidRootPart.CFrame = basePart.CFrame + Vector3.new(0, 5, 0)
            Notify("Teleport", "🏠 Телепортация на базу", 2)
        end
    end
end

-- ════════════════════════════════════════════════════════════════
-- СОЗДАНИЕ GUI
-- ════════════════════════════════════════════════════════════════

local function CreateGUI()
    -- Создаем ScreenGui
    local ScreenGui = Instance.new("ScreenGui")
    ScreenGui.Name = "StealBrainrotGUI"
    ScreenGui.ResetOnSpawn = false
    ScreenGui.Parent = PlayerGui
    
    -- Главный фрейм
    local MainFrame = Instance.new("Frame")
    MainFrame.Name = "MainFrame"
    MainFrame.Size = UDim2.new(0, 450, 0, 600)
    MainFrame.Position = UDim2.new(0.5, -225, 0.5, -300)
    MainFrame.BackgroundColor3 = Color3.fromRGB(25, 25, 35)
    MainFrame.BorderSizePixel = 0
    MainFrame.Active = true
    MainFrame.Draggable = true
    MainFrame.Parent = ScreenGui
    
    local MainCorner = Instance.new("UICorner")
    MainCorner.CornerRadius = UDim.new(0, 12)
    MainCorner.Parent = MainFrame
    
    -- Заголовок
    local Title = Instance.new("TextLabel")
    Title.Name = "Title"
    Title.Size = UDim2.new(1, -20, 0, 50)
    Title.Position = UDim2.new(0, 10, 0, 10)
    Title.BackgroundTransparency = 1
    Title.Text = "🧠 STEAL A BRAINROT | PREMIUM v2.0"
    Title.TextColor3 = Color3.fromRGB(255, 255, 255)
    Title.TextSize = 20
    Title.Font = Enum.Font.GothamBold
    Title.Parent = MainFrame
    
    -- Линия разделитель
    local Divider = Instance.new("Frame")
    Divider.Size = UDim2.new(1, -20, 0, 2)
    Divider.Position = UDim2.new(0, 10, 0, 65)
    Divider.BackgroundColor3 = Color3.fromRGB(100, 100, 255)
    Divider.BorderSizePixel = 0
    Divider.Parent = MainFrame
    
    -- Скролл контейнер
    local ScrollFrame = Instance.new("ScrollingFrame")
    ScrollFrame.Size = UDim2.new(1, -20, 1, -80)
    ScrollFrame.Position = UDim2.new(0, 10, 0, 70)
    ScrollFrame.BackgroundTransparency = 1
    ScrollFrame.BorderSizePixel = 0
    ScrollFrame.ScrollBarThickness = 6
    ScrollFrame.ScrollBarImageColor3 = Color3.fromRGB(100, 100, 255)
    ScrollFrame.Parent = MainFrame
    
    local UIListLayout = Instance.new("UIListLayout")
    UIListLayout.Padding = UDim.new(0, 8)
    UIListLayout.Parent = ScrollFrame
    
    -- Функция создания переключателя
    local function CreateToggle(name, callback, defaultState)
        local ToggleFrame = Instance.new("Frame")
        ToggleFrame.Size = UDim2.new(1, -10, 0, 40)
        ToggleFrame.BackgroundColor3 = Color3.fromRGB(35, 35, 45)
        ToggleFrame.BorderSizePixel = 0
        ToggleFrame.Parent = ScrollFrame
        
        local ToggleCorner = Instance.new("UICorner")
        ToggleCorner.CornerRadius = UDim.new(0, 8)
        ToggleCorner.Parent = ToggleFrame
        
        local ToggleLabel = Instance.new("TextLabel")
        ToggleLabel.Size = UDim2.new(1, -60, 1, 0)
        ToggleLabel.Position = UDim2.new(0, 15, 0, 0)
        ToggleLabel.BackgroundTransparency = 1
        ToggleLabel.Text = name
        ToggleLabel.TextColor3 = Color3.fromRGB(255, 255, 255)
        ToggleLabel.TextSize = 14
        ToggleLabel.Font = Enum.Font.Gotham
        ToggleLabel.TextXAlignment = Enum.TextXAlignment.Left
        ToggleLabel.Parent = ToggleFrame
        
        local ToggleButton = Instance.new("TextButton")
        ToggleButton.Size = UDim2.new(0, 50, 0, 25)
        ToggleButton.Position = UDim2.new(1, -60, 0.5, -12.5)
        ToggleButton.BackgroundColor3 = defaultState and Color3.fromRGB(0, 200, 100) or Color3.fromRGB(150, 50, 50)
        ToggleButton.BorderSizePixel = 0
        ToggleButton.Text = defaultState and "ON" or "OFF"
        ToggleButton.TextColor3 = Color3.fromRGB(255, 255, 255)
        ToggleButton.TextSize = 12
        ToggleButton.Font = Enum.Font.GothamBold
        ToggleButton.Parent = ToggleFrame
        
        local ButtonCorner = Instance.new("UICorner")
        ButtonCorner.CornerRadius = UDim.new(0, 6)
        ButtonCorner.Parent = ToggleButton
        
        local state = defaultState
        ToggleButton.MouseButton1Click:Connect(function()
            state = not state
            ToggleButton.Text = state and "ON" or "OFF"
            ToggleButton.BackgroundColor3 = state and Color3.fromRGB(0, 200, 100) or Color3.fromRGB(150, 50, 50)
            callback(state)
        end)
        
        return ToggleFrame
    end
    
    -- Функция создания кнопки
    local function CreateButton(name, callback)
        local Button = Instance.new("TextButton")
        Button.Size = UDim2.new(1, -10, 0, 40)
        Button.BackgroundColor3 = Color3.fromRGB(60, 120, 255)
        Button.BorderSizePixel = 0
        Button.Text = name
        Button.TextColor3 = Color3.fromRGB(255, 255, 255)
        Button.TextSize = 14
        Button.Font = Enum.Font.GothamBold
        Button.Parent = ScrollFrame
        
        local ButtonCorner = Instance.new("UICorner")
        ButtonCorner.CornerRadius = UDim.new(0, 8)
        ButtonCorner.Parent = Button
        
        Button.MouseButton1Click:Connect(callback)
        
        return Button
    end
    
    -- Создаем секции
    local function CreateSection(name)
        local Section = Instance.new("TextLabel")
        Section.Size = UDim2.new(1, -10, 0, 30)
        Section.BackgroundTransparency = 1
        Section.Text = "═══ " .. name .. " ═══"
        Section.TextColor3 = Color3.fromRGB(150, 150, 200)
        Section.TextSize = 16
        Section.Font = Enum.Font.GothamBold
        Section.Parent = ScrollFrame
        return Section
    end
    
    -- ═══ ОСНОВНЫЕ ФУНКЦИИ ═══
    CreateSection("⚡ ОСНОВНЫЕ ФУНКЦИИ")
    
    CreateToggle("🎯 Auto Steal (Автокража)", function(state)
        Config.AutoSteal = state
        if state then
            Notify("Auto Steal", "✅ Включено", 2)
        end
    end, false)
    
    CreateToggle("💰 Auto Buy (Автопокупка)", function(state)
        Config.AutoBuy = state
        if state then
            Notify("Auto Buy", "✅ Включено", 2)
        end
    end, false)
    
    CreateToggle("🔒 Auto Lock Base (Защита базы)", function(state)
        Config.AutoLockBase = state
        if state then
            Notify("Auto Lock", "✅ База защищена", 2)
        end
    end, false)
    
    CreateToggle("🌾 Auto Farm Money", function(state)
        Config.AutoFarm = state
        if state then
            Notify("Auto Farm", "✅ Фарм запущен", 2)
        end
    end, false)
    
    -- ═══ ПОИСК И СЕРВЕРЫ ═══
    CreateSection("🔍 ПОИСК И СЕРВЕРЫ")
    
    CreateButton("🔍 Brainrot Finder (Найти редкие)", function()
        Notify("Brainrot Finder", "🔍 Поиск редких брейнротов...", 2)
        spawn(function()
            wait(1)
            BrainrotFinder()
        end)
    end)
    
    CreateButton("🔄 Server Hop (Сменить сервер)", function()
        Notify("Server Hop", "🔄 Поиск лучшего сервера...", 2)
        spawn(function()
            wait(1)
            ServerHop()
        end)
    end)
    
    CreateButton("🎲 Server Hop + Find Rare", function()
        Notify("Smart Hop", "🎲 Поиск сервера с редкими брейнротами...", 2)
        spawn(function()
            local attempts = 0
            local maxAttempts = 5
            
            while attempts < maxAttempts do
                wait(2)
                if BrainrotFinder() then
                    Notify("Smart Hop", "✅ Найден хороший сервер!", 3)
                    break
                else
                    attempts = attempts + 1
                    if attempts < maxAttempts then
                        Notify("Smart Hop", "🔄 Попытка " .. attempts .. "/" .. maxAttempts, 2)
                        ServerHop()
                        wait(15) -- Ждем загрузки нового сервера
                    else
                        Notify("Smart Hop", "❌ Не найдено за " .. maxAttempts .. " попыток", 3)
                    end
                end
            end
        end)
    end)
    
    -- ═══ ВИЗУАЛЬНЫЕ ═══
    CreateSection("👁️ ВИЗУАЛЬНЫЕ")
    
    CreateToggle("👁️ ESP Players (Подсветка игроков)", function(state)
        Config.ESP_Players = state
        Config.ESP_Enabled = Config.ESP_Players or Config.ESP_Brainrots
        UpdateESP()
    end, false)
    
    CreateToggle("🧠 ESP Brainrots (Подсветка брейнротов)", function(state)
        Config.ESP_Brainrots = state
        Config.ESP_Enabled = Config.ESP_Players or Config.ESP_Brainrots
        UpdateESP()
    end, false)
    
    -- ═══ ПЕРЕДВИЖЕНИЕ ═══
    CreateSection("🏃 ПЕРЕДВИЖЕНИЕ")
    
    CreateToggle("⚡ Speed Boost (x2 скорость)", function(state)
        Config.SpeedEnabled = state
        if state then
            Config.Speed = 32
        else
            Config.Speed = OriginalSpeed
        end
        ToggleSpeed()
    end, false)
    
    CreateToggle("🚀 Infinite Jump", function(state)
        Config.InfiniteJump = state
        ToggleInfiniteJump()
    end, false)
    
    CreateToggle("👻 NoClip (Прохождение сквозь стены)", function(state)
        Config.NoClip = state
        ToggleNoClip()
    end, false)
    
    -- ═══ ТЕЛЕПОРТАЦИЯ ═══
    CreateSection("✈️ ТЕЛЕПОРТАЦИЯ")
    
    CreateButton("🏠 Teleport to Base", function()
        TeleportToBase()
    end)
    
    -- ═══ ЗАЩИТА ═══
    CreateSection("🛡️ ЗАЩИТА")
    
    CreateToggle("🛡️ Anti-Ragdoll", function(state)
        Config.AntiRagdoll = state
        if state then
            SetupAntiRagdoll()
        end
    end, true)
    
    -- Кнопка закрытия
    local CloseButton = Instance.new("TextButton")
    CloseButton.Size = UDim2.new(0, 30, 0, 30)
    CloseButton.Position = UDim2.new(1, -40, 0, 10)
    CloseButton.BackgroundColor3 = Color3.fromRGB(200, 50, 50)
    CloseButton.BorderSizePixel = 0
    CloseButton.Text = "X"
    CloseButton.TextColor3 = Color3.fromRGB(255, 255, 255)
    CloseButton.TextSize = 18
    CloseButton.Font = Enum.Font.GothamBold
    CloseButton.Parent = MainFrame
    
    local CloseCorner = Instance.new("UICorner")
    CloseCorner.CornerRadius = UDim.new(0, 8)
    CloseCorner.Parent = CloseButton
    
    CloseButton.MouseButton1Click:Connect(function()
        ScreenGui:Destroy()
    end)
    
    -- Установка размера контента
    ScrollFrame.CanvasSize = UDim2.new(0, 0, 0, UIListLayout.AbsoluteContentSize.Y + 10)
    UIListLayout:GetPropertyChangedSignal("AbsoluteContentSize"):Connect(function()
        ScrollFrame.CanvasSize = UDim2.new(0, 0, 0, UIListLayout.AbsoluteContentSize.Y + 10)
    end)
end

-- ════════════════════════════════════════════════════════════════
-- ГЛАВНЫЙ ЦИКЛ
-- ════════════════════════════════════════════════════════════════

local function MainLoop()
    while wait(1) do
        pcall(function()
            -- Auto Steal
            if Config.AutoSteal then
                AutoSteal()
            end
            
            -- Auto Buy
            if Config.AutoBuy then
                AutoBuy()
            end
            
            -- Auto Lock Base
            if Config.AutoLockBase then
                AutoLockBase()
            end
            
            -- Auto Farm
            if Config.AutoFarm then
                AutoFarmMoney()
            end
            
            -- Update ESP
            if Config.ESP_Enabled then
                UpdateESP()
            end
        end)
    end
end

-- ════════════════════════════════════════════════════════════════
-- ЗАПУСК СКРИПТА
-- ════════════════════════════════════════════════════════════════

local function Initialize()
    -- Приветствие
    Notify("🧠 Steal Brainrot Script", "✅ Скрипт загружен успешно!\n\n📌 Версия: Premium 2.0\n🎯 Все функции активны", 5)
    
    -- Создаем GUI
    CreateGUI()
    
    -- Настраиваем защиту
    SetupAntiRagdoll()
    
    -- Запускаем главный цикл
    spawn(MainLoop)
    
    -- Обработчик переспавна
    LocalPlayer.CharacterAdded:Connect(function(char)
        Character = char
        HumanoidRootPart = char:WaitForChild("HumanoidRootPart")
        Humanoid = char:WaitForChild("Humanoid")
        
        wait(1)
        SetupAntiRagdoll()
        
        if Config.SpeedEnabled then
            ToggleSpeed()
        end
        
        if Config.InfiniteJump then
            ToggleInfiniteJump()
        end
        
        if Config.NoClip then
            ToggleNoClip()
        end
    end)
    
    print("═══════════════════════════════════════")
    print("    STEAL A BRAINROT SCRIPT LOADED    ")
    print("═══════════════════════════════════════")
end

-- Запуск
Initialize()
