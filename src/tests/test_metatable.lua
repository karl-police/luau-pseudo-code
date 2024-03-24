local test = {}
test.__index = test


function test:Get()
	return value
end


function test.new()
	local self = setmetatable({}, test)
	
	self.value = "hello"
	
	return self
end



local newTest = test.new()

print(newTest:Get())