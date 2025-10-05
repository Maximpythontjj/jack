-- SCARRY EXECUTOR Test Script
-- This script tests basic functionality of the executor

print("========================================")
print("   SCARRY EXECUTOR TEST SCRIPT")
print("========================================")
print("")

-- Test 1: Basic print functionality
print("✓ Test 1: Basic print functionality")
print("Hello from SCARRY EXECUTOR!")
print("Current time:", os.time())
print("")

-- Test 2: Roblox API access
print("✓ Test 2: Roblox API access")
local players = game:GetService("Players")
print("Players service found:", players ~= nil)

if players then
    local localPlayer = players.LocalPlayer
    if localPlayer then
        print("Local player name:", localPlayer.Name)
        print("Local player UserId:", localPlayer.UserId)
    else
        print("No local player found")
    end
end
print("")

-- Test 3: Workspace access
print("✓ Test 3: Workspace access")
local workspace = game:GetService("Workspace")
print("Workspace found:", workspace ~= nil)

if workspace then
    print("Workspace children count:", #workspace:GetChildren())
    print("Camera found:", workspace.CurrentCamera ~= nil)
end
print("")

-- Test 4: ReplicatedStorage access
print("✓ Test 4: ReplicatedStorage access")
local replicatedStorage = game:GetService("ReplicatedStorage")
print("ReplicatedStorage found:", replicatedStorage ~= nil)

if replicatedStorage then
    print("ReplicatedStorage children:", #replicatedStorage:GetChildren())
end
print("")

-- Test 5: Math operations
print("✓ Test 5: Math operations")
local x = 10
local y = 20
local sum = x + y
print("10 + 20 =", sum)

local multiply = x * y
print("10 * 20 =", multiply)
print("")

-- Test 6: Table operations
print("✓ Test 6: Table operations")
local testTable = {
    name = "ScarryTest",
    version = 1.0,
    features = {"print", "api_access", "math"}
}

print("Table name:", testTable.name)
print("Table version:", testTable.version)
print("Features count:", #testTable.features)

for i, feature in ipairs(testTable.features) do
    print("Feature " .. i .. ":", feature)
end
print("")

-- Test 7: Function calls
print("✓ Test 7: Function calls")
local function greet(name)
    return "Hello, " .. name .. "!"
end

print(greet("SCARRY EXECUTOR"))
print("")

-- Test 8: Error handling
print("✓ Test 8: Error handling")
local function safeDivide(a, b)
    if b == 0 then
        error("Division by zero!")
    end
    return a / b
end

local success, result = pcall(safeDivide, 10, 2)
if success then
    print("10 / 2 =", result)
else
    print("Error:", result)
end

local success2, result2 = pcall(safeDivide, 10, 0)
if success2 then
    print("10 / 0 =", result2)
else
    print("Error:", result2)
end
print("")

print("========================================")
print("   ALL TESTS COMPLETED")
print("========================================")
print("If you can see this, SCARRY EXECUTOR is working correctly!")