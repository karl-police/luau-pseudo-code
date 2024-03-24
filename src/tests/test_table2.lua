local tbl = {
	value = "hello";
}

tbl.value = "overwritten"

tbl.newValue = "new value"


local abc = tbl

local abc = table.clone(tbl)