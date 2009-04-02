
function dump_o(object, name)
	g3d.debug("\\dumping object '" .. name .. "':")
	for k, v in pairs(object) do
		g3d.debug("\\ " .. type(v) .. " " .. k)
	end
end

for i = 1, 3 do
	g3d.debug('|Hello World ' .. i)
end

g3d.debug('bla blubb')

o = g3d.Object()

o:setName('bla bla')
g3d.debug("|vertex count: " .. o:addVertex(0.1, 2, 4))
g3d.debug("|vertex count: " .. o:addVertex(1, .2, 4))

m = g3d.Material()

f = g3d.Face()
f:setMaterial(m)

dump_o(o, "G3DObject")
dump_o(f, "G3DFace")
dump_o(m, "G3DMaterial")
dump_o(g3d.model, "g3d.model");

