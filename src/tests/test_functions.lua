function abc()
	return 1
end


local abc2 = function()
	return 2
end


local returnedValue = (function()
	return 3
end)()



local test = 0

local function modify()
	test = "modified"
	
	if (test ~= 0) then
		print("It's not 0 anymore, value now is: ".. test)
	end
end

modify()



function arguments(a, b, c)
	a += 1
	b += 1
	c += 1

	print(a, b, c)
end


arguments(1,2,3)