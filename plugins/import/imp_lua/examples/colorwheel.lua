radius = 100
height = 50

o = g3d.Object()
o:setName("color wheel")
g3d.model:addObject(o)

for i = 0, 359 do
	x = math.sin(math.rad(i)) * radius
	z = math.cos(math.rad(i)) * radius
	o:addVertex(x, 0, z)
	o:addVertex(x, height, z)

	i2 = (i + 1) % 360
	f = g3d.Face(i * 2, i * 2 + 1, i2 * 2 + 1, i2 * 2)

	m = g3d.Material()
	r = math.sin(math.rad(i / 2))
	g = math.sin(math.rad((i + 120) % 360 / 2))
	b = math.sin(math.rad((i + 240) % 360 / 2))
	m:setColor(r, g, b)

	f:setMaterial(m)
	o:addFace(f)
end
