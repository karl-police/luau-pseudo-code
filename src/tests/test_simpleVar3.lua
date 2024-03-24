local test = os.time()

local test2 = test

local test3 = test + test2
local test3 = test2 + test


print("test")

local test4 = 1

do
	local test5 = 2
	test4 = 2
end

if (true) then
	test4 = 2
end