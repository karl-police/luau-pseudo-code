local test = nil

function input(a)
	test = a or 5
end


input()
input(1)

function input2(b)
	test = (b and true) or 1
end