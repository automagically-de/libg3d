
for i = 1, 3 do
	g3d.debug('|Hello World ' .. i)
end

g3d.debug('bla blubb')

o = g3d.Object()
for k, v in pairs(o) do
	g3d.debug("\\" .. type(v) .. " " .. k)
end

o:setName('bla bla')

f = {}
f.vertices = {{1, 2, 3}, {5, 6, 7}}
f.vertices[3] = {7, 8 ,9}

for k, v in pairs(f.vertices) do
	g3d.debug('|x='..v[1]..', y='..v[2]..', z='..v[3])
end
