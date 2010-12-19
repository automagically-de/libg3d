ry = 10
rr = 1

sy = 36
sr = 12

o = g3d.Object()
o:setName("Torus")

i = g3d.Image('checker.png')
m = g3d.Material()
--m:setColor(0.8, 0.2, 0.2)
m:setTexture(i)

normals = {}

-- vertices
for i0 = 0, (sy - 1) do
	ay = i0 * (360 / sy)

	for i1 = 0, (sr - 1) do
		ar = i1 * (360 / sr)

		fx = math.sin(math.rad(ar)) * rr
		fy = math.cos(math.rad(ar)) * rr

		x = math.sin(math.rad(ay)) * (ry + fx)
		z = math.cos(math.rad(ay)) * (ry + fx)

		o:addVertex(x, fy, z)

		i = i0 * sr + i1
		v = g3d.Vector(x, fy, z)
		v:unify()
		normals[i] = v
	end
end

-- faces
for i0 = 0, (sy - 1) do
	for i1 = 0, (sr - 1) do
		p0 = i0 * sr + i1
		p1 = i0 * sr + ((i1 + 1) % sr)
		p2 = ((i0 + 1) % sy) * sr + ((i1 + 1) % sr)
		p3 = ((i0 + 1) % sy) * sr + i1

		f = g3d.Face(p0, p1, p2, p3)
		f:setMaterial(m)

		f:addTexVertex((i0 / sy),       (i1 / sr))
		f:addTexVertex((i0 / sy),       ((i1 + 1) / sr))
		f:addTexVertex(((i0 + 1) / sy), ((i1 + 1) / sr))
		f:addTexVertex(((i0 + 1) / sy), (i1 / sr))

		f:setNormal(0, normals[p0].x, normals[p0].y, normals[p0].z)
		f:setNormal(1, normals[p1].x, normals[p1].y, normals[p1].z)
		f:setNormal(2, normals[p2].x, normals[p2].y, normals[p2].z)
		f:setNormal(3, normals[p3].x, normals[p3].y, normals[p3].z)

		o:addFace(f)
	end
end

g3d.model:addObject(o)
