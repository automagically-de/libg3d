radius = 100
height = 20

o = g3d.Object()
o:setName("color wheel")

for i = 0, 359 do
	x = math.sin(math.rad(i)) * radius / math.pi
	y = math.cos(math.rad(i)) * radius / math.pi
	o:addVertex(x, y, 0)
	o:addVertex(x, y, height)

	f = g3d.Face(
		i * 2,
		i * 2 + 1,
		((i + 1) % 360) * 2 + 1,
		((i + 1) % 360) * 2)

	m = g3d.Material()
	r = math.sin(math.rad(i / 2))
	g = math.sin(math.rad((i + 120) % 360 / 2))
	b = math.sin(math.rad((i + 240) % 360 / 2))
	m:setColor(r, g, b)

	f:setMaterial(m)
	o:addFace(f)
end

g3d.model:addObject(o)
