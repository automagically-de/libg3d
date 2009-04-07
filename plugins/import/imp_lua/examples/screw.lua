function screw(obj, mat, n, astep, inc, ri, ro)
	for i = 0, n do
		angle = math.rad((i * astep) % 360)
		y = i * inc
		x = math.sin(angle)
		z = math.cos(angle)
		obj:addVertex(x * ri, y, z * ri)
		obj:addVertex(x * ro, y, z * ro)
		if(i < n) then
			f = g3d.Face(
				i * 2,
				i * 2 + 1,
				(i + 1) * 2 + 1,
				(i + 1) * 2)
			f:setMaterial(mat)
			obj:addFace(f)
		end
	end
end

screws = {
	{ "red",   0.8, 0.2, 0.2,   0 },
	{ "green", 0.2, 0.8, 0.2, 120 },
	{ "blue",  0.2, 0.2, 0.8, 240 }
}

for i, s in ipairs(screws) do
	o = g3d.Object()
	o:setName("screw " .. s[1])
	m = g3d.Material()
	m:setColor(s[2], s[3], s[4])
	m:setAlpha(0.6)
	screw(o, m, 500, 10, 10, 600, 900)
	x = g3d.Matrix()
	x:rotateXYZ(0, math.rad(s[5]), 0)
	o:transform(x)
	g3d.model:addObject(o)
end

