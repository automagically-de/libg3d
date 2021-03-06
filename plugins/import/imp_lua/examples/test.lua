
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

c = g3d.Object()
c:setName("Container object")

o = g3d.Object()
o:setName('bla bla')
c:addObject(o)

o:addVertex(-1, 1, 0)
o:addVertex(1, 1, 0)
o:addVertex(1, -1, 0)
o:addVertex(-1, -1, 0)

m = g3d.Material()
m:setColor(0.2, 0.8, 0.9)
m:setAlpha(0.7)

f = g3d.Face(0, 1, 2)
f:setMaterial(m)
o:addFace(f)

f = g3d.Face(2, 3, 0)
f:setMaterial(m)
o:addFace(f)

g3d.model:addObject(c)

x = g3d.Matrix()
x:translate(1, 0, 0)
x:rotate(math.rad(90), 0, 1, 0)
o:transform(x)

for x = 0, 2 do
	for y = 0, 2 do
		o = g3d.Object('gauss.lua')
		-- o:setName("gauss @ "..x..", "..y)
		g3d.model:addObject(o)
		mtx = g3d.Matrix()
		mtx:translate(x * 30, 0, y * 30)
		o:transform(mtx)
	end
end

dump_o(o, "G3DObject")
dump_o(f, "G3DFace")
dump_o(m, "G3DMaterial")
dump_o(x, "G3DMatrix")
dump_o(g3d.model, "g3d.model");

